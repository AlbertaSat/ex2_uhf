
cd \
cd C:\Documents and Settings\a0324020\My Documents\workspace_v5\SafetyMCU_Bootloader\

rem cmd

rem ==============================================
rem compile RM48 bootloaders: uart, spi, and can

cd RM48\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM48\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM48\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

rem ==============================================
rem compile RM46 bootloaders: uart, spi, and can

cd RM46\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM46\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM46\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

rem ==============================================
rem compile RM42 bootloaders: uart, spi, and can

cd RM42\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM42\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd RM42\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

rem ==============================================
rem compile TMS570LS31x bootloaders: uart, spi, and can

cd TMS570LS31x\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS31x\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS31x\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

rem ==============================================
rem compile TMS570LS12x bootloaders: uart, spi, and can

cd TMS570LS12x\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS12x\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS12x\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

rem ==============================================
rem compile TMS570LS04x bootloaders: uart, spi, and can

cd TMS570LS04x\boot_uart\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS04x\boot_spi\Release
call make_clean.bat
cd ..
cd ..
cd ..

cd TMS570LS04x\boot_can\Release
call make_clean.bat
cd ..
cd ..
cd ..

