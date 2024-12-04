CC = gcc
CFLAGS = -Wall -lncurses -p

TARGET = calendar_manager
OBJS = main.o calendar.o display.o event.o habit.o scheduler.o util.o date_check.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) -c $< $(CFLAGS)

clean:
	rm -f $(TARGET) *.o

