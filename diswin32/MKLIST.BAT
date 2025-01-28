echo off
if "%1" == "" goto usage
if not exist %1\krnl386.exe goto notexist

diswin -x %1\krnl386.exe   >diswin.api
diswin -x %1\user.exe     >>diswin.api
diswin -x %1\gdi.exe      >>diswin.api
diswin -x %1\keyboard.drv >>diswin.api
diswin -x %1\mouse.drv    >>diswin.api
diswin -x %1\system.drv   >>diswin.api
diswin -x %1\comm.drv     >>diswin.api
diswin -x %1\toolhelp.dll >>diswin.api
diswin -x %1\commdlg.dll  >>diswin.api
diswin -x %1\ddeml.dll    >>diswin.api
diswin -x %1\mmsystem.dll >>diswin.api
diswin -x %1\shell.dll    >>diswin.api
diswin -x %1\olecli.dll   >>diswin.api
diswin -x %1\unidrv.dll   >>diswin.api
diswin -x %1\win87em.dll  >>diswin.api
diswin -x %1\windebug.dll >>diswin.api
diswin -x %1\lzexpand.dll >>diswin.api
diswin -x %1\v7vga.drv    >>diswin.api
diswin -x %1\olesvr.dll   >>diswin.api
diswin -x %1\sound.drv    >>diswin.api
  goto end


:notexist
  echo ﾃﾞｨﾚｸﾄﾘ「%1」にはウィンドウズのシステムファイルが見あたりません

:usage
  echo "使い方："
  echo "  mklist c:\windows\system "
  echo "         ~~~~~~~~~~~~~~~~~   ウィンドウズのシステムディレクトリを"
  echo "                             指定してください"
  goto end
:end
