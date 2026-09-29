@echo off
REM Script to generate Doxygen documentation for Poko project

echo Generating Poko documentation...
cd /d "%~dp0.."
doxygen Doxyfile

if %ERRORLEVEL% EQU 0 (
    echo Documentation generated successfully in docs/generated/html/
    echo Open docs/generated/html/index.html in your browser to view the documentation
) else (
    echo Documentation generation failed
    exit /b 1
)