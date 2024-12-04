CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lcurses -lpthread
# 타겟 파일 이름
TARGET = Scheduling

# 소스 파일 및 오브젝트 파일 목록
SRCS = AutoScheduling.c CalculateScheduling.c SchedulingErrorCheck.c date_check.c util.c
OBJS = $(SRCS:.c=.o)

# 최종 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# 개별 오브젝트 파일 생성
AutoScheduling.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h date_check.h global.h util.h AutoScheduling.c
	$(CC) $(CFLAGS) -c AutoScheduling.c

CalculateScheduling.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h date_check.h global.h util.h CalculateScheduling.c
	$(CC) $(CFLAGS) -c CalculateScheduling.c

SchedulingErrorCheck.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h date_check.h global.h util.h SchedulingErrorCheck.c
	$(CC) $(CFLAGS) -c SchedulingErrorCheck.c

date_check.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h date_check.h global.h util.h date_check.c
	$(CC) $(CFLAGS) -c date_check.c

util.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h date_check.h global.h util.h util.c
	$(CC) $(CFLAGS) -c util.c


# 클린업
clean:
	rm -f $(OBJS) $(TARGET)