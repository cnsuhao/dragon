;regsvr32 UILog.dll /s

; %~dp0% 表示将第一个参数即BAT所在路径扩展为其所在的目录
; 如果不这么做的话，从菜单中选择使用管理员权限执行bat文件时路径不对

regsvr32 "%~dp0%UILog.dll"