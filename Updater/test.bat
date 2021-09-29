@echo off
echo open %1>> ftp.tmp
echo anonymous>> ftp.tmp
echo 123456>> ftp.tmp
echo get %2 %3>> ftp.tmp
echo bye>> ftp.tmp
ftp -i -s:ftp.tmp
del ftp.tmp
@echo on