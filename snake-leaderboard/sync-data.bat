@echo off
chcp 65001 >nul
echo ========================================
echo   贪吃蛇排行榜 - 数据同步工具
echo   Railway → 本地 MySQL
echo ========================================
echo.

cd /d "%~dp0"

:: 检查 node_modules
if not exist "node_modules\" (
    echo [1/2] 正在安装依赖...
    call npm install
    echo.
)

echo [2/2] 开始同步数据...
echo.
node import-to-mysql.js
echo.
pause
