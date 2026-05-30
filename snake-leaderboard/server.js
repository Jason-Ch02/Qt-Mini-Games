const express = require('express');
const cors = require('cors');
const fs = require('fs');
const app = express();
const PORT = 3000;

app.use(cors());
app.use(express.json());

// ---- 实时排行榜（内存，服务器重启后清空） ----
let scores = [];

// ---- 历史总排行榜（持久化到 alltime.json） ----
const ALLTIME_FILE = __dirname + '/alltime.json';
let alltimeScores = [];

function loadAlltime() {
    try {
        if (fs.existsSync(ALLTIME_FILE)) {
            const raw = fs.readFileSync(ALLTIME_FILE, 'utf-8');
            alltimeScores = JSON.parse(raw);
        }
    } catch (e) {
        console.error('读取历史记录失败:', e.message);
        alltimeScores = [];
    }
}

function saveAlltime() {
    try {
        fs.writeFileSync(ALLTIME_FILE, JSON.stringify(alltimeScores));
    } catch (e) {
        console.error('保存历史记录失败:', e.message);
    }
}

loadAlltime();

// ---- 提交分数 ----
app.post('/submit', (req, res) => {
    const { name, score } = req.body;
    if (!name || typeof score !== 'number' || score <= 0) {
        return res.status(400).json({ error: '数据不合法' });
    }
    const cleanName = name.trim().slice(0, 20);
    const now = new Date().toLocaleString('zh-CN');

    // 1) 更新实时排行榜
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

    // 2) 更新历史总排行榜（持久化）
    const allIdx = alltimeScores.findIndex(e => e.name === cleanName);
    if (allIdx >= 0) {
        if (score > alltimeScores[allIdx].score) {
            alltimeScores[allIdx].score = score;
            alltimeScores[allIdx].time = now;
        }
    } else {
        alltimeScores.push({ name: cleanName, score, time: now });
    }
    alltimeScores.sort((a, b) => b.score - a.score);
    saveAlltime();

    const rank = scores.findIndex(e => e.name === cleanName) + 1;
    res.json({ status: 'ok', rank });
});

// ---- 实时排行榜 API ----
app.get('/leaderboard', (req, res) => {
    res.json(scores.slice(0, 10));
});

// ---- 历史总排行榜 API ----
app.get('/alltime', (req, res) => {
    res.json(alltimeScores.slice(0, 10));
});

// ---- 全量数据导出 API（用于数据迁移） ----
app.get('/export', (req, res) => {
    res.json(alltimeScores);
});

// ---- 网页界面 ----
const htmlCache = fs.readFileSync(__dirname + '/index.html', 'utf-8');
app.get('/', (req, res) => {
    res.type('html').send(htmlCache);
});

app.listen(PORT, '0.0.0.0', () => {
    console.log('排行榜服务器已启动: http://localhost:' + PORT);
});
