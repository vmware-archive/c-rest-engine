
@echo on
set LOGDIR=%CD%
set LOG=%CD%\build.log
set DEBUGLOG=%CD%\debug_build.log
set RELEASELOG=%CD%\release_build.log

set WINSDK_PATH=%TCROOT%\win32\winsdk-7.0.7600
set VC_PATH=%WINSDK_PATH%\VC
set FRAMEWORK_PATH=%windir%\Microsoft.NET\Framework\v3.5
set BASE_PATH=%VC_PATH%\bin\amd64;%VC_PATH%\vcpackages;%WINSDK_PATH%\bin
set BASE_PATH=%BASE_PATH%;%TCROOT%\win32\bin;%WIX_PATH%
set BASE_PATH=%BASE_PATH%;%SystemRoot%\system32;%SystemRoot%
set BASE_PATH=%BASE_PATH%;%SystemRoot%\SysWOW64\wbem;%SystemRoot%\system32\wbem
set CPU=AMD64
set INCLUDE=%VC_PATH%\atlmfc\Include;%VC_PATH%\Include;%WINSDK_PATH%\Include;
set LIB=%VC_PATH%\atlmfc\Lib\amd64;%VC_PATH%\lib\amd64;%WINSDK_PATH%\Lib\x64;
set LIBPATH=%FRAMEWORK_PATH%;%VC_PATH%\atlmfc\Lib\amd64;%VC_PATH%\lib\amd64;%WINSDK_PATH%\Lib\x64;
set MS_BUILD="%windir%\Microsoft.NET\Framework\v3.5\MSBuild.exe"

set PATH=%TCROOT%\win32\make-3.81;%TCROOT%\win32\cygwin-1.5.19-4\bin;%BASE_PATH%;%WINSDK_PATH%\dll\amd64\Microsoft.VC90.DebugCRT;%WINSDK_PATH%\bin

(echo starting trident build )>%LOG%

set BUILD_VS2008="%TCROOT%\win32\winsdk-7.0.7600\VC\vcpackages\vcbuild.exe"
(echo BUILD_VS2008=%BUILD_VS2008%)>>%LOG%

pushd .

(echo  )>>%LOG%
(echo =======================================================================================)>>%LOG%
(echo Building trident )>%LOG%

echo Build Debug x64
%BUILD_VS2008% td4.sln /logfile:%DEBUGLOG% /Rebuild "Debug|x64" /useenv /showenv /logcommands /Time
if errorlevel 1 (
  (echo VS 2008 td4.sln Debug x64 FAILED)>>%DEBUGLOG%
  echo VS 2008 td4.sln Debug x64 FAILED
  goto error
)

(echo DONE building trident)>%LOG%

(echo  )>>%LOG%
(echo =======================================================================================)>>%LOG%

(echo  )>>%LOG%
(echo =======================================================================================)>>%LOG%
(echo Building trident )>%LOG%

echo Build Release x64
%BUILD_VS2008% td4.sln /logfile:%RELEASELOG% /Rebuild "Release|x64" /useenv /showenv /logcommands /Time
if errorlevel 1 (
  (echo VS 2008 td4.sln Release x64 FAILED)>>%RELEASELOG%
  echo VS 2008 td4.sln Release x64 FAILED
  goto error
)

(echo DONE building trident)>%LOG%

(echo  )>>%LOG%
(echo =======================================================================================)>>%LOG%


goto end

:error

(echo !!!!!!!!!!Build FAILED!!!!!!!!!! [Log: "%LOG%"])>>%LOG%
echo !!!!!!!!!!Build FAILED!!!!!!!!!! [Log: "%LOG%"]
if not %BUILDLOG_DIR% == "" (
    copy %LOGDIR%\*.log %BUILDLOG_DIR:/=\%
)
popd
exit /B 1

:end

(echo !!!!!!!!!!Build succeeded!!!!!!!!!!)>>%LOG%
echo !!!!!!!!!!Build succeeded!!!!!!!!!!
if not "%BUILDLOG_DIR%" == "" (
    copy %LOGDIR%\*.log %BUILDLOG_DIR:/=\%
)

popd
exit /B 0
