echo off
set ACLIDIR = %HOMEDRIVE%%HOMEPATH%\go\bin
if not exist %ACLIDIR%\arduino-cli.exe (
 go version
 IF %ERRORLEVEL% NEQ 0 (
  Echo Go is not installed. Please installed Go-Lang first
  exit
 ) else (
  go get -u github.com/arduino/arduino-cli
 )
)
echo Checking for \.cli-config.yml
if not exist %ACLIDIR%\.cli-config.yml (
 copy .cli-config.yml %ACLIDIR%\.cli-config.yml
) else (
 echo  .cli-config.yml already installed.
)
%ACLIDIR%\arduino-cli core update-index
%ACLIDIR%\arduino-cli core install esp8266:esp8266
%ACLIDIR%\arduino-cli core install esp32:esp32
