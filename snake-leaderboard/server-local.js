/**
 * ============================================
 * 贪吃蛇排行榜 - 本地 MySQL 版服务器
 * ============================================
 *
 * 用途：Railway 停用后，在本地运行此服务器替代。
 *
 * 启动：node server-local.js
 * 默认端口：3000
 *
 * 与 Railway 版的区别：
 *   - 数据存储在本地 MySQL（而非 JSON 文件）
 *   - 数据不会因重启丢失
 *   - 排行榜查询更高效
 *
 * 客户端配置：
 *   将 C++ 代码中的 serverUrl 改为 http://你的局域网IP:3000
 *   或者使用 localhost:3000（仅本机）
 */

const express = require('express');
const cors = require('cors');
const fs = require('fs');
const mysql = require('mysql2/promise');
const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

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

// ==================== MySQL 配置 ====================
const DB_CONFIG = {
    host: process.env.DB_HOST || '127.0.0.1',
    port: parseInt(process.env.DB_PORT) || 3306,
    user: process.env.DB_USER || 'root',
    password: process.env.DB_PASSWORD || 'YOUR_MYSQL_PASSWORD',  // ← 在 .env 中修改
    database: process.env.DB_NAME || 'snake_leaderboard',
};

// ==================== 数据库连接池 ====================
let pool;
async function getPool() {
    if (!pool) {
        pool = mysql.createPool({
            ...DB_CONFIG,
            waitForConnections: true,
            connectionLimit: 10,
        });
    }
    return pool;
}

// ==================== 提交分数 ====================
app.post('/submit', async (req, res) => {
    const { name, score } = req.body;
    if (!name || typeof score !== 'number' || score <= 0) {
        return res.status(400).json({ error: '数据不合法' });
    }
    const cleanName = name.trim().slice(0, 20);
    const now = new Date().toLocaleString('zh-CN');

    try {
        const p = await getPool();

        // 1) 更新 scores 表（同名保留最高分）
        await p.execute(
            `INSERT INTO scores (name, score, time)
             VALUES (?, ?, ?)
             ON DUPLICATE KEY UPDATE
                 score = IF(VALUES(score) > score, VALUES(score), score),
                 time  = IF(VALUES(score) > score, VALUES(time), time)`,
            [cleanName, score, now]
        );

        // 2) 记录提交日志
        await p.execute(
            `INSERT INTO score_logs (name, score, time) VALUES (?, ?, ?)`,
            [cleanName, score, now]
        );

        // 3) 查询当前排名
        const [rows] = await p.execute(
            `SELECT COUNT(*) AS rank FROM scores WHERE score > ?`,
            [score]
        );
        const rank = rows[0].rank + 1;

        res.json({ status: 'ok', rank });
    } catch (e) {
        console.error('提交分数失败:', e.message);
        res.status(500).json({ error: '服务器内部错误' });
    }
});

// ==================== 实时排行榜（按最高分排序） ====================
app.get('/leaderboard', async (req, res) => {
    try {
        const p = await getPool();
        const [rows] = await p.execute(
            `SELECT name, score, time FROM scores ORDER BY score DESC LIMIT 10`
        );
        res.json(rows);
    } catch (e) {
        console.error('查询排行榜失败:', e.message);
        res.status(500).json({ error: '服务器内部错误' });
    }
});

// ==================== 历史总榜（同 leaderboard，基于 MySQL） ====================
app.get('/alltime', async (req, res) => {
    try {
        const p = await getPool();
        const [rows] = await p.execute(
            `SELECT name, score, time FROM scores ORDER BY score DESC LIMIT 10`
        );
        res.json(rows);
    } catch (e) {
        console.error('查询历史总榜失败:', e.message);
        res.status(500).json({ error: '服务器内部错误' });
    }
});

// ==================== 全量数据导出 ====================
app.get('/export', async (req, res) => {
    try {
        const p = await getPool();
        const [rows] = await p.execute(
            `SELECT name, score, time FROM scores ORDER BY score DESC`
        );
        res.json(rows);
    } catch (e) {
        console.error('导出数据失败:', e.message);
        res.status(500).json({ error: '服务器内部错误' });
    }
});

// ==================== 统计数据 ====================
app.get('/stats', async (req, res) => {
    try {
        const p = await getPool();
        const [[{ totalPlayers }]] = await p.execute(
            `SELECT COUNT(*) AS totalPlayers FROM scores`
        );
        const [[{ totalPlays }]] = await p.execute(
            `SELECT COUNT(*) AS totalPlays FROM score_logs`
        );
        const [[{ highScore }]] = await p.execute(
            `SELECT MAX(score) AS highScore FROM scores`
        );
        res.json({ totalPlayers, totalPlays, highScore: highScore || 0 });
    } catch (e) {
        console.error('查询统计失败:', e.message);
        res.status(500).json({ error: '服务器内部错误' });
    }
});

// ==================== 网页界面 ====================
const fs = require('fs');
const htmlCache = fs.readFileSync(__dirname + '/index.html', 'utf-8');
app.get('/', (req, res) => {
    res.type('html').send(htmlCache);
});

// ==================== 启动服务器 ====================
app.listen(PORT, '0.0.0.0', () => {
    console.log('═══════════════════════════════════════════');
    console.log('  贪吃蛇排行榜 - 本地 MySQL 版');
    console.log(`  服务器已启动: http://localhost:${PORT}`);
    console.log('  数据存储: 本地 MySQL (snake_leaderboard)');
    console.log('═══════════════════════════════════════════');
    console.log('');
    console.log('  局域网访问地址:');
    const os = require('os');
    const nets = os.networkInterfaces();
    for (const name of Object.keys(nets)) {
        for (const net of nets[name]) {
            if (net.family === 'IPv4' && !net.internal) {
                console.log(`    http://${net.address}:${PORT}`);
            }
        }
    }
    console.log('');
});
