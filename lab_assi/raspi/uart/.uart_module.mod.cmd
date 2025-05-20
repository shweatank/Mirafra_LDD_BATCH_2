cmd_/home/user1/Desktop/demo/uart/uart_module.mod := printf '%s\n'   uart_module.o | awk '!x[$$0]++ { print("/home/user1/Desktop/demo/uart/"$$0) }' > /home/user1/Desktop/demo/uart/uart_module.mod
