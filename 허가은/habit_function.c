#include <stdio.h>
#include <string.h>
#include <time.h>
#include "habit.h"
#include "file_name.h"


Habit habits[MAX_HABITS];
int habit_count = 0;
char last_checked_date[11]; 

// 현재 날짜를 가져옴
void getCurrentDate(char *date) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(date, 11, "%Y-%m-%d", tm_info);
}

// 파일에서 데이터 불러오기
void loadHabits() {
    FILE *file = fopen(HABIT_FILE, "r");
    if (!file) {
        //printf("파일을 열 수 없습니다. 새 파일을 생성합니다.\n");
        getCurrentDate(last_checked_date); // 현재 날짜로 초기화
        return;
    }

    // 첫 줄: 마지막 확인 날짜
    if (fscanf(file, "%10s", last_checked_date) != 1) {
        getCurrentDate(last_checked_date); 
    }

    // 나머지 줄: 습관 데이터
    while (fscanf(file, "%49s %d %d", habits[habit_count].name, &habits[habit_count].days, &habits[habit_count].is_done) == 3) {
        habit_count++;
        if (habit_count >= MAX_HABITS) break;
    }
    fclose(file);
}

// 데이터를 파일에 저장
void saveHabits() {
    FILE *file = fopen(HABIT_FILE, "w");
    if (!file) {
        printf("파일을 저장할 수 없습니다.\n");
        return;
    }

    // 첫 줄: 마지막 확인 날짜 저장
    fprintf(file, "%s\n", last_checked_date);

    // 나머지 줄: 습관 데이터 저장
    for (int i = 0; i < habit_count; i++) {
        fprintf(file, "%s %d %d\n", habits[i].name, habits[i].days, habits[i].is_done);
    }
    fclose(file);
}

// 날짜가 변경되었는지 확인
int hasDateChanged() {
    char current_date[11];
    getCurrentDate(current_date);

    if (strcmp(last_checked_date, current_date) != 0) {
        strcpy(last_checked_date, current_date);
        return 1; // 날짜가 변경됨 : 다음날로 넘어감
    }
    return 0; // 같은날
}

// 다음날이면 is_done 값을 초기화하고, 미완료된 과제의 days 리셋
void resetHabitsIfDateChanged() {
    
    if (hasDateChanged()) {
        for (int i = 0; i < habit_count; i++) {
            if (habits[i].is_done == 0) {
                habits[i].days = 0; // 미완료 과제의 days 리셋
            }
            habits[i].is_done = 0; // 모든 과제를 미완료 상태로 초기화
        }
        printf("\033[41;37m!! 미완료된 과제는 연속 일수가 리셋됩니다 !!\033[0m\n");
    }
}

void showHabits() {
    printf("=== 오늘 해야 할 과제 ===\n");
    printf("완료된 과제:\n");
    for (int i = 0; i < habit_count; i++) {
        if (habits[i].is_done) {
            printf(" - %s (%d days)\n", habits[i].name, habits[i].days);
        }
    }

    printf("\n미완료된 과제:\n");
    for (int i = 0; i < habit_count; i++) {
        if (!habits[i].is_done) {
            printf(" - %s (%d days)\n", habits[i].name, habits[i].days);
        }
    }
    printf("===============================");
}

// 과제 추가
void addHabit(const char *name) {
    if (habit_count >= MAX_HABITS) {
        printf("목록이 가득 찼습니다.\n\n");
        return;
    }

    // 중복 확인
    for (int i = 0; i < habit_count; i++) {
        if (strcmp(habits[i].name, name) == 0) {
            printf("이미 저장된 과제입니다.\n\n");
            return;
        }
    }

    strcpy(habits[habit_count].name, name);
    habits[habit_count].days = 0;
    habits[habit_count].is_done = 0;
    habit_count++;
    printf("과제 '%s'가 추가되었습니다.\n\n", name);
    showHabits();
}

// 과제 완료 처리
void markHabitDone(const char *name) {
    for (int i = 0; i < habit_count; i++) {
        if (strcmp(habits[i].name, name) == 0) {
            if (habits[i].is_done) {
                printf("이미 완료된 과제입니다.\n\n");
                return;
            }
            habits[i].is_done = 1;
            habits[i].days++;
            printf("과제 '%s'가 완료 처리되었습니다.\n\n", name);
            showHabits();
            return;
        }
    }
    printf("해당 과제는 저장되어있지 않습니다.\n\n");
}

// 과제 삭제
void deleteHabit(const char *name) {
    for (int i = 0; i < habit_count; i++) {
        if (strcmp(habits[i].name, name) == 0) {
            for (int j = i; j < habit_count - 1; j++) {
                habits[j] = habits[j + 1];
            }
            habit_count--;
            printf("과제 '%s'를 삭제합니다.\n\n", name);
            showHabits();
            return;
        }
    }
    printf("과제를 찾을 수 없습니다.\n\n");
}
