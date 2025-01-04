@echo off

pushd %windir%\System32\inetsrv

appcmd delete app "Default Web Site/InteractWS"
appcmd uninstall module InteractWSModule
appcmd delete apppool InteractWSPool

FOR /F delims^=^"^ tokens^=2 %%G IN ('appcmd list site /id:1') DO SET site=%%G
IF NOT %errorlevel% == 0 (
ECHO No site available. At least you need create a site as default one for IIS
)

FOR /F %%I in ('appcmd.exe list app "%site%/" /"text:[path='/'].physicalPath"') do set rootPathTemp=%%I
IF NOT %errorlevel% == 0 (
ECHO No physical path found for site. Check site configuration for proper physical path
)

FOR /F %%a in ('echo %rootPathTemp%') do set rootPath=%%a
IF NOT %errorlevel% == 0 (
ECHO No physical path %rootPath% found. Check your privilege accessing path %rootPathTemp%
)

IF exist %rootPath%\InteractWS (
rmdir /S /Q %rootPath%\InteractWS
)

IF exist InteractWS.dll (
del /Q InteractWS.dll
)
 
popd