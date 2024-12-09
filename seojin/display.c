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

// 일정 관리 서브메뉴 화면
void draw_event_screen() {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);

    mvprintw(height / 2 - 4, (width - 25) / 2, "1. Add Event");
    mvprintw(height / 2 - 2, (width - 25) / 2, "2. Modify Event");
   	mvprintw(height / 2, (width - 25) / 2, "3. Delete Event");
    mvprintw(height / 2 + 2, (width - 25) / 2, "4. Auto Scheduling");
    mvprintw(height / 2 + 4, (width - 25) / 2, "5. Back to Main Menu");
    
	refresh();
}

// 습관 관리 서브메뉴 화면
void draw_habit_screen() {
    clear();
    int height, width;
    getmaxyx(stdscr, height, width);

	mvprintw(height / 2 - 4, (width - 30) / 2, "1. Add Habit");
    mvprintw(height / 2 - 2, (width - 30) / 2, "2. Change Habit");
    mvprintw(height / 2, (width - 30) / 2, "3. Delete Habit");
    mvprintw(height / 2 + 2, (width - 30) / 2, "4. Mark Habit Success");
    mvprintw(height / 2 + 4, (width - 30) / 2, "5. Back to Main Menu");
    
	refresh();
}

void draw_add_event_ui(EventInputState *state) {
    clear();
    mvprintw(2, 10, "Add Event:");

    // 제목 출력
    mvprintw(4, 12, "Enter event title: %s", state->title[0] ? state->title : "(not set)");

    // 시작 날짜 출력
    mvprintw(6, 12, "Enter start date (YYYY MM DD): %04d-%02d-%02d", state->date_start.year ? state->date_start.year : 0,
             state->date_start.month ? state->date_start.month : 0,
             state->date_start.day ? state->date_start.day : 0);

    // 시작 시간 출력
    mvprintw(8, 12, "Enter start time (HH MM): %02d:%02d", state->date_start.hour ? state->date_start.hour : 0,
             state->date_start.minute ? state->date_start.minute : 0);

    // 종료 날짜 출력
    mvprintw(10, 12, "Enter end date (YYYY MM DD): %04d-%02d-%02d", state->date_end.year ? state->date_end.year : 0,
             state->date_end.month ? state->date_end.month : 0,
             state->date_end.day ? state->date_end.day : 0);

    // 종료 시간 출력
    mvprintw(12, 12, "Enter end time (HH MM): %02d:%02d", state->date_end.hour ? state->date_end.hour : 0,
             state->date_end.minute ? state->date_end.minute : 0);

    // 리마인더 출력
    mvprintw(14, 12, "Set reminder (1: Yes, 0: No): %d", state->reminder);
    
	// 세부 사항 출력
    mvprintw(16, 12, "Enter event details: %s", state->details[0] ? state->details : "(not set)");

    refresh();
}

