#include <ncurses.h>
#include <string.h>
#include "global.h"
#include "util.h"
#include "scheduler.h"

void add_schedule() {
    char title[50], details[100];
    int year, month, day, year_end, month_end, day_end;
    double weight;
    double quantity;
    int reminder;

    clear();
    echo();

    mvprintw(2, 10, "Add Schedule:");

    mvprintw(5, 12, "Enter start date (YYYY MM DD): ");
    scanw("%d %d %d", &year, &month, &day);

    mvprintw(6, 12, "Enter end date (YYYY MM DD): ");
    scanw("%d %d %d", &year_end, &month_end, &day_end);

    mvprintw(7, 12, "Enter weight (1~5): ");
    scanw("%lf", &weight);

    mvprintw(8, 12, "Enter quantity: ");
    scanw("%lf", &quantity);

    mvprintw(9, 12, "Enter details: ");
    getstr(details);

    mvprintw(10, 12, "Set reminder (1: Yes, 0: No): ");
    scanw("%d", &reminder);

    noecho();

    // 새로운 스케줄 생성 및 데이터 저장
    Event new_event;
    new_event.id = ++last_id;
    strncpy(new_event.title, title, sizeof(new_event.title));
    new_event.date_start.year = year;
    new_event.date_start.month = month;
    new_event.date_start.day = day;
    new_event.date_end.year = year_end;
    new_event.date_end.month = month_end;
	new_event.date_end.day = day_end;
    new_event.weight = weight;
    new_event.quantity = quantity;
    strncpy(new_event.details, details, sizeof(new_event.details));

    // 배열에 새 스케줄 추가
    events[event_count++] = new_event;

    popup_message("Schedule successfully added!");
}
