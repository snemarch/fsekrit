@echo off
set CL=/nologo /W3 /EHsc /Ox
set FILES=embed.cpp ..\sekrit.cpp ..\xtea.cpp ..\sha256.cpp ..\FileEndData.cpp ..\File.cpp
cl %FILES% /link /nologo /release /filealign:512 /out:embed.exe user32.lib gdi32.lib
if exist *.obj del *.obj
