#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define MAX_TODO 100

int todoCnt;

time_t timer;
struct tm* t;

typedef struct Event {
    int id;
    int start[3];
    int end[3];
    double interval;        //시작일 ~ 마감일
    char name[100];
    double Dday;            //현재일 ~ 마감일
    double weight;          //우선순위(가중치)
    double quantity;        //분량
} Event;

typedef struct Time {
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Time;

typedef struct ToDo {
    int id;
    char name[100];
    int is_done;
} ToDo;

typedef struct currentTime {
    int year;
    int month;
    int day;
} currentTime;

void initTime(currentTime* x) {
    timer = time(NULL);
    t = localtime(&timer);

    x->year = t->tm_year + 1900;
    x->month = t->tm_mon + 1;
    x->day = t->tm_mday;
}

void ask(char* message) {
    printf("******************\n");
    printf("%s\n", message);
    printf("******************\n");
}

int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    switch (month) {
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return isLeapYear(year) ? 29 : 28;
    default:
        return 31;
    }
}

void intervalError(int start[], int end[]) {
    if (start[0] > end[0]) {
        fprintf(stderr, "시작연도가 마감연도보다 늦습니다.\n");
        exit(1);
    }
    else if (start[0] == end[0] && start[1] > end[1]) {
        fprintf(stderr, "시작월이 마감월보다 늦습니다.\n");
        exit(1);
    }
    else if (start[0] == end[0] && start[1] == end[1] && start[2] > end[2]) {
        fprintf(stderr, "시작일이 마감일보다 늦습니다.\n");
        exit(1);
    }
    else {
        printf("Success interval\n");
    }
}

void calInterval(Event* t) {
    t->interval = 0;
    int year = t->start[0];
    int month = t->start[1];
    int day = t->start[2];

    int endYear = t->end[0];
    int endMonth = t->end[1];
    int endDay = t->end[2];

    while (year < endYear || (year == endYear && month < endMonth) || (year == endYear && month == endMonth && day < endDay)) {
        t->interval++;
        day++;
        if (day > daysInMonth(month, year)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
}

void calDday(Event* t, currentTime current) {
    t->Dday = 0;
    int year = current.year;
    int month = current.month;
    int day = current.day;

    int endYear = t->end[0];
    int endMonth = t->end[1];
    int endDay = t->end[2];

    while (year < endYear || (year == endYear && month < endMonth) || (year == endYear && month == endMonth && day < endDay)) {
        t->Dday++;
        day++;
        if (day > daysInMonth(month, year)) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
}

void calWeight(Event* t, int importance) {
    const double a = 100.0;
    const double b = 1.0;

    if (t->Dday == 0)
        t->weight = importance;
    else
        t->weight = a * (1.0 / t->Dday) + b * importance;
}

int calculateStartDayOfWeek(int year, int month, int day) {
    if (month < 3) {
        month += 12;
        year--;
    }
    int dayOfWeek = (day + (13 * (month + 1)) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return (dayOfWeek + 6) % 7;
}

int compareByWeight(const void* a, const void* b) {
    const Event* todoA = (const Event*)a;
    const Event* todoB = (const Event*)b;

    if (todoA->weight < todoB->weight) return 1;
    if (todoA->weight > todoB->weight) return -1;
    return 0;
}

void sortTodo(Event* t, int count) {
    qsort(t, count, sizeof(Event), compareByWeight);
}

int validateDate(int year, int month, int day) {
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) {
        fprintf(stderr, "날짜 형식 또는 범위가 올바르지 않습니다. 다시 입력해 주세요.\n");
        return 0;
    }
    return 1;
}

int validateImportance(int importance) {
    if (importance < 0 || importance > 5) {
        fprintf(stderr, "중요도는 0에서 5 사이의 값이어야 합니다. 다시 입력해 주세요.\n");
        return 0;
    }
    return 1;
}

void printCalendar(Event* t) {
    int year = t->start[0];
    int month = t->start[1];
    double index = 0;
    double div = t->quantity / (t->interval + 1);

    printf("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    printf("%4d\n", year);
    printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    while (year < t->end[0] || (year == t->end[0] && month <= t->end[1])) {
        int days = daysInMonth(month, year);
        int startDayOfWeek = calculateStartDayOfWeek(year, month, 1);

        printf("\n%2d월\n", month);
        printf("\nSUN                  MON                  TUE                  WED                  THU                  FRI                  SAT\n");
        printf("------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

        for (int i = 0; i < startDayOfWeek; i++) {
            printf("                      ");
        }

        for (int d = 1; d <= days; d++) {
            if ((year > t->end[0]) || (year == t->end[0] && month == t->end[1] && d > t->end[2])) {
                break;
            }
            if (year == t->start[0] && month == t->start[1] && d == t->start[2]) {
                index = 0;
            }
            if (year > t->start[0] || month > t->start[1] || d >= t->start[2]) {
                index += div;
                printf("%2d %s[%.1f] ", d, t->name, index);
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
    Event todoList[MAX_TODO];
    todoCnt = 0;

    currentTime current;
    initTime(&current);

    int flag = 1;
    char reaction;
    while (1) {
        if (flag == 0 || todoCnt == 100)
            break;

        int importance = 0;

        ask("Enter Name of ToDo");
        scanf("%s", todoList[todoCnt].name);

        ask("Enter Quantity of ToDo");
        scanf("%lf", &todoList[todoCnt].quantity);

        do {
            ask("Enter Weight of ToDo (0 ~ 5)");
            scanf("%d", &importance);
        } while (!validateImportance(importance));

        int startYear, startMonth, startDay;
        do {
            ask("Enter Start of Day. (YYYY-MM-DD)");
            scanf("%d-%d-%d", &startYear, &startMonth, &startDay);
        } while (!validateDate(startYear, startMonth, startDay));
        todoList[todoCnt].start[0] = startYear;
        todoList[todoCnt].start[1] = startMonth;
        todoList[todoCnt].start[2] = startDay;

        int endYear, endMonth, endDay;
        do {
            ask("Enter End of Day. (YYYY-MM-DD)");
            scanf("%d-%d-%d", &endYear, &endMonth, &endDay);
        } while (!validateDate(endYear, endMonth, endDay));
        todoList[todoCnt].end[0] = endYear;
        todoList[todoCnt].end[1] = endMonth;
        todoList[todoCnt].end[2] = endDay;

        intervalError(todoList[todoCnt].start, todoList[todoCnt].end);
        calInterval(&todoList[todoCnt]);
        calDday(&todoList[todoCnt], current);
        calWeight(&todoList[todoCnt], importance);

        todoCnt++;

        ask("Continue to enter your schedule? (Y/N)");
        scanf(" %c", &reaction);

        if (reaction == 'N' || reaction == 'n')
            flag = 0;
    }

    sortTodo(todoList, todoCnt);
    for (int i = 0; i < todoCnt; i++) {
        printCalendar(&todoList[i]);
    }


    return 0;
}
