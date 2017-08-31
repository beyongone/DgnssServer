

set DIR_BUILD=%~dp0
set DIR_PROJECT=%DIR_BUILD%..

mkdir %DIR_PROJECT%\vs
cd /D %DIR_PROJECT%\vs

del %DIR_PROJECT%\comn\lib\*.lib
del %DIR_PROJECT%\lib\*.lib
del %DIR_PROJECT%\Agnss.*


cmake %DIR_PROJECT% 
cmake --build . --config release
cpack
