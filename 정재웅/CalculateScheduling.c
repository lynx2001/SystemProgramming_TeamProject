#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "CalculateScheduling.h"

// 현재 시각 계산
void initTime(Time* x) {
    timer = time(NULL);
    t = localtime(&timer);

    x->year = t->tm_year + 1900;
    x->month = t->tm_mon + 1;
    x->day = t->tm_mday;
}

// Interval 계산: 마감일 - 시작일
void calInterval(Event* event_t) {
    event_t->interval = 0;
    int year = event_t->date_start.year;
    int month = event_t->date_start.month;
    int day = event_t->date_start.day;

    int endYear = event_t->date_end.year;
    int endMonth = event_t->date_end.month;
    int endDay = event_t->date_end.day;

    while (year < endYear || (year == endYear && month < endMonth) || (year == endYear && month == endMonth && day < endDay)) {
        event_t->interval++;
        day++;
        if (day > daysInMonth(month, year)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
}

//Dday 계산: 마감일 - 현재일
void calDday(Event* event_t, Time current) {
    event_t->Dday = 0;
    int year = current.year;
    int month = current.month;
    int day = current.day;

    int endYear = event_t->date_end.year;
    int endMonth = event_t->date_end.month;
    int endDay = event_t->date_end.day;

    while (year < endYear || (year == endYear && month < endMonth) || (year == endYear && month == endMonth && day < endDay)) {
        event_t->Dday++;
        day++;
        if (day > daysInMonth(month, year)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
}

// 가중치 계산: (100 * 1/Dday) + (사용자 입력 우선순위)
// Dday는 작을수록 중요한 일정이므로 역수로 계산
void calWeight(Event* event_t, int importance) {
    const double a = 100.0;
    const double b = 1.0;

    if (event_t->Dday == 0)
        event_t->weight = importance;
    else
        event_t->weight = a * (1.0 / event_t->Dday) + b * importance;
}

// 월의 시작일 계산
int calculateStartDayOfWeek(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }
    int dayOfWeek = (day + (13 * (month + 1)) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return (dayOfWeek + 6) % 7;
}