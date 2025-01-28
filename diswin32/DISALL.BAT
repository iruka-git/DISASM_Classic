echo off
if "%1" == "" goto usage
if not exist %1\krnl386.exe goto notexist

rem dis-list:解析すべきファイルのリストを作る
dir /b %1\*.exe  >dis-list
dir /b %1\*.dll >>dis-list
dir /b %1\*.drv >>dis-list

rem winapi16.lst :全てのWin16 APIマップを作る
echo **ALL-WIN16-API-LIST** >winapi16.lst
mkbat winapi16.scr dis-list >api-list.bat
call api-list %1

rem winapi32.lst:全てのWin32 APIマップを作る
echo **ALL-WIN32-API-LIST** >winapi32.lst
mkbat winapi32.scr dis-list >api-list.bat
call api-list %1

mkbat disall.scr  dis-list >dis-all.bat
echo ﾃﾞｨﾚｸﾄﾘ「%1」のexe dll を全て解析します。よろしいですか。
pause
mkdir dis
dis-all %1
goto end

:notexist
  echo ﾃﾞｨﾚｸﾄﾘ「%1」にはウィンドウズのシステムファイルが見あたりません

:usage
  echo "使い方："
  echo "  disall c:\windows\system "
  echo "         ~~~~~~~~~~~~~~~~~   ウィンドウズのシステムディレクトリを"
  echo "                             指定してください"
  goto end
:end
