CC = gcc
CFLAGS = -Wall
LDFLAGS = -lbluetooth
TARGET = camera_control_bluetooth
SRC = camera_control_bluetooth.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
