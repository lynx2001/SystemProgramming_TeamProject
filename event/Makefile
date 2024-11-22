# Makefile for Event Management System
# build		: make
# execute	: ./event_test
# clean		: make clean

# 컴파일러 설정
CC = gcc
CFLAGS = -Wall -g

# 파일 목록
SRC = main.c event.c util.c event_io.c global.c
OBJ = $(SRC:.c=.o)
TARGET = event_test

# 실행 파일 생성
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# 개별 파일 컴파일 규칙
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 파일 의존성 설정
main.o: global.h event.h util.h
event.o: global.h event.h util.h event_io.h
util.o: global.h util.h
event_io.o: event_io.h
global.o: global.h

# 클린 규칙
clean:
	rm -f $(OBJ) $(TARGET)
