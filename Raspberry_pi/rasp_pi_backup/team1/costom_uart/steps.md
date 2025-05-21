
# --- Prerequisites ---
# 1. Ensure source files are saved:
#   Pi: rpi_uart_driver.c,Makefile,
#       rpi-uart-overlay.dts, test.c
#  
# 2. Confirm Raspberry Pi model (e.g., 4B, 3B) for UART_BASE:
#    - Pi 4: 0xFE201000 (default)
#    - Pi 3: 0x3F201000
# 3. Install required tools on Pi:
sudo apt update
sudo apt install raspberrypi-kernel-headers build-essential gcc

# --- Step 1: Verify Raspberry Pi Model ---
# On Pi, check UART base address:
cat /proc/iomem | grep serial
# If Pi 3 (BCM2837), edit ~/Desktop/uart/rpi_uart_driver.c, line 14:
# #define UART_BASE 0x3F201000
# Then re-save the file.

# --- Step 2: Configure Raspberry Pi ---
# Edit boot configuration:
sudo nano /boot/firmware/config.txt
# Add or verify:
# enable_uart=1
# dtoverlay=disable-bt
# dtoverlay=rpi-uart
# Save and exit (Ctrl+O, Enter, Ctrl+X).

# Disable serial console:
sudo systemctl stop serial-getty@ttyAMA0.service
sudo systemctl disable serial-getty@ttyAMA0.service

# optional
# Edit cmdline:
sudo nano /boot/firmware/cmdline.txt
# Remove 'console=serial0,115200' or 'console=ttyAMA0,115200'.
# Save and exit.

# Reboot Pi:
sudo reboot
# Log back in as user1.

# --- Step 3: Compile and Install Device Tree Overlay ---
# On Pi:
cd ~/Desktop/uart
dtc -@ -I dts -O dtb -o rpi-uart.dtbo rpi-uart-overlay.dts
sudo cp rpi-uart.dtbo /boot/overlays/
# Verify dtoverlay=rpi-uart in /boot/firmware/config.txt.

# --- Step 4: Compile and Load Kernel Module ---
# On Pi:
cd ~/Desktop/uart
make clean
make
sudo insmod rpi_uart_driver.ko
# Verify device:
ls /dev/rpi_uart
# Should show: /dev/rpi_uart
# Check logs:
dmesg | grep rpi_uart
# Expect: [timestamp] rpi_uart: RPi UART driver loaded

# Set permissions:
sudo chmod 660 /dev/rpi_uart
sudo chown root:dialout /dev/rpi_uart
# Verify:
ls -l /dev/rpi_uart
# Should show: crw-rw---- 1 root dialout ...


# For testing
# Test code(short tx,rx)
sudo ./test
# Expect:
# Serial port opened. Sending initial message...
# Sent: Hello from Raspi
# Received: Hello from Raspi
