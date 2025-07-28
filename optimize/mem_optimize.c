step 1 : cat /proc/meminfo //check mem usage
//memtotal->total physical ram mem installed
//memfree->free mem availbale not at all used by sym
//buffer,catched->OS used mem i.e mem used by the kernel buffer
step 2 : cat /proc/meminfo > meminfo_before.txt
step 3 : grep -i mem /var/log/dmesg //how much RAM the kernel uses at boot.
step 4 : ls -lh /boot/vmlinuz-$(uname -r)//Check the size of your kernel image
step 5 : df -h / //df is disk free & h is human readable :  shows how much disk space is used and free on the root filesystem (the / directory).

step 6 : systemctl list-unit-files --type=service //disable the unnecessary services(like bluetooth and all);
step 7 : sudo systemctl disable bluetooth.service //disable the bluetooth
step 8 : lsmod         # shows loaded modules
step 9 :lsinitramfs /boot/initrd.img-$(uname -r) | less //display the contents of initramfs(early boot programs)
step 10 : sudo update-initramfs -c -k $(uname -r) //rebuild the curret initramfs with the current kernel config  and modules
step 11 : cat /proc/meminfo > meminfo_after.txt
diff meminfo_before.txt meminfo_after.txt

SUMMARY

when powererd on, the bios starts
,loads grub, the grub(defualt bootloader) load the kernel image and initramfs(compressed file that contains early boot programs) 
linux kernel boots  user space,
 now when the kernel starts it needs to mount to smthg so rootfs is the temp virtual file s/m created by linux kernel b4 booting b4 the actual disk based root file s/m becomes available now
 inside the rootfs we have the initramfs(which runs) and the driver needs acess to real storage like sd card 
 and ,once the driver is loaded ,the kernel can find the real root system and booting will continue
