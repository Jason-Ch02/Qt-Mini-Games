@echo off
chcp 65001 >nul
echo ========================================
echo   贪吃蛇排行榜 - 本地服务器启动
echo ========================================
echo.

cd /d "%~dp0"

:: 检查 node_modules
if not exist "node_modules\" (
    echo [1/2] 正在安装依赖...
    call npm install
    echo.
)

:: 检查 .env 文件
if not exist ".env" (
    echo [警告] 未找到 .env 文件
    echo 请先复制 .env.example 为 .env 并填写你的 MySQL 密码
    echo.
    pause
    exit /b 1
)

echo [2/2] 启动本地 MySQL 版服务器...
echo.
call npm run start-local
pause
