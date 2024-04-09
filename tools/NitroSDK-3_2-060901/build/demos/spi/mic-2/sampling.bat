@ECHO OFF
IF EXIST *.wav  del *.wav /q
REM del log.txt
"%IS_NITRO_DIR%/X86/bin/ISNITROEMULATOR.exe" "bin/ARM9-TEG/Release/main.srl" /logfile log.txt
%NITROSDK_ROOT%/tools/bin/mic2wav log.txt
