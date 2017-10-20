set dst=c:\data\scan%1

if not exist %dst% md %dst%

set /A last=%1+180

@echo %last%

C:\Users\kaestner\repos\tomography\trunk\src\build-muhrec3-Qt5-Release\release\muhrec3.exe -f c:\data\CurrentRecon.xml matrix:path=%dst% projections:firstindex=%1 projections:lastindex=%last%