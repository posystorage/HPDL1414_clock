@REM This batch file has been generated by the IAR Embedded Workbench
@REM C-SPY Debugger, as an aid to preparing a command line for running
@REM the cspybat command line utility using the appropriate settings.
@REM
@REM You can launch cspybat by typing the name of this batch file followed
@REM by the name of the debug file (usually an ELF/DWARF or UBROF file).
@REM Note that this file is generated every time a new debug session
@REM is initialized, so you may want to move or rename the file before
@REM making changes.
@REM 


"H:\st\IAR\common\bin\cspybat" "H:\st\IAR\stm8\bin\stm8proc.dll" "H:\st\IAR\stm8\bin\stm8stlink.dll"  %1 --plugin "H:\st\IAR\stm8\bin\stm8bat.dll" --backend -B "-p" "H:\st\IAR\stm8\config\ddf\iostm8s103k3.ddf" "-d" "stlink" "--mcuname" "STM8S103K3U" 

