

CC = gcc
CFLAGS  = -g -Wall
TARGET = myprogram
 
all: $(TARGET)
$(TARGET): $(TARGET).c
			$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
			$(RM) $(TARGET)

default: all