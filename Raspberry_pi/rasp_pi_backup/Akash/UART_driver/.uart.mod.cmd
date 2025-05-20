cmd_/home/user1/Desktop/Akash/UART_driver/uart.mod := printf '%s\n'   uart.o | awk '!x[$$0]++ { print("/home/user1/Desktop/Akash/UART_driver/"$$0) }' > /home/user1/Desktop/Akash/UART_driver/uart.mod
