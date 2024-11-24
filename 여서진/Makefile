CC = gcc
CFLAGS = -Wall -lncurses

TARGET = calendar_manager
OBJS = main.o calendar.o display.o event.o habit.o scheduler.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(TARGET) *.o

