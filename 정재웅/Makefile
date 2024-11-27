CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lcurses -lpthread
# 타겟 파일 이름
TARGET = Scheduling

# 소스 파일 및 오브젝트 파일 목록
SRCS = AutoScheduling.c CalculateScheduling.c SchedulingErrorCheck.c
OBJS = $(SRCS:.c=.o)

# 최종 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# 개별 오브젝트 파일 생성
AutoScheduling.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h event.h AutoScheduling.c
	$(CC) $(CFLAGS) -c AutoScheduling.c

CalculateScheduling.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h event.h CalculateScheduling.c
	$(CC) $(CFLAGS) -c CalculateScheduling.c

SchedulingErrorCheck.o: AutoScheduling.h CalculateScheduling.h SchedulingErrorCheck.h event.h SchedulingErrorCheck.c
	$(CC) $(CFLAGS) -c SchedulingErrorCheck.c

# 클린업
clean:
	rm -f $(OBJS) $(TARGET)