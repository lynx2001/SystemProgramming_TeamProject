#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include "habit.h"
#include "file_name.h"

#include "habit_function.h"

pthread_t date_check_thread;  // 날짜 확인 스레드
int program_running = 1; 


Habit habits[MAX_HABITS];
int habit_count = 0;
char last_checked_date[11]; 


//strptime이 되지 않아서.. 따로 만든 함수
time_t customParseDate(const char *date) {
    struct tm tm_info = {0};

    if (strlen(date) != 10 || date[4] != '-' || date[7] != '-') {
        fprintf(stderr, "Invalid date format: %s\n", date);
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


// 매일 is_done 값을 초기화
void resetHabitsIfDateChanged() {
    char current_date[11];
    getCurrentDate(current_date);

    // 날짜 차이 계산
    time_t last_time = customParseDate(last_checked_date);
    time_t current_time = customParseDate(current_date);
    double diff_days = difftime(current_time, last_time) / (60 * 60 * 24);

    
    if (diff_days >= 1) {
        printf("\033[41;37m!! 날짜가 변경되어 모든 과제가 미완료로 리셋되었습니다 !!\033[0m\n");

        for (int i = 0; i < habit_count; i++) {
            if (diff_days >= 2) {
                // 1일 이상 건너뛴 경우 미완료된 과제의 days 리셋
                habits[i].days = 0;
            }
            habits[i].is_done = 0; // 모든 과제 미완료 상태로 초기화
        }

        // 마지막 확인 날짜 갱신
        strcpy(last_checked_date, current_date);

        
        saveHabits();
    }
}

void *dateCheckThread(void *arg) {
    while (program_running) {
        sleep(10);  // 10초마다 확인
        resetHabitsIfDateChanged();
    }
    return NULL;
}

int hasDateChanged() {
    char current_date[11];
    getCurrentDate(current_date);

    if (strcmp(last_checked_date, current_date) != 0) {
        strcpy(last_checked_date, current_date);
        return 1; // 날짜가 변경됨 
    }
    return 0; 
}

// 파일에서 데이터 불러오기
void loadHabits() {
    FILE *file = fopen(HABIT_FILE, "r");
    if (!file) {
        getCurrentDate(last_checked_date); 
        return;
    }

    // 첫 줄: 마지막 확인 날짜
    if (fscanf(file, "%10s", last_checked_date) != 1) {
        getCurrentDate(last_checked_date); 
    }

    // 나머지 줄
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

    // 첫 줄: 마지막 확인 날짜
    fprintf(file, "%s\n", last_checked_date);

    // 나머지 
    for (int i = 0; i < habit_count; i++) {
        fprintf(file, "%s %d %d\n", habits[i].name, habits[i].days, habits[i].is_done);
    }
    fclose(file);
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
    printf("%s 목록에 없는 과제 입니다.\n\n", name);
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