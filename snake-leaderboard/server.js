const express = require('express');
const cors = require('cors');
const fs = require('fs');
const app = express();
const PORT = 3000;

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
// 网页界面在 index.html — 所有 HTML/CSS/JS 都在那个文件里
// ============================================================
const htmlCache = fs.readFileSync(__dirname + '/index.html', 'utf-8');
app.get('/', (req, res) => {
    res.type('html').send(htmlCache);
});

app.listen(PORT, '0.0.0.0', () => {
    console.log('排行榜服务器已启动: http://localhost:' + PORT);
});
