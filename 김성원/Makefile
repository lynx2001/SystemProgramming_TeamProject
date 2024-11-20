CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lcurses
# 타겟 파일 이름
TARGET = calendar

# 소스 파일 및 오브젝트 파일 목록
SRCS = calendar.c calendar_control.c calendar_display.c calendar_get_event.c
OBJS = $(SRCS:.c=.o)

# 최종 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# 개별 오브젝트 파일 생성
calendar.o: calendar_control.h calendar_display.h calendar.h calendar.c
	$(CC) $(CFLAGS) -c calendar.c

calendar_control.o: calendar.h calendar_control.h calendar_display.h calendar_control.c
	$(CC) $(CFLAGS) -c calendar_control.c

calendar_display.o: calendar.h calendar_display.h calendar_get_event.h calendar_display.c
	$(CC) $(CFLAGS) -c calendar_display.c

calendar_get_event.o: calendar.h calendar_get_event.h calendar_get_event.c
	$(CC) $(CFLAGS) -c calendar_get_event.c

# 클린업
clean:
	rm -f $(OBJS) $(TARGET)
