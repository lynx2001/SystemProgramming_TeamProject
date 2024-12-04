#include <ncurses.h>
#include <string.h>
#include "global.h"
#include "util.h"
#include "scheduler.h"

void add_schedule() {
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
}
