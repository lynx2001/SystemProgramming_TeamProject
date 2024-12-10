#include <ncurses.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "habit.h"
#include "global.h"
#include "util.h"
#include "display.h"

time_t customParseDate(const char *date) {
    struct tm tm_info = {0};

    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
        return (time_t)-1;
    }

    char year[5], month[3], day[3];
    strncpy(year, date, 4); year[4] = '\0';
    strncpy(month, date + 5, 2); month[2] = '\0';
    strncpy(day, date + 8, 2); day[2] = '\0';

    tm_info.tm_year = atoi(year) - 1900;
    tm_info.tm_mon = atoi(month) - 1;
    tm_info.tm_mday = atoi(day);

    time_t result = mktime(&tm_info);
    if (result == -1) {
        fprintf(stderr, "Failed to parse date: %s\n", date);
    }
    return result;
}

// 현재 날짜를 가져오기
void getCurrentDate(char *date) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(date, 11, "%Y-%m-%d", tm_info);
}

void saveHabits() {
    FILE *file = fopen(HABIT_FILE, "w");
    if (!file) {
        printf("파일을 저장할 수 없습니다.\n");
        return;
    }

    // 첫 줄: 마지막 확인 날짜
    fprintf(file, "%s\n", last_checked_date);

    // 나머지 
    for (int i = 0; i < habit_count; i++) {
        fprintf(file, "%s %d %d\n", habits[i].name, habits[i].streak, habits[i].is_done);
    }
    fclose(file);
}

// 매일 전날과 비교해서 streak과 is_done 값을 초기화
void resetHabitsIfDateChanged() {
    char current_date[11];
    getCurrentDate(current_date);

    // 날짜 차이 계산
    time_t last_time = customParseDate(last_checked_date);
    time_t current_time = customParseDate(current_date);
    double diff_streak = difftime(current_time, last_time) / (60 * 60 * 24);

    
    if (diff_streak >= 1) {
        for (int i = 0; i < habit_count; i++) {
            if (diff_streak >= 2) {
                habits[i].streak = 0;
            }
            if(habits[i].is_done==0) { habits[i].streak = 0;}
            
            habits[i].is_done = 0; 
        }
        strcpy(last_checked_date, current_date);

    }
}

void signalresetHabits(){
    char current_date[11];
    getCurrentDate(current_date);
    
    for (int i = 0; i < habit_count; i++) {
            if(habits[i].is_done==0) habits[i].streak = 0;

            habits[i].is_done = 0; // 모든 과제 미완료 상태로 초기화
        }
        strcpy(last_checked_date, current_date);

        saveHabits();
}

// 파일에서 데이터 불러오기
void loadHabits() {
    FILE *file = fopen(HABIT_FILE, "r");
    if (!file) {
        // 파일이 없으면 새 파일 생성
        file = fopen(HABIT_FILE, "w");
        if (!file) {
            perror("파일 생성 실패");
            return;
        }
        getCurrentDate(last_checked_date); 
        fprintf(file, "%s\n", last_checked_date);
        fclose(file);

        habit_count = 0; 
        return;
    }
    

    if (fscanf(file, "%10s", last_checked_date) != 1) {
        getCurrentDate(last_checked_date); 

    }

    while (fscanf(file, "%49s %d %d", habits[habit_count].name, &habits[habit_count].streak, &habits[habit_count].is_done) == 3) {
        habit_count++;
        if (habit_count >= MAX_HABITS) break;
    }
    fclose(file);

    resetHabitsIfDateChanged();
}

// 습관 추가 함수
void add_habit() {
    char name[50];

    clear();
    echo();

    mvprintw(2, 10, "Add Habit:");
	
	// 습관 이름 입력
    if (get_input("Enter habit name: ", name, sizeof(name)) == -1) return;
    noecho();

    for (int i = 0; i < habit_count; i++) {
        if (strcmp(habits[i].name, name) == 0) {
            popup_message("Habit is already stored!");
            return;
        }
    }

    Habit new_habit;
    new_habit.id = ++last_habit_id;
    strncpy(new_habit.name, name, sizeof(new_habit.name));
    new_habit.streak = 0;
    new_habit.is_done = 0;

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
        mvprintw(6 + i, 12, "%d. %s (Streak: %d streak)", i + 1, habits[i].name, habits[i].streak);
    }

    mvprintw(6 + habit_count, 12, "Enter habit number: ");
    scanw("%d", &choice);

    if (choice >= 1 && choice <= habit_count) {
        if(habits[choice-1].is_done){
            popup_message("Habit already marked as success!");
        }
        else{
        habits[choice - 1].streak++;  // 성공하면 연속 일수를 증가
        habits[choice - 1].is_done = 1;
        popup_message("Habit marked as success!");
        }
    } else {
        popup_message("Invalid choice!");
    }

    noecho();
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
            saveHabits();
            break;
        }
    }
}
