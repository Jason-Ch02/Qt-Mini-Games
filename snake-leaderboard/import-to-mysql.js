/**
 * ============================================
 * 贪吃蛇排行榜 - Railway → 本地 MySQL 同步工具
 * ============================================
 *
 * 功能：
 *   从 Railway 服务器拉取全量排行榜数据，导入本地 MySQL。
 *
 * 用法：
 *   1. 先执行 setup-mysql.sql 创建数据库
 *   2. 修改下方 DB_CONFIG 为你的 MySQL 连接信息
 *   3. 运行：node import-to-mysql.js
 *   4. 可配合 Windows 计划任务定期自动运行
 *
 * 数据安全：
 *   - 同名玩家保留最高分（ON DUPLICATE KEY UPDATE）
 *   - 每次提交记录都保存到 score_logs 表
 */

const https = require('https');
const fs = require('fs');
const mysql = require('mysql2/promise');

// ==================== 加载 .env 文件 ====================
function loadEnv(filePath) {
    if (!fs.existsSync(filePath)) return;
    const lines = fs.readFileSync(filePath, 'utf-8').split('\n');
    for (const line of lines) {
        const trimmed = line.trim();
        if (!trimmed || trimmed.startsWith('#')) continue;
        const eqIdx = trimmed.indexOf('=');
        if (eqIdx === -1) continue;
        const key = trimmed.slice(0, eqIdx).trim();
        const val = trimmed.slice(eqIdx + 1).trim();
        if (!process.env[key]) {
            process.env[key] = val;
        }
    }
}
loadEnv(__dirname + '/.env');

// ==================== 配置区 ====================

// Railway 服务器地址
const RAILWAY_URL = 'https://lively-balance-s.up.railway.app';

// 本地 MySQL 配置
const DB_CONFIG = {
    host: process.env.DB_HOST || '127.0.0.1',
    port: parseInt(process.env.DB_PORT) || 3306,
    user: process.env.DB_USER || 'root',
    password: process.env.DB_PASSWORD || 'YOUR_MYSQL_PASSWORD',  // ← 在 .env 中修改
    database: process.env.DB_NAME || 'snake_leaderboard',
};

// ==================== 主逻辑 ====================

/**
 * 从 Railway 获取全量数据
 */
function fetchFromRailway(endpoint) {
    return new Promise((resolve, reject) => {
        const url = `${RAILWAY_URL}${endpoint}`;
        https.get(url, (res) => {
            let data = '';
            res.on('data', chunk => data += chunk);
            res.on('end', () => {
                try {
                    resolve(JSON.parse(data));
                } catch (e) {
                    reject(new Error(`解析响应失败: ${data.slice(0, 200)}`));
                }
            });
        }).on('error', (e) => {
            reject(new Error(`请求失败: ${e.message}\n请确认 Railway 服务器可访问`));
        });
    });
}

async function main() {
    console.log('========================================');
    console.log('  贪吃蛇排行榜 - 数据同步工具');
    console.log('  Railway → 本地 MySQL');
    console.log('========================================\n');

    // 1. 从 Railway 拉取数据
    console.log('[1/4] 正在从 Railway 拉取全量数据...');
    let records;
    try {
        records = await fetchFromRailway('/export');
        console.log(`  ✓ 成功获取 ${records.length} 条记录\n`);
    } catch (e) {
        console.error(`  ✗ ${e.message}`);
        process.exit(1);
    }

    if (records.length === 0) {
        console.log('  ⚠ Railway 上暂无数据，无需同步。');
        process.exit(0);
    }

    // 打印数据预览
    console.log('  数据预览:');
    console.log('  ┌──────┬────────────────────┬─────────┬──────────────────────┐');
    console.log('  │ 排名 │ 玩家               │ 分数    │ 时间                 │');
    console.log('  ├──────┼────────────────────┼─────────┼──────────────────────┤');
    records.slice(0, 10).forEach((r, i) => {
        const name = r.name.padEnd(18);
        const score = String(r.score).padStart(7);
        console.log(`  │ ${String(i + 1).padStart(4)} │ ${name} │ ${score} │ ${r.time} │`);
    });
    if (records.length > 10) {
        console.log(`  │ ...  │ ... (共 ${records.length} 条)                     │`);
    }
    console.log('  └──────┴────────────────────┴─────────┴──────────────────────┘\n');

    // 2. 连接本地 MySQL
    console.log('[2/4] 正在连接本地 MySQL...');
    let connection;
    try {
        connection = await mysql.createConnection(DB_CONFIG);
        console.log('  ✓ MySQL 连接成功\n');
    } catch (e) {
        console.error(`  ✗ MySQL 连接失败: ${e.message}`);
        console.error('  请检查:');
        console.error('    1. MySQL 服务是否启动');
        console.error('    2. DB_CONFIG 中的密码是否正确');
        console.error('    3. 是否已执行 setup-mysql.sql');
        process.exit(1);
    }

    try {
        // 3. 同步到 scores 表（同名玩家保留最高分）
        console.log('[3/4] 正在同步到 scores 表...');
        let insertedCount = 0;
        let updatedCount = 0;

        for (const r of records) {
            const [result] = await connection.execute(
                `INSERT INTO scores (name, score, time)
                 VALUES (?, ?, ?)
                 ON DUPLICATE KEY UPDATE
                     score = IF(VALUES(score) > score, VALUES(score), score),
                     time  = IF(VALUES(score) > score, VALUES(time), time)`,
                [r.name, r.score, r.time]
            );
            if (result.affectedRows === 1) {
                insertedCount++;
            } else if (result.affectedRows === 2) {
                updatedCount++;
            }
        }
        console.log(`  ✓ 新增 ${insertedCount} 条，更新 ${updatedCount} 条\n`);

        // 4. 同步到 score_logs 表（保留每次提交记录）
        console.log('[4/4] 正在写入提交日志...');
        let logCount = 0;
        for (const r of records) {
            // 使用 INSERT IGNORE 避免重复日志
            const [result] = await connection.execute(
                `INSERT IGNORE INTO score_logs (name, score, time)
                 VALUES (?, ?, ?)`,
                [r.name, r.score, r.time]
            );
            if (result.affectedRows === 1) logCount++;
        }
        console.log(`  ✓ 写入 ${logCount} 条日志\n`);

        // 5. 验证结果
        const [rows] = await connection.execute(
            'SELECT COUNT(*) AS total FROM scores'
        );
        console.log('========================================');
        console.log(`  同步完成！本地共 ${rows[0].total} 条排行榜记录`);
        console.log('========================================\n');

    } finally {
        await connection.end();
    }
}

main().catch(e => {
    console.error('\n✗ 同步失败:', e.message);
    process.exit(1);
});
