@echo off
echo ������ʱ�ļ������ڴ���ת��

del  ..\..\uileeihcy.ncb

del  ..\..\bin\Debug\UI\*.dll
del  ..\..\bin\Debug\UI\*.exe

rmdir ..\..\Build /S /Q
pause