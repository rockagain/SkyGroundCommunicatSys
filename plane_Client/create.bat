@echo off
echo open 192.168.0.101 > ftp.bat
echo user jztx 123 >> ftp.bat
echo put %1% >> ftp.bat
echo bye >> ftp.bat
echo exit >> ftp.bat