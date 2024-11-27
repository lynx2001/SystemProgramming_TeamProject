#include <ncurses.h>
#include <string.h>
#include <math.h>
#include "display.h"
#include "global.h"
#include "habit.h"

// 메뉴 출력
void draw_main_menu() {
    int height, width;
    getmaxyx(stdscr, height, width);  // 현재 터미널 크기 가져오기

    // 메뉴 출력 위치 (달력 텍스트 아래 마진 5)
    int menu_y = 10;
    const char *menu_items[] = {"1. Change Calendar Format", "2. Event Management",
                                "3. Habit Management", "4. Scheduler Management", "q. Quit"};
    int menu_count = sizeof(menu_items) / sizeof(menu_items[0]);

    // 메뉴 아이템을 가로로 일정 간격으로 출력
    int spacing = width / (menu_count + 1);
    for (int i = 0; i < menu_count; i++) {
        mvprintw(menu_y, spacing * (i + 1) - strlen(menu_items[i]) / 2, "%s", menu_items[i]);
    }

    refresh();
}

// 리마인더 조건 확인
static int is_within_3_days(Event *event) {
    // 현재 날짜와 비교해 남은 일수 계산 (예시: 3일 이내인지 확인)
    // 실제 구현에서는 날짜 비교 로직 추가 필요
    return event->Dday <= 3;
}

// 리마인더 리스트와 습관 목록 출력
void draw_lists() {
    int height, width;
    getmaxyx(stdscr, height, width);  // 현재 터미널 크기 가져오기

    // 리마인더와 습관 출력 영역 설정
    int list_start_y = 15;  // 메뉴 아래로 마진 5
    int list_height = height - list_start_y - 5; // 하단에서 위로 마진 5까지
    int column_width = (width - 12) / 2;  // 리스트 사이 마진 2, 양쪽 마진 5 포함

    // 리마인더 출력 (왼쪽)
    mvprintw(list_start_y - 1, 5, "Reminders:");
    int reminder_y = list_start_y;
    for (int i = 0; i < event_count; i++) {
        if (is_within_3_days(&events[i])) {
            if (reminder_y < list_start_y + list_height) {
				mvprintw(reminder_y++, 5, "[%s] - D-%.0f", events[i].title, events[i].Dday + 1);
            }
        }
    }

    // 습관 목록 출력 (오른쪽)
    mvprintw(list_start_y - 1, width - column_width - 5, "Habits:");
    int habit_y = list_start_y;
    for (int i = 0; i < habit_count; i++) {
        if (habit_y < list_start_y + list_height) {
            mvprintw(habit_y++, width - column_width - 5, "[%s] - streak: %ddays", habits[i].name, habits[i].streak);
        }
    }

    refresh();
}
