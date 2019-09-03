for /F %%i in ('dir .\build-* /s /b') do (
	del /s /q %%i ) 
