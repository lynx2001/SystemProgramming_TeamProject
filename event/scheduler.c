#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <ncurses.h>
#include <string.h>
#include <time.h>
#include "global.h"
#include "util.h"
#include "scheduler.h"
#include "date_check.h"

time_t timer;
struct tm* t;

void initTime(Time* x) {
    timer = time(NULL);
    t = localtime(&timer);

    x->year = t->tm_year + 1900;
    x->month = t->tm_mon + 1;
    x->day = t->tm_mday;
}


//Interval Calculation
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
    event_t->interval++;
}

//Dday Calculation

void calDday(Event* event_t, Time current) {
    event_t->Dday = 0;
    int year = current.year;
    int month = current.month;
    int day = current.day;

    int endYear = event_t->date_end.year;
    int endMonth = event_t->date_end.month;
    int endDay = event_t->date_end.day;

    if (year > endYear || (year == endYear && month > endMonth) || (year == endYear && month == endMonth && day > endDay)) {
        event_t->Dday = -1;
        return;
    }
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


//Weigth Calcuation
void calWeight(Event* event_t, int importance) {
    const double a = 100.0;
    const double b = 1.0;

    if (event_t->Dday == 0)
        event_t->weight = importance;
    else
        event_t->weight = a * (1.0 / event_t->Dday) + b * importance;
}

//Thread: Updaye Dday and Weight By DayChange
void updateDdayAndWeights(Event events[], int count) {
    Time current;
    initTime(&current);

    for(int i = 0; i < count; i++) {
        calDday(&events[i], current);
        calWeight(&events[i], events[i].importance);
    }
}

void add_schedule() {
    char title[50], details[100], buffer[20];
    int year_start, month_start, day_start, year_end, month_end, day_end;
    double importance;
    double quantity;
    int reminder;

    Time current;
    initTime(&current);


    clear();
    echo();

    mvprintw(2, 10, "Add Schedule:");

	// 제목 입력
    if (get_input("Enter event title: ", title, sizeof(title)) == -1) return;

    // 시작 날짜 입력
    while (1) {
        if (get_input("Enter start date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year_start, &month_start, &day_start) == 3 && validateDate(year_start, month_start, day_start)) {
            break;
        }
        popup_message("Invalid date. Please try again.");
    }

	// 마감 날짜 입력
    while (1) {
        if (get_input("Enter end date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year_end, &month_end, &day_end) == 3 && validateDate(year_end, month_end, day_end)) {
            break;
        }
        popup_message("Invalid date. Please try again.");
    }

	// 가중치 입력
    while (1) {
        if (get_input("Enter weight (0~5): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%lf", &importance) == 1 && (importance == 0 || importance == 1 || importance == 2 || importance == 3 || importance == 4 || importance == 5)) {
            break;
        }
        popup_message("Invalid input. Please enter 1 - 5.");
    }

	// 분량 입력
    while (1) {
        if (get_input("Enter quantity (integer): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%lf", &quantity) == 1) {	// 정수 여부 검증 필요
            break;
        }
        popup_message("Invalid input. Please enter integer.");
    }
	
	// 세부사항 입력
    if (get_input("Enter details: ", details, sizeof(details)) == -1) return;

    // 리마인더 입력
    while (1) {
        if (get_input("Set reminder (1: Yes, 0: No): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d", &reminder) == 1 && (reminder == 0 || reminder == 1)) {
            break;
        }
        popup_message("Invalid input. Please enter 1 or 0.");
    }

    noecho();

    // 새로운 스케줄 생성 및 데이터 저장
    Event new_event;
    new_event.id = ++last_event_id;
    strncpy(new_event.title, title, sizeof(new_event.title));

    new_event.date_start.year = year_start;
    new_event.date_start.month = month_start;
    new_event.date_start.day = day_start;
    new_event.date_start.hour = 24;
    new_event.date_start.minute = 0;

    new_event.date_end.year = year_end;
    new_event.date_end.month = month_end;
	new_event.date_end.day = day_end;
    new_event.date_end.hour = 24;
    new_event.date_end.minute = 0;

    new_event.importance = importance;
    new_event.reminder = reminder;
    strncpy(new_event.details, details, sizeof(new_event.details));
    calInterval(&new_event);
    calDday(&new_event, current);
    calWeight(&new_event, importance);
    new_event.quantity = quantity / new_event.interval;
    
    // 배열에 새 스케줄 추가
    events[event_count++] = new_event;

    //Sort by Weight
    sortTodo(events, event_count);

    popup_message("Schedule successfully added!");
}
