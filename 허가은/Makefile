CC = gcc
CFLAGS = -Wall -g 
LDFLAGS = -lcurses
# 타겟 파일 이름
TARGET = Habit

# 소스 파일 및 오브젝트 파일 목록
SRCS = habit_function.c habit_main.c date_check.c
OBJS = $(SRCS:.c=.o)

# 최종 실행 파일 생성
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $(TARGET)

# 개별 오브젝트 파일 생성
habit_main.o: habit_function.h habit_main.c
	$(CC) $(CFLAGS) -c habit_main.c

habit_function.o: habit_function.h habit_function.c
	$(CC) $(CFLAGS) -c habit_function.c

date_check.o: date_check.h date_check.c
	$(CC) $(CFLAGS) -c date_check.c

# 클린업
clean:
	rm -f $(OBJS) $(TARGET)