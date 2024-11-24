#ifndef CALENDAR_H
#define CALENDAR_H

// 달력 형식 열거형
typedef enum {
    MONTHLY,
    WEEKLY,
    DAILY
} CalendarFormat;

void init_calendar();              // 캘린더 초기화
void draw_calendar_screen();       // 캘린더 텍스트 출력
void change_calendar_format();     // 캘린더 형식 변경

#endif

