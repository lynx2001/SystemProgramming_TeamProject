#ifndef CALENDAR_H
#define CALENDAR_H

#define MONTHLY_CALENDAR 1
#define WEEKLY_CALENDAR 2
#define DAILY_CALENDAR 3

extern int year, month, day;
extern int state; // 화면 출력 상태(1: 큰 월간 캘린더, 2: 작은 월간 캘린더, 3: 주간 캘린더, 4: 일간 캘린더)

#endif
