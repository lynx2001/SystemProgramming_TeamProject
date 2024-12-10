#include <ncurses.h>
#include <string.h>
#include "habit.h"
#include "global.h"
#include "util.h"
#include "display.h"
#include <stdlib.h>

Habit habits[MAX_HABITS];
int habit_count = 0;
int last_habit_id = 0;

// 습관 추가 함수
void add_habit() {
    if (habit_count >= MAX_HABITS) {
        popup_message("Cannot add more habits. Maximum limit reached!");
        return;
    }

    // 입력 필드 정의
    char name[50] = {0};

    InputField fields[] = {
        {"Enter habit name", name, sizeof(name)}
    };

    UIScreen habit_screen = {
        "Add Habit",
        fields,
        sizeof(fields) / sizeof(fields[0])
    };

    // 공통 입력 처리
    active_screen = &habit_screen; // 현재 UI 화면 설정
    current_step = 0;

    if (process_user_input(&habit_screen) == 0) {
        // 습관 데이터 저장
        Habit new_habit;
        new_habit.id = ++last_habit_id;
        strncpy(new_habit.name, name, sizeof(new_habit.name));
        new_habit.streak = 0;

        habits[habit_count++] = new_habit;

        popup_message("Habit successfully added!");
    } else {
        popup_message("Habit addition canceled.");
    }

    active_screen = NULL; // 현재 UI 화면 초기화
}

// 습관 변경 함수
void change_habit() {
    if (habit_count == 0) {
        popup_message("No habits to change!");
        return;
    }

    // 1. 현재 존재하는 습관 목록 출력
    clear();
    mvprintw(2, 10, "Change Habit:");
    mvprintw(4, 10, "Select a habit to change:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 10, "%d. %s (Streak: %d)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(7 + habit_count, 10, "Enter the number of the habit (or :b to go back): ");
    refresh();

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > habit_count) {
        popup_message("Invalid choice. Please try again.");
        return;
    }

    Habit *habit = &habits[choice - 1];

    // 3. 수정 UI 준비
    char new_name[50] = {0};
    snprintf(new_name, sizeof(new_name), "%s", habit->name);

    InputField fields[] = {
        {"Modify habit name", new_name, sizeof(new_name)}
    };

    UIScreen modify_screen = {
        "Change Habit",
        fields,
        sizeof(fields) / sizeof(fields[0])
    };

    // 4. 공통 입력 처리
    active_screen = &modify_screen; // 현재 UI 화면 설정
    current_step = 0;

    if (process_user_input(&modify_screen) == 0) {
        // 5. 수정 데이터 반영
        strncpy(habit->name, new_name, sizeof(habit->name));
        popup_message("Habit successfully changed!");
    } else {
        popup_message("Change canceled.");
    }

    active_screen = NULL; // 현재 UI 화면 초기화
}

// 습관 삭제 함수
void delete_habit() {
    if (habit_count == 0) {
        popup_message("No habits to delete!");
        return;
    }

    // 1. 현재 존재하는 습관 목록 출력
    clear();
    mvprintw(2, 10, "Delete Habit:");
    mvprintw(4, 10, "Select a habit to delete:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 10, "%d. %s (Streak: %d)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(7 + habit_count, 10, "Enter the number of the habit (or :b to go back): ");
    refresh();

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > habit_count) {
        popup_message("Invalid choice. Please try again.");
        return;
    }

    Habit *habit = &habits[choice - 1];

    // 3. 삭제 확인 메시지 출력
    char confirmation[128];
    snprintf(confirmation, sizeof(confirmation), "Delete \"%s\"? (y/n):", habit->name);

    int confirm = popup_confirmation(confirmation);  // 사용자 확인 요청
    if (confirm == 1) {
        // 4. 습관 삭제 처리
        for (int i = choice - 1; i < habit_count - 1; i++) {
            habits[i] = habits[i + 1];
        }
        habit_count--;

        popup_message("Habit successfully deleted!");
    } else {
        popup_message("Deletion canceled.");
    }
}

// 습관 성공 여부 입력
void mark_habit_success() {
    if (habit_count == 0) {
        popup_message("No habits to mark success!");
        return;
    }

    // 1. 현재 존재하는 습관 목록 출력
    clear();
    mvprintw(2, 10, "Mark Habit Success:");
    mvprintw(4, 10, "Select a habit to mark success:");

    for (int i = 0; i < habit_count; i++) {
        mvprintw(6 + i, 10, "%d. %s (Streak: %d days)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(7 + habit_count, 10, "Enter the number of the habit (or :b to go back): ");
    refresh();

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > habit_count) {
        popup_message("Invalid choice. Please try again.");
        return;
    }

    // 3. 성공 처리
    habits[choice - 1].streak++;  // 성공하면 연속 일수를 증가
    popup_message("Habit marked as success!");
}

// 습관 관리 서브 메뉴
void habit_submenu() {
	current_screen = HABIT_SCREEN;
    draw_habit_screen();

	int choice;
    
	while (1) {
        choice = getch();

        if (choice == '1') {
			current_screen = DEFAULT_SCREEN;
            add_habit();
			clear();
			
			current_screen = HABIT_SCREEN;
			draw_habit_screen();
        } else if (choice == '2') {
			current_screen = DEFAULT_SCREEN;
            change_habit();
			clear();
			
			current_screen = HABIT_SCREEN;
			draw_habit_screen();
        } else if (choice == '3') {
			current_screen = DEFAULT_SCREEN;
            delete_habit();
			clear();
			
			current_screen = HABIT_SCREEN;
			draw_habit_screen();
        } else if (choice == '4') {
			current_screen = DEFAULT_SCREEN;
            mark_habit_success();
			clear();
			
			current_screen = HABIT_SCREEN;
			draw_habit_screen();
        } else if (choice == '5') {
	        current_screen = MAIN_SCREEN;
			break;
        }
    }
}
