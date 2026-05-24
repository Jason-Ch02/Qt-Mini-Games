# 🎮 Qt-Mini-Games

这是一个基于 **Qt 6** 开发的小游戏合集仓库，收录了多个轻量、独立的游戏项目，开箱即玩，无需复杂配置。

---

## 🕹️ 游戏列表

### 1. Taffy 贪吃蛇大作战
> 一个塔菲主题的 Qt 贪吃蛇游戏，支持本地游玩与排行榜同步。

- **源码目录**：`/snake`
- **游玩方式**：
  1. 下载 `/snake/Taffy贪吃蛇大作战.zip`
  2. 解压后双击 `snake--button.exe` 即可开始游戏
- **额外功能**：可连接 `snake-leaderboard` 服务器，记录你的最高分

### 2. Snake Leaderboard Server
> 为贪吃蛇游戏提供后端服务的排行榜服务器

- **源码目录**：`/snake-leaderboard`
- **功能说明**：负责存储、同步玩家的最高分数据，为游戏提供在线排行支持

---
### 3.游戏画面：
<img width="600" height="450" alt="a3e19eca4500c5f1d95b2be9a38fa310" src="https://github.com/user-attachments/assets/e37edf9c-bd37-43a1-a444-c4227a9583b0" />
<img width="630" height="452" alt="9f3234ad33e14c599dd7a0297751c429" src="https://github.com/user-attachments/assets/2b06345f-da01-4350-bd77-8f37c99c993b" />

界面主要采用Taffy风格，包括音乐与音效，还支持把你的游玩记录上传至排行榜，与所有玩家一教高下。

