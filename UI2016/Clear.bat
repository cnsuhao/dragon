@echo off
echo 清理临时文件，用于代码转移

del  UI2016.sdf
rmdir Build /S /Q
rmdir ipch  /S /Q

del  Bin\*.dll /S /Q
del  Bin\*.exe /S /Q
del  Bin\*.ilk /S /Q
del  Bin\*.pak /S /Q
del  Bin\*.dat /S /Q
del  Bin\*.bin /S /Q
del  Bin\Debug\AC\html\ /S /Q
rmdir Bin\Debug\AC\html\ /S /Q
pause