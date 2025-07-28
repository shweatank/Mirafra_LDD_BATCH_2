# Step 1: Check current boot time
systemd-analyze
# Step 2: Reduce GRUB timeout
sudo nano /etc/default/grub
# Set: GRUB_TIMEOUT=0 //GRUB waits (in seconds) before booting the default OS.so we have reduced the delay
#step 3:sudo update-grub //Apply the changes


step 1:sudo nano /etc/default/grub
step 2 :  check for line GRUB_CMDLINE_LINUX_DEFAULT="quiet splash"
//quiet is to reduce kernel log msg during booting splash is the screen
step 3 : modify the line GRUB_CMDLINE_LINUX_DEFAULT="quiet loglevel=3"
//quiet: hides unnecessary logs
//loglevel=3: shows only warnings and errors
step 4 : sudo update-grub //to apply the changes

sudo reboot
step 1 :  systemd-analyze//we can see the boot time reduced
