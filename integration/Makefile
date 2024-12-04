CC = gcc
CFLAGS = -Wall -lncurses -p

TARGET = calendar_manager
CALENDAR = ./calendar/calendar.o ./calendar/calendar_control.o ./calendar/calendar_display.o ./calendar/calendar_get_event.o
OBJS = main.o display.o event.o habit.o scheduler.o util.o date_check.o ${CALENDAR}

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CFLAGS)

# integration 디렉토리의 .c 파일 규칙
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# calendar 디렉토리의 .c 파일 규칙
calendar/%.o: calendar/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

clean:
	rm -f $(TARGET) *.o calendar/*.o

