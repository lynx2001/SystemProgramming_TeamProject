// global.c
#include "global.h"

Event events[MAX_EVENTS] = {0};  // events 배열 초기화
int event_count = 0;             // 이벤트 개수 초기화
int last_id = 0;                 // 마지막 ID 초기화

Habit habits[MAX_HABITS] = {0};  // habits 배열 초기화  
int habit_count = 0;             // 습관 개수 초기화
int last_habit_id = 0;           // 마지막 ID 초기화