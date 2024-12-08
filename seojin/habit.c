#include <ncurses.h>
#include <string.h>
#include "habit.h"
#include "global.h"
#include "util.h"
#include "display.h"

Habit habits[MAX_HABITS];
int habit_count = 0;
int last_habit_id = 0;

// 습관 추가 함수
void add_habit() {
    char name[50];

    clear();
    echo();

    mvprintw(2, 10, "Add Habit:");
	
	// 습관 이름 입력
    if (get_input("Enter habit name: ", name, sizeof(name)) == -1) return;
    noecho();

    Habit new_habit;
    new_habit.id = ++last_habit_id;
    strncpy(new_habit.name, name, sizeof(new_habit.name));
    new_habit.streak = 0;

    habits[habit_count++] = new_habit;

    popup_message("Habit successfully added!");
}

// 습관 변경 함수
void change_habit() {
    clear();
    int choice;
    echo();

    mvprintw(2, 10, "Change Habit:");
    mvprintw(4, 12, "Select a habit to change:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Streak: %d)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(6 + habit_count, 12, "Enter habit number: ");
    scanw("%d", &choice);

    if (choice >= 1 && choice <= habit_count) {
        char new_name[50];
        mvprintw(6 + habit_count + 1, 12, "Enter new name: ");
        getstr(new_name);

        strncpy(habits[choice - 1].name, new_name, sizeof(habits[choice - 1].name));
        popup_message("Habit successfully changed!");
    } else {
        popup_message("Invalid choice!");
    }

    noecho();
}

// 습관 삭제 함수
void delete_habit() {
    clear();
    int choice;
    echo();

    mvprintw(2, 10, "Delete Habit:");
    mvprintw(4, 12, "Select a habit to delete:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Streak: %d)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(6 + habit_count, 12, "Enter habit number: ");
    scanw("%d", &choice);

    if (choice >= 1 && choice <= habit_count) {
        for (int i = choice - 1; i < habit_count - 1; i++) {
            habits[i] = habits[i + 1];
        }
        habit_count--;
        popup_message("Habit successfully deleted!");
    } else {
        popup_message("Invalid choice!");
    }

    noecho();
}

// 습관 성공 여부 입력
void mark_habit_success() {
    clear();
    int choice;
    echo();

    mvprintw(2, 10, "Mark Habit Success:");
    mvprintw(4, 12, "Select a habit to mark success:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Streak: %d days)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(6 + habit_count, 12, "Enter habit number: ");
    scanw("%d", &choice);

    if (choice >= 1 && choice <= habit_count) {
        habits[choice - 1].streak++;  // 성공하면 연속 일수를 증가
        popup_message("Habit marked as success!");
    } else {
        popup_message("Invalid choice!");
    }

    noecho();
}

// 습관 관리 서브 메뉴
void habit_submenu() {
    draw_habit_screen();

	int choice;
    
	while (1) {
        choice = getch();

        if (choice == '1') {
            add_habit();
        } else if (choice == '2') {
            change_habit();
        } else if (choice == '3') {
            delete_habit();
        } else if (choice == '4') {
            mark_habit_success();
        } else if (choice == '5') {
	        current_screen = MAIN_SCREEN;
			break;
        }
    }
}
