@echo off
echo 清理临时文件，用于代码转移

del  ..\..\uileeihcy.ncb

del  ..\..\bin\Debug\UI\*.dll
del  ..\..\bin\Debug\UI\*.exe

rmdir ..\..\Build /S /Q
pause