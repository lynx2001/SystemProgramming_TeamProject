#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "event.h"

// 날짜 입력 받았을 때 유효성 검사
int validateDate(int year, int month, int day) {
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) {
        fprintf(stderr, "날짜 형식 또는 범위가 올바르지 않습니다. 다시 입력해 주세요.\n");
        return 0;
    }
    return 1;
}


// 사용자 입력 유효성 검사 (0 ~ 5 사이의 값이 아닐 때)
int validateImportance(int importance) {
    if (importance < 0 || importance > 5) {
        fprintf(stderr, "중요도는 0에서 5 사이의 값이어야 합니다. 다시 입력해 주세요.\n");
        return 0;
    }
    return 1;
}

// 윤년 계산
int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// 각 월의 일수 유효성
int daysInMonth(int month, int year) {
    switch (month) {
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return isLeapYear(year) ? 29 : 28;
    default:
        return 31;
    }
}


// Interval 유효성 계산 - Error: Start > End 
void intervalError(Time start, Time end) {
    if (start.year > end.year) {
        fprintf(stderr, "시작연도가 마감연도보다 늦습니다.\n");
        exit(1);
    }
    else if (start.year == end.year && start.day > end.day) {
        fprintf(stderr, "시작월이 마감월보다 늦습니다.\n");
        exit(1);
    }
    else if (start.year == end.year && start.month == end.month && start.day > end.day) {
        fprintf(stderr, "시작일이 마감일보다 늦습니다.\n");
        exit(1);
    }
    else {
        printf("Success interval\n");
    }
}