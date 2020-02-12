::========================= 说明  =========================
:: 	制作：Purp1e		bilibili@Purple-CSGO
::	参考：@狙击手Ricky 和 @B5官方 提供的批处理脚本
:: 	使用：调用控制台指令 "..../SolveVAC.bat" "路径"
::	注意：1. 系统盘必须为C盘  2. 路径带/不含xxx.exe
::		3.兼容国服		4. 可处理带空格路径
::=====================  获取管理员权限  ======================
 @echo off
 CLS
 SETLOCAL ENABLEDELAYEDEXPANSION
 title VAC验证修复 - Purp1e
 
:init
 setlocal DisableDelayedExpansion
 set cmdInvoke=1
 set winSysFolder=System32
 set "batchPath=%~0"
 for %%k in (%0) do set batchName=%%~nk
 set "vbsGetPrivileges=%temp%\OEgetPriv_%batchName%.vbs"
 setlocal EnableDelayedExpansion

:checkPrivileges
  NET FILE 1>NUL 2>NUL
  if '%errorlevel%' == '0' ( goto gotPrivileges ) else ( goto getPrivileges )

:getPrivileges
  if '%1'=='ELEV' (echo ELEV & shift /1 & goto gotPrivileges)

  ECHO Set UAC = CreateObject^("Shell.Application"^) > "%vbsGetPrivileges%"
  ECHO args = "ELEV " >> "%vbsGetPrivileges%"
  ECHO For Each strArg in WScript.Arguments >> "%vbsGetPrivileges%"
  ECHO args = args ^& strArg ^& " "  >> "%vbsGetPrivileges%"
  ECHO Next >> "%vbsGetPrivileges%"

  if '%cmdInvoke%'=='1' goto InvokeCmd 

  ECHO UAC.ShellExecute "!batchPath!", args, "", "runas", 1 >> "%vbsGetPrivileges%"
  goto ExecElevation

:InvokeCmd
  ECHO args = "/c """ + "!batchPath!" + """ " + args >> "%vbsGetPrivileges%"
  ECHO UAC.ShellExecute "%SystemRoot%\%winSysFolder%\cmd.exe", args, "", "runas", 1 >> "%vbsGetPrivileges%"

:ExecElevation
 "%SystemRoot%\%winSysFolder%\WScript.exe" "%vbsGetPrivileges%" %*
 exit /B

:gotPrivileges
 setlocal & cd /d %~dp0
 if '%1'=='ELEV' (del "%vbsGetPrivileges%" 1>nul 2>nul  &  shift /1)
 
::=================== 读取传过来的路径参数  ====================
mode con cols=80 lines=30
taskkill /F /IM csgo.exe
taskkill /F /IM Steam.exe
taskkill /F /IM csgolauncher.exe
 CLS

set path=%1 %2 %3 %4 %5 %6 %7 %8 %9

:delright
if "%path:~-1%"==" " set path=%path:~0,-1%&&goto delright
echo 获得路径：%path%

if "%path%" == "" (
title 错误
echo 路径为空！
goto end
)
::else (goto debug)


::======================= 解决VAC屏蔽  ========================
:start
echo 开始解决VAC屏蔽

echo 开启 Network Connections
c:\windows\system32\ac config Netman start= AUTO
c:\windows\system32\sc start Netman

echo 开启 Remote Access Connection Manager
c:\windows\system32\sc config RasMan start= AUTO
c:\windows\system32\sc start RasMan

echo 开启 Telephony
c:\windows\system32\sc config TapiSrv start= AUTO
c:\windows\system32\sc start TapiSrv

echo 开启 Windows Firewall
c:\windows\system32\sc config MpsSvc start= AUTO
c:\windows\system32\sc start MpsSvc
c:\windows\system32\netsh advfirewall set allprofiles state on

echo 恢复 Data Execution Prevention 启动设置为默认值
c:\windows\system32\bcdedit /deletevalue nointegritychecks
c:\windows\system32\bcdedit /deletevalue loadoptions
c:\windows\system32\bcdedit /debug off
c:\windows\system32\bcdedit /deletevalue nx

:: ==================== 重装Steam Services ====================
CLS
echo ═══════════════════════════════════════════════════════
echo ^>^>^>^>^>^>^>^>^>^>       开始重装Steam Services       ^<^<^<^<^<^<^<^<^<^<
echo ═══════════════════════════════════════════════════════

cd /d "%path%/bin"
steamservice  /uninstall
c:\windows\system32\ping -n 3 127.0.0.1>nul
steamservice  /install
echo .

echo	═══════════════════════════════════════════════════════
echo ^>^>^>^>^>^>^>^>^>^>       重装Steam Services完毕       ^<^<^<^<^<^<^<^<^<^<  
echo	═══════════════════════════════════════════════════════
echo # 出现"Steam client service install completed.."
echo # 且无任何"Fail"字样才可以结束
echo # 否则请检查您的防火墙设置(关闭“不允许例外”选项)

start steam -cafeapplaunch -forceservice

echo	═══════════════════════════════════════════════════════
echo ^>^>^>^>^>^>^>^>^>^>       启动Steam Services服务       ^<^<^<^<^<^<^<^<^<^<  
echo	═══════════════════════════════════════════════════════
c:\windows\system32\sc config "Steam Client Service" start= AUTO
c:\windows\system32\sc start "Steam Client Service"

:debug
title VAC验证修复 - Purp1e
echo	═══════════════════════════════════════════════════════
echo ^>^>^>^>^>^>^>^>^>^>      完毕！按任意键结束窗口！      ^<^<^<^<^<^<^<^<^<^<  
echo	═══════════════════════════════════════════════════════

:end
pause>nul
exit
