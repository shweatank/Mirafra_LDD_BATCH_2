cmd_/home/mirafra/Documents/DeviceDriver/modules.order := {   echo /home/mirafra/Documents/DeviceDriver/sample_driver.ko; :; } | awk '!x[$$0]++' - > /home/mirafra/Documents/DeviceDriver/modules.order
