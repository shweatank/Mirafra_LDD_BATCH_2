sudo insmod procfs.ko
echo Hello > /proc/my_proc
cat /proc/my_proc
