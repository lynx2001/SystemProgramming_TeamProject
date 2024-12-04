#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <ncurses.h>
#include <string.h>
#include "AutoScheduling.h"

Event events[MAX_EVENTS];
int event_count = 0;



typedef struct ToDo {
    int id;
    char title[100];
    int is_done;
} ToDo;


void ask(char* message) {
    printf("******************\n");
    printf("%s\n", message);
    printf("******************\n");
}

int compareByWeight(const void* a, const void* b) {
    const Event* todoA = (const Event*)a;
    const Event* todoB = (const Event*)b;

    if (todoA->weight < todoB->weight) return 1;
    if (todoA->weight > todoB->weight) return -1;
    return 0;
}

int compareByStart(const void* a, const void* b) {
    const Event* todoA = (const Event*)a;
    const Event* todoB = (const Event*)b;

    if(todoA->date_start.year < todoB->date_start.year) return 1;
    else if(todoA->date_start.year > todoB->date_start.year) return -1;
    else {
        if(todoA->date_start.month < todoB->date_start.month) return 1;
        else if(todoA->date_start.month < todoB->date_start.month) return -1;
        else {
            if(todoA->date_start.day < todoB->date_start.day) return 1;
            else if(todoA->date_start.day < todoB->date_start.day) return -1;
            else return 0;
        }
    }
}

void sortTodo(Event* event_t, int count) {
    qsort(event_t, count, sizeof(Event), compareByWeight);
}



void printCalendar(Event* event_t) {
    int year = event_t->date_start.year;
    int month = event_t->date_start.month;
    double workProgress = 0;
    double dailyWorkload = event_t->quantity / (event_t->interval + 1);

    printf("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("%4d\n", year);
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    while (year < event_t->date_end.year || (year == event_t->date_end.year && month <= event_t->date_end.month)) {
        int days = daysInMonth(month, year);
        int startDayOfWeek = calculateStartDayOfWeek(year, month, 1);

        printf("\n%2d��\n", month);
        printf("\nSUN                  MON                  TUE                  WED                  THU                  FRI                  SAT\n");
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < startDayOfWeek; i++) {
            printf("                      ");
        }

        for (int d = 1; d <= days; d++) {
            if ((year > event_t->date_end.year) || (year == event_t->date_end.year && month == event_t->date_end.month && d > event_t->date_end.day)) {
                break;
            }
            if (year == event_t->date_start.year && month == event_t->date_start.month && d == event_t->date_start.day) {
                workProgress = 0;
            }
            if (year > event_t->date_start.year || month > event_t->date_start.month || d >= event_t->date_start.day) {
                workProgress += dailyWorkload;
                printf("%2d %s[%.1f] ", d, event_t->title, workProgress);
            }
            else {
                printf(" %2d                    ", d);
            }

            if ((startDayOfWeek + d) % 7 == 0) {
                printf("\n");
            }
        }
        printf("\n");

        month++;
        if (month > 12) {
            month = 1;
            year++;
            printf("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
            printf("%4d\n", year);
            printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
        }
    }
    printf("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
}
/*
void *date_checker(void* arg) {
    Event* events = (Event*)arg;
    struct tm current_date, new_date;
    time_t now = time(NULL);
    localtime_r(&now, &current_date);

    while(1) {
        sleep(60);
        now = time(NULL);
        localtime_r(&now, &new_date);

        if(new_date.tm_mday != current_date.tm_mday ||
            new_date.tm_mon != current_date.tm_mon ||
            new_date.tm_year != current_date.tm_year) {
                printf("Change date! %d-%d-%d\n", new_date.tm_year + 1900, new_date.tm_mon + 1, new_date.tm_mday);

                current_date = new_date;

                Time current;
                current.year = new_date.tm_year + 1900;
                current.month = new_date.tm_mon + 1;
                current.day = new_date.tm_mday;

                updateDdayAndWeights(events, event_count, current);
        }
    }
    return NULL;
}
*/

void handleDateChangeSignal(int signal) {
    if(signal == SIGUSR1) {
        updateDdayAndWeights(events, event_count);
    }
        
}

int main(int argc, char* argv[]) {
    Time current;
    initTime(&current);

    struct sigaction new_handler;
    new_handler.sa_sigaction = handleDateChangeSignal;//시그널발생시 처리 함수
    new_handler.sa_flags = 0; //일단 0으로 세팅
    sigemptyset(&new_handler.sa_mask);
    if (sigaction(SIGUSR1, &new_handler, NULL) == -1) {
        perror("sigaction error");
        return 1;
    }

    initializeDateMonitor();
    startDateMonitor();


    char title[50], details[100], buffer[20];
    int year_start, month_start, day_start, year_end, month_end, day_end;
    double weight;
    double quantity;
    int reminder;

    clear();
    echo();

    mvprintw(2, 10, "Add Schedule:");

	// 제목 입력
    if (get_input("Enter event title: ", title, sizeof(title)) == -1) return;

    // 시작 날짜 입력
    while (1) {
        if (get_input("Enter start date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year_start, &month_start, &day_start) == 3 && validateDate(year_start, month_start, day_start)) {
            break;
        }
        popup_message("Invalid date. Please try again.");
    }

	// 마감 날짜 입력
    while (1) {
        if (get_input("Enter end date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year_end, &month_end, &day_end) == 3 && validateDate(year_end, month_end, day_end)) {
            break;
        }
        popup_message("Invalid date. Please try again.");
    }

	// 가중치 입력
    while (1) {
        if (get_input("Enter weight (1~5): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%lf", &weight) == 1 && (weight == 1 || weight == 2 || weight == 3 || weight == 4 || weight == 5)) {
            break;
        }
        popup_message("Invalid input. Please enter 1 - 5.");
    }

	// 분량 입력
    while (1) {
        if (get_input("Enter quantity (integer): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%lf", &quantity) == 1) {	// 정수 여부 검증 필요
            break;
        }
        popup_message("Invalid input. Please enter integer.");
    }
	
	// 세부사항 입력
    if (get_input("Enter details: ", details, sizeof(details)) == -1) return;

    // 리마인더 입력
    while (1) {
        if (get_input("Set reminder (1: Yes, 0: No): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d", &reminder) == 1 && (reminder == 0 || reminder == 1)) {
            break;
        }
        popup_message("Invalid input. Please enter 1 or 0.");
    }

    noecho();

    // 새로운 스케줄 생성 및 데이터 저장
    Event new_event;
    new_event.id = ++last_id;
    strncpy(new_event.title, title, sizeof(new_event.title));
    new_event.date_start.year = year_start;
    new_event.date_start.month = month_start;
    new_event.date_start.day = day_start;
    new_event.date_end.year = year_end;
    new_event.date_end.month = month_end;
	new_event.date_end.day = day_end;
    new_event.weight = weight;
    new_event.quantity = quantity;
    strncpy(new_event.details, details, sizeof(new_event.details));

    // 배열에 새 스케줄 추가
    events[event_count++] = new_event;

    popup_message("Schedule successfully added!");






/*
    int continueFlag = 1;
    char userResponse;
    while (1) {
        if (continueFlag == 0 || event_count == 100)
            break;

        ask("Enter Name of ToDo");
        scanf("%s", events[event_count].title);

        ask("Enter Quantity of ToDo");
        scanf("%lf", &events[event_count].quantity);

        do {
            ask("Enter Weight of ToDo (0 ~ 5)");
            scanf("%d", &events[event_count].importance);
        } while (!validateImportance(events[event_count].importance));

        int startYear, startMonth, startDay;
        do {
            ask("Enter Start of Day. (YYYY-MM-DD)");
            scanf("%d-%d-%d", &startYear, &startMonth, &startDay);
        } while (!validateDate(startYear, startMonth, startDay));
        events[event_count].date_start.year = startYear;
        events[event_count].date_start.month = startMonth;
        events[event_count].date_start.day = startDay;

        int endYear, endMonth, endDay;
        do {
            ask("Enter End of Day. (YYYY-MM-DD)");
            scanf("%d-%d-%d", &endYear, &endMonth, &endDay);
        } while (!validateDate(endYear, endMonth, endDay));
        events[event_count].date_end.year = endYear;
        events[event_count].date_end.month = endMonth;
        events[event_count].date_end.day = endDay;

        intervalError(events[event_count].date_start, events[event_count].date_end);
        calInterval(&events[event_count]);
        calDday(&events[event_count], current);
        calWeight(&events[event_count], events[event_count].importance);

        event_count++;

        ask("Continue to enter your schedule? (Y/N)");
        scanf(" %c", &userResponse);

        if (userResponse == 'N' || userResponse == 'n')
            continueFlag = 0;
    }

    sortTodo(events, event_count);
    for (int i = 0; i < event_count; i++) {
        printCalendar(&events[i]);
    }
*/
    //pthread_join(date_thread, NULL);
    //add header file
    stopDateMonitor();
    return 0;
}