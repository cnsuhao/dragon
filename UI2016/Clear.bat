@echo off
echo 清理临时文件，用于代码转移

del  UI2016.sdf
rmdir Build /S /Q
rmdir ipch  /S /Q

del  Bin\Debug\UI\*.dll
del  Bin\Debug\UI\*.exe
del  Bin\Debug\UI\*.ilk

pause