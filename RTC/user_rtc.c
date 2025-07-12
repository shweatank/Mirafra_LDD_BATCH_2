user space code-->

 1.Hardware Setup (Wiring)
 2.Enable I2C and Load DS3231 Driver
 dtoverlay=i2c-rtc,ds3231
sudo reboot
3.Remove Fake Hardware Clock (One-Time Step)sudo apt remove fake-hwclock
sudo systemctl disable fake-hwclock
sudo rm /etc/adjtime
4.i2cdetect -y 1   # You should see 0x68 in the table
reboot
dmesg | grep rtc
ls /dev/rtc*

5.C User-Space Program to Read RTC

6.all:
	gcc -o read_rtc read_rtc.c
clean:
	rm -f read_rtc

./read_rtc

optional command
sudo hwclock -s
sudo hwclock -w
sudo hwclock -r




