@echo off

IF "%1"=="" GOTO :BLANK
IF "%2"=="" GOTO :BLANK

IF NOT EXIST C:\Windows\System32\inetsrv\appcmd.exe (
ECHO IIS service not installed. Run command to install: DISM /online /enable-feature /featureName:IIS-DefaultDocument /All
GOTO :BLANK
)

DISM /online /get-features | findstr /I /C:"IIS-WebSockets"
IF NOT %errorlevel% == 0 (
ECHO WebSockets not installed. Run command to install: DISM /online /enable-feature /featurename:IIS-WebSockets
GOTO :BLANK
)

copy /Y InteractWS.dll %windir%\System32\inetsrv
IF NOT %errorlevel% == 0 (
ECHO Copying InteractWS.dll to %windir%\System32\inetsrv failed. Check your folder and privilege.
GOTO :BLANK
)

SET currentPath=%cd%

pushd %windir%\System32\inetsrv

FOR /F delims^=^"^ tokens^=2 %%G IN ('appcmd list site /id:1') DO SET site=%%G
IF NOT %errorlevel% == 0 (
ECHO No site available. At least you need create a site as default one for IIS
GOTO :BLANK
)

FOR /F %%I in ('appcmd.exe list app "%site%/" /"text:[path='/'].physicalPath"') do set rootPathTemp=%%I
IF NOT %errorlevel% == 0 (
ECHO No physical path found for site. Check site configuration for proper physical path
GOTO :BLANK
)

FOR /F %%a in ('echo %rootPathTemp%') do set rootPath=%%a
IF NOT %errorlevel% == 0 (
ECHO No physical path %rootPath% found. Check your privilege accessing path %rootPathTemp%
GOTO :BLANK
)

IF exist %rootPath%\InteractWS (
rmdir /S /Q %rootPath%\InteractWS
)

xcopy /Y /I /Q /S %currentPath%\InteractWS %rootPath%\InteractWS\
IF NOT %errorlevel% == 0 (
ECHO Copying %currentPath%\InteractWS to %rootPath%\InteractWS\ failed. Check your folder and privilege.
GOTO :BLANK
)

appcmd add apppool /name:InteractWSPool
IF NOT %errorlevel% == 0 (
ECHO Add appPool InteractWSPool failed
GOTO :BLANK
)

appcmd set apppool "InteractWSPool" /autoStart:true /managedRuntimeVersion:"" /processModel.identityType:SpecificUser /processModel.userName:%1 /processModel.password:%2
IF NOT %errorlevel% == 0 (
ECHO Add account for InteractWSPool failed. Check user name and password (admin privilege required^)
GOTO :BLANK
)

appCmd set config -section:system.applicationHost/applicationPools /[name='InteractWSPool'].recycling.periodicRestart.time:"00:00:00"
IF NOT %errorlevel% == 0 (
ECHO Set InteractWSPool recycling time failed. Make sure InteractWSPool exists 
GOTO :BLANK
)

appcmd install module /add:false /name:InteractWSModule /image:%windir%\System32\inetsrv\InteractWS.dll
IF NOT %errorlevel% == 0 (
ECHO Register InteractWS module failed. Make sure %windir%\System32\inetsrv\InteractWS.dll exist
GOTO :BLANK
)

appcmd add app /site.name:"%site%" /path:/InteractWS /physicalPath:%rootPath%\InteractWS /applicationPool:InteractWSPool
IF NOT %errorlevel% == 0 (
ECHO Add InteractWS app failed. Make sure %currentPath%\InteractWS and InteractWSPool exist
GOTO :BLANK
)

appcmd unlock config /section:system.webServer/modules
IF NOT %errorlevel% == 0 (
ECHO Unlock config modules failed.
GOTO :BLANK
)

appcmd add module /name:InteractWSModule /app.name:"%site%/InteractWS"
IF NOT %errorlevel% == 0 (
ECHO Add InteractWS module failed. Make sure InteractWS module registered
GOTO :BLANK
)

appcmd unlock config /section:handlers
appcmd.exe set config "%site%/InteractWS" /section:handlers /+[name='InteractWSHandler',path='*.iws',modules='InteractWSModule',verb='*',resourceType='File',requireAccess='Read']
IF NOT %errorlevel% == 0 (
ECHO Add InteractWS handler failed. Make sure InteractWS module available and %site%/InteractWS exist
GOTO :BLANK
)

GOTO :DONE
:BLANK
ECHO usage: install.bat userAccount userPassword
ECHO 	userAccount - need have administrator privilege
:DONE
popd