@echo off
:: Determine architecture and set curl path
IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    SET CURL_PATH=third_party\curl\x64\curl.exe
) ELSE (
    SET CURL_PATH=third_party\curl\x86\curl.exe
)

:: Define the URL for the feed archives
SET FEED_URL=https://example.com/user/feeds/today

:: Fetch the feed archives
%~dp0%..\%CURL_PATH% -o "%~dp0%..\archives\feed_archive.zip" %FEED_URL%

:: Notify the user
echo Feed archives downloaded successfully.
