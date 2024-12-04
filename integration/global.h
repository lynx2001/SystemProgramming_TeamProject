#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_EVENTS 100
#define EVENT_FILE "event.txt"
#define ID_FILE "last_id.txt"

// 화면 상수
#define MAIN_SCREEN 0
#define EVENT_SCREEN 1
#define HABIT_SCREEN 2

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Time;

typedef struct {
    int id;
    char title[50];
    Time date_start;
    Time date_end;
    double quantity;    // 총 분량 / 총 일수 = 하루당 분량
    int importance;     // 0 ~ 5 사이 정수
    char details[100];  // 상세 기록

    // 아래는 계산되는 값
    double interval;    // 마감일 - 시작일
    double Dday;        // 마감일 - 현재일(now)
    double weight;      // 가중치 식에 따라 계산 (지역변수 importance를 사용해서 계산) >> default : -1

    /* default 정리 (일반 일정에 필요 없는 값)  >> 다 기본값 -1
    quantity
    importance
    interval
    weight
    */
} Event;

extern Event events[MAX_EVENTS];
extern int event_count;
extern int last_id;

extern int current_screen;

#endif
