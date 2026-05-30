@echo off
chcp 65001 >nul
echo ========================================
echo   ngrok 隧道启动 - 固定域名
echo ========================================
echo.
echo   域名: eclipse-bridged-credibly.ngrok-free.dev
echo   映射: localhost:3000
echo.
echo   按 Ctrl+C 可以停止
echo ========================================
echo.

D:\ngrok\ngrok.exe http --domain=eclipse-bridged-credibly.ngrok-free.dev 3000

pause
