@ECHO OFF

ECHO Generating Bold...
..\fntgen\Release\fntgen generate C:\Windows\Fonts\tahomabd.ttf /o Bold.fdt /s 10 > nul
IF ERRORLEVEL 1 ECHO Error generating Bold. && PAUSE

ECHO Generating UI font...
..\fntgen\Release\fntgen generate C:\Windows\Fonts\tahomabd.ttf /o UI.fdt /s 10 > nul
IF ERRORLEVEL 1 ECHO Error generating UI font. && PAUSE

ECHO Generating Large Bold font...
..\fntgen\Release\fntgen generate C:\Windows\Fonts\tahomabd.ttf /o LargeBold.fdt /s 14 > nul
IF ERRORLEVEL 1 ECHO Error generating Large Bold font. && PAUSE