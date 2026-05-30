-- ============================================
-- 贪吃蛇排行榜 - 本地 MySQL 数据库初始化
-- ============================================

CREATE DATABASE IF NOT EXISTS snake_leaderboard
    CHARACTER SET utf8mb4
    COLLATE utf8mb4_unicode_ci;

USE snake_leaderboard;

-- 排行榜表
CREATE TABLE IF NOT EXISTS scores (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(20)  NOT NULL,
    score       INT          NOT NULL,
    time        VARCHAR(30)  NOT NULL COMMENT '提交时间字符串',
    created_at  TIMESTAMP    DEFAULT CURRENT_TIMESTAMP,
    updated_at  TIMESTAMP    DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
    UNIQUE KEY uk_name (name)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

-- 分数提交日志表（保留每次提交记录，便于追溯）
CREATE TABLE IF NOT EXISTS score_logs (
    id          INT AUTO_INCREMENT PRIMARY KEY,
    name        VARCHAR(20)  NOT NULL,
    score       INT          NOT NULL,
    time        VARCHAR(30)  NOT NULL,
    created_at  TIMESTAMP    DEFAULT CURRENT_TIMESTAMP,
    INDEX idx_name (name),
    INDEX idx_created (created_at)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;
