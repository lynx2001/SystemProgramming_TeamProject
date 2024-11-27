#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>

// 서브화면 상수 정의
#define EVENT_SUBMENU 1
#define HABIT_SUBMENU 2
#define SCHEDULER_SUBMENU 3

// 전역 변수 선언 (extern)
extern bool is_in_submenu;
extern int current_submenu;

#endif
