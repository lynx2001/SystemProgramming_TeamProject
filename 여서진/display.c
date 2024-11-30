#include <ncurses.h>
#include <string.h>
#include "display.h"
#include "global.h"
#include "event.h"
#include "habit.h"

// 리마인더 리스트 출력
static void draw_reminders(int start_y, int column_width) {
    mvprintw(start_y++, 5, "Reminders:");
    for (int i = 0; i < event_count; i++) {
        if (events[i].Dday <= 3) {  // D-day가 3일 이하인 이벤트만 출력
            mvprintw(start_y++, 5, "- %s (D-%.0f)", events[i].title, events[i].Dday + 1);
        }
    }
}

// 습관 리스트 출력
static void draw_habits(int start_y, int column_width) {
    mvprintw(start_y++, column_width + 5, "Habits:");
    for (int i = 0; i < habit_count; i++) {
        mvprintw(start_y++, column_width + 5, "- %s (Streak: %d days)", habits[i].name, habits[i].streak);
    }
}

// 가로 레이아웃 메뉴 출력
static void draw_horizontal_menu() {
    int height, width;
    getmaxyx(stdscr, height, width);

	(void)height;  // 높이를 명시적으로 무시
    
	const char *menu_items[] = {"1. Change Calendar Format", "2. Event Management",
                                "3. Habit Management", "q. Quit"};
    int menu_count = sizeof(menu_items) / sizeof(menu_items[0]);
    int menu_y = 10;
    int spacing = width / (menu_count + 1);

    // 가로 메뉴 출력
    for (int i = 0; i < menu_count; i++) {
        mvprintw(menu_y, spacing * (i + 1) - strlen(menu_items[i]) / 2, "%s", menu_items[i]);
    }

    refresh();
}

// 리스트 레이아웃 메뉴 출력
static void draw_vertical_menu() {
    // 세로 메뉴 출력
    mvprintw(5, 5, "1. Change Calendar Format");
    mvprintw(6, 5, "2. Event Management");
    mvprintw(7, 5, "3. Habit Management");
    mvprintw(8, 5, "q. Quit");
    refresh();
}

// 메인 메뉴 출력
void draw_main_menu() {
    int height, width;
    getmaxyx(stdscr, height, width);

    (void)height;  // 높이를 명시적으로 무시

	if (width >= 80) {
        draw_horizontal_menu();  // 가로 메뉴
    } else {
        draw_vertical_menu();    // 세로 메뉴
    }
}

// 리마인더와 습관 리스트 출력
void draw_lists() {
    int height, width;
    getmaxyx(stdscr, height, width);

	(void)height;  // 높이를 명시적으로 무시

    if (width >= 80) {
        // 가로 레이아웃
        int list_start_y = 15;
        int column_width = (width - 12) / 2;

        draw_reminders(list_start_y, column_width); // 리마인더 출력
        draw_habits(list_start_y, column_width);    // 습관 출력
    } else {
        // 세로 레이아웃
        int list_start_y = 12;

        draw_reminders(list_start_y, 0);  // 리마인더 출력
        draw_habits(list_start_y + 6, 0); // 습관 출력 (리마인더 아래에 배치)
    }

    refresh();
}
