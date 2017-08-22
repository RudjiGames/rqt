@echo off

:: Now we declare a scope
Setlocal EnableDelayedExpansion EnableExtensions

if not defined Configuration set Configuration=2015

if "%Configuration%"=="MinGW" ( goto :mingw )

set arch=x86

if "%platform%" EQU "x64" ( set arch=x86_amd64 )

if "%TOOLSET%"=="vs2015" (
	set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat"
)

if "%TOOLSET%"=="vs2013" (
	set SET_VS_ENV="C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat"
)

@echo "1"

:: Visual Studio detected
endlocal & call %SET_VS_ENV% %arch%

@echo "2"
goto :eof

:: MinGW detected
:mingw
endlocal & set PATH=c:\mingw\bin;%PATH%
