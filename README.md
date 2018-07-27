# ex2_uhf
Ex-Alta 2 UHF module firmware

The 'prototype' branch contains code for a prototype built around a Gecko PG12 board and an ADF7021 eval board.
 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!!!!  IF YOU ARE GETTING BUILD ERRORS  !!!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
If you get build errors like "undefined symbol `__Vectors' referenced in expression" with a fresh git clone,
it may be due to missing build directories and a buggy Simplicity Studios overwriting the project file with a bad one.
Fix:
1. Close the project
2. git checkout .project
3. Open and build the project again.

