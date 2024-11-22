// event_io.c
#include "event_io.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 숫자 입력 유효성 검사 함수
int getIntInput(const char *prompt) {
    char buffer[50];
    int value;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            // 문자열 끝에서 개행 제거
            buffer[strcspn(buffer, "\n")] = '\0';

            // 문자열을 정수로 변환
            if (sscanf(buffer, "%d", &value) == 1) {
                return value;
            }
        }
        printf("잘못된 입력입니다. 정수를 입력하세요.\n");
    }
}

// 실수 입력 유효성 검사 함수
double getDoubleInput(const char *prompt) {
    char buffer[50];
    double value;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin)) {
            // 문자열 끝에서 개행 제거
            buffer[strcspn(buffer, "\n")] = '\0';

            // 문자열을 정수로 변환
            if (sscanf(buffer, "%lf", &value) == 1) {
                return value;
            }
        }
        printf("잘못된 입력입니다. 실수를 입력하세요.\n");
    }
}


// 문자열 입력 함수 (공백 허용 포함)
void getStringInput(const char *prompt, char *input, int size, int allowEmpty) {
    while (1) {
        printf("%s", prompt);
        if (fgets(input, size, stdin) != NULL) {
            size_t len = strlen(input);
            if (len > 0 && input[len - 1] == '\n') {
                input[len - 1] = '\0'; // 개행 문자 제거
            }

            // 공백 허용 여부에 따른 처리
            if (allowEmpty || strlen(input) > 0) {
                return; // 공백 허용이거나 입력이 비어있지 않을 경우 종료
            }
        }
        printf("입력은 비워둘 수 없습니다. 다시 입력하세요.\n");
    }
}

// 윤년 확인 함수
int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

bool validateDate(int year, int month, int day) {
    // 기본 범위 검사
    if (year < 1900 || year > 2100 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31) {
        return false;
    }

    // 월별 일수 검사
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // 윤년 처리
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    return day <= daysInMonth[month];
}

bool validateTime(int hour, int minute) {
    // 24:00은 특별한 경우로 허용
    if (hour == 24) {
        return minute == 0;
    }
    
    // 일반적인 시간 범위 검사
    return hour >= 0 && hour < 24 &&
           minute >= 0 && minute < 60;
}