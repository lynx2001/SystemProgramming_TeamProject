#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "AutoScheduling.h"
#include "event.h"
#include "SchedulingErrorCheck.h"

Event events[MAX_EVENTS];
event_count = 0;


typedef struct ToDo {
    int id;
    char name[100];
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

        printf("\n%2d¿ù\n", month);
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
                printf("%2d %s[%.1f] ", d, event_t->name, workProgress);
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

int main(int argc, char* argv[]) {
    Time current;
    initTime(&current);

    int continueFlag = 1;
    char userResponse;
    while (1) {
        if (continueFlag == 0 || event_count == 100)
            break;

        int importance = 0;

        ask("Enter Name of ToDo");
        scanf("%s", events[event_count].name);

        ask("Enter Quantity of ToDo");
        scanf("%lf", &events[event_count].quantity);

        do {
            ask("Enter Weight of ToDo (0 ~ 5)");
            scanf("%d", &importance);
        } while (!validateImportance(importance));

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
        calWeight(&events[event_count], importance);

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


    return 0;
}