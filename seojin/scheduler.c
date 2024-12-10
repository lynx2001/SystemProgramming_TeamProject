#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include "global.h"
#include "util.h"
#include "scheduler.h"

void add_schedule() {
    char title[50] = {0}, details[100] = {0};
    char start_date[20] = {0}, end_date[20] = {0};
    char weight[10] = {0}, quantity[10] = {0}, reminder[5] = {0};

    // 입력 필드 정의
    InputField fields[] = {
        {"Enter title", title, sizeof(title), NULL},  // 제목은 유효성 검사 없음
        {"Enter start date (YYYY MM DD)", start_date, sizeof(start_date), validate_date_wrapper},
        {"Enter end date (YYYY MM DD)", end_date, sizeof(end_date), validate_date_wrapper},
        {"Enter weight (1-5)", weight, sizeof(weight), validate_weight},
        {"Enter quantity", quantity, sizeof(quantity), validate_quantity},
        {"Enter reminder (1: Yes, 0: No)", reminder, sizeof(reminder), validate_reminder},
        {"Enter details", details, sizeof(details), NULL}  // 세부 사항은 유효성 검사 없음
    };

    // UI 화면 정의
    UIScreen screen = {"Add Schedule", fields, sizeof(fields) / sizeof(fields[0])};

    // 공통 입력 처리 함수 호출
    active_screen = &screen;
    current_step = 0;

    if (process_user_input(&screen) == 0) {
        // 데이터 저장
        Event new_event;

        // 제목 복사
        strncpy(new_event.title, title, sizeof(new_event.title));

        // 날짜 변환 및 유효성 검사
        sscanf(start_date, "%d %d %d", &new_event.date_start.year, &new_event.date_start.month, &new_event.date_start.day);
        sscanf(end_date, "%d %d %d", &new_event.date_end.year, &new_event.date_end.month, &new_event.date_end.day);

        // 가중치와 분량 변환
        new_event.weight = atof(weight);
        new_event.quantity = atof(quantity);

        // 리마인더 값 변환
        new_event.Dday = (atoi(reminder) == 1) ? 1 : 0;

        // 세부사항 복사
        strncpy(new_event.details, details, sizeof(new_event.details));

        // ID 및 이벤트 저장
        new_event.id = ++last_id;
        events[event_count++] = new_event;

        popup_message("Schedule successfully added!");
    } else {
        popup_message("Schedule creation canceled.");
    }

    active_screen = NULL; // 현재 UI 화면 초기화
}

