cmd_/home/user1/Desktop/uart/rpi_uart_driver.mod := printf '%s\n'   rpi_uart_driver.o | awk '!x[$$0]++ { print("/home/user1/Desktop/uart/"$$0) }' > /home/user1/Desktop/uart/rpi_uart_driver.mod
