const express = require('express');
const cors = require('cors');
const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());

let scores = [];

app.post('/submit', (req, res) => {
    const { name, score } = req.body;
    if (!name || typeof score !== 'number' || score <= 0) {
        return res.status(400).json({ error: '数据不合法' });
    }
    const cleanName = name.trim().slice(0, 20);
    const now = new Date().toLocaleString('zh-CN');

    // Update existing player's score if higher, otherwise add new entry
    const existingIdx = scores.findIndex(e => e.name === cleanName);
    if (existingIdx >= 0) {
        if (score > scores[existingIdx].score) {
            scores[existingIdx].score = score;
            scores[existingIdx].time = now;
        }
    } else {
        scores.push({ name: cleanName, score, time: now });
    }

    scores.sort((a, b) => b.score - a.score);
    if (scores.length > 100) scores.length = 100;

    const rank = scores.findIndex(e => e.name === cleanName) + 1;
    res.json({ status: 'ok', rank });
});

app.get('/leaderboard', (req, res) => {
    const top10 = scores.slice(0, 10);
    res.json(top10);
});

// ============================================================
// 网页界面 — 下面就是你要修改的 HTML/CSS
// ============================================================
app.get('/', (req, res) => {
    res.send(`<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>贪吃蛇排行榜</title>
    <style>
        /* ===== 页面整体样式 ===== */
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            background: #1a1a2e;
            color: #eee;
            font-family: "Microsoft YaHei", "PingFang SC", sans-serif;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }

        /* ===== 主容器 ===== */
        .container {
            width: 100%;
            max-width: 600px;
            padding: 20px;
        }

        /* ===== 标题 ===== */
        h1 {
            text-align: center;
            font-size: 28px;
            margin-bottom: 24px;
            color: #e94560;
            letter-spacing: 2px;
        }

        /* ===== 排行榜表格 ===== */
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            padding: 12px 16px;
            text-align: center;
        }
        th {
            background: #16213e;
            color: #aaa;
            font-weight: normal;
            font-size: 14px;
            border-bottom: 2px solid #e94560;
        }
        td {
            border-bottom: 1px solid #333;
        }

        /* ===== 前三名高亮 ===== */
        tr:nth-child(2) td:first-child { color: #ffd700; font-weight: bold; } /* 金牌 */
        tr:nth-child(3) td:first-child { color: #c0c0c0; font-weight: bold; } /* 银牌 */
        tr:nth-child(4) td:first-child { color: #cd7f32; font-weight: bold; } /* 铜牌 */

        /* ===== 空状态 ===== */
        .empty {
            text-align: center;
            padding: 60px 0;
            color: #666;
            font-size: 18px;
        }

        /* ===== 底部信息 ===== */
        .footer {
            text-align: center;
            margin-top: 20px;
            color: #555;
            font-size: 13px;
        }
        .footer span {
            color: #e94560;
            cursor: pointer;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🐍 贪吃蛇排行榜</h1>
        <div id="app">
            <div class="empty">加载中...</div>
        </div>
        <div class="footer">
            每 <span onclick="location.reload()">30 秒</span> 自动刷新 | 点击刷新
        </div>
    </div>

    <script>
        async function loadLeaderboard() {
            const app = document.getElementById('app');
            try {
                const resp = await fetch('/leaderboard');
                const data = await resp.json();

                if (data.length === 0) {
                    app.innerHTML = '<div class="empty">暂无记录，快来玩一局吧！</div>';
                    return;
                }

                let html = '<table>';
                html += '<tr><th>排名</th><th>玩家</th><th>分数</th><th>时间</th></tr>';
                data.forEach((entry, i) => {
                    html += \`<tr>
                        <td>\${i + 1}</td>
                        <td>\${escapeHtml(entry.name)}</td>
                        <td>\${entry.score}</td>
                        <td>\${entry.time}</td>
                    </tr>\`;
                });
                html += '</table>';
                app.innerHTML = html;
            } catch (e) {
                app.innerHTML = '<div class="empty">加载失败，请刷新重试</div>';
            }
        }

        function escapeHtml(text) {
            const div = document.createElement('div');
            div.textContent = text;
            return div.innerHTML;
        }

        loadLeaderboard();
        setInterval(loadLeaderboard, 30000);
    </script>
</body>
</html>`);
});

app.listen(PORT, '0.0.0.0', () => {
    console.log('排行榜服务器已启动: http://localhost:' + PORT);
});
