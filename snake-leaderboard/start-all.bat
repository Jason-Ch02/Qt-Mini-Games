@echo off
chcp 65001 >nul
echo ========================================
echo   贪吃蛇排行榜 - 完整启动
echo   1. 本地 MySQL 服务器 (:3000)
echo   2. ngrok 公网隧道
echo ========================================
echo.
echo   公网地址: https://eclipse-bridged-credibly.ngrok-free.dev
echo.
echo   按 Ctrl+C 停止所有服务
echo ========================================
echo.

:: 启动本地服务器（新窗口）
start "Snake-Server" cmd /c "cd /d %~dp0 && node server-local.js"

:: 等待服务器启动
timeout /t 2 >nul

:: 启动 ngrok 隧道
D:\ngrok\ngrok.exe http --domain=eclipse-bridged-credibly.ngrok-free.dev 3000

pause
