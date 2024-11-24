#include <ncurses.h>
#include <string.h>
#include "calendar.h"

// 현재 달력 형식을 저장하는 전역 변수
static CalendarFormat current_format = MONTHLY;

// 달력 형식에 따른 텍스트를 반환하는 함수
static const char* get_calendar_text() {
    switch (current_format) {
        case MONTHLY: return "here monthly";
        case WEEKLY:  return "here weekly";
        case DAILY:   return "here daily";
        default:      return "unknown format";
    }
}

// 캘린더 초기화
void init_calendar() {
    current_format = MONTHLY;
}

// 캘린더 텍스트 출력
void draw_calendar_screen() {
    clear();  // 화면 초기화

    int height, width;
    getmaxyx(stdscr, height, width);  // 현재 터미널 크기 가져오기

    // 달력 형식 텍스트 위치 (상단 기준 마진 5)
    int calendar_text_y = 5;

    // 달력 형식 텍스트 출력
    mvprintw(calendar_text_y, (width - strlen(get_calendar_text())) / 2, "%s", get_calendar_text());
    refresh();
}

// 캘린더 형식 변경
void change_calendar_format() {
    current_format = (current_format + 1) % 3; // MONTHLY -> WEEKLY -> DAILY -> MONTHLY
    draw_calendar_screen(); // 화면 갱신
}

