#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "CalculateScheduling.h"

time_t timer;
struct tm* t;

void initTime(Time* x) {
    timer = time(NULL);
    t = localtime(&timer);

    x->year = t->tm_year + 1900;
    x->month = t->tm_mon + 1;
    x->day = t->tm_mday;
}

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

void calWeight(Event* event_t, int importance) {
    const double a = 100.0;
    const double b = 1.0;

    if (event_t->Dday == 0)
        event_t->weight = importance;
    else
        event_t->weight = a * (1.0 / event_t->Dday) + b * importance;
}

int calculateStartDayOfWeek(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }
    int dayOfWeek = (day + (13 * (month + 1)) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return (dayOfWeek + 6) % 7;
}

void updateDdayAndWeights(Event events[], int count, Time current) {
    for(int i = 0; i < count; i++) {
        calDday(&events[i], current);
        calWeight(&events[i], events[i].importance);
    }
}