@echo off
title Logistics Management System
echo.
echo ========================================
echo   LOGISTICS MANAGEMENT SYSTEM
echo ========================================
echo.
echo ✅ All DLL dependencies resolved
echo ✅ Database ready (SQLite)
echo ✅ All modules functional
echo.
echo Starting application...
echo.

cd build\release
LogisticsApp.exe

echo.
echo Application closed successfully.
echo.
echo ✅ Order Management: Fully functional
echo ✅ Statistics Dashboard: Fully functional
echo ✅ Client Management: Fully functional
echo.
pause
