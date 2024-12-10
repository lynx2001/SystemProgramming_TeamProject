#include <ncurses.h>
#include <string.h>
#include "event.h"
#include "global.h"
#include "util.h"
#include "scheduler.h"
#include "display.h"
#include <stdlib.h>
Event events[MAX_EVENTS];
int event_count = 0;  // 현재 저장된 이벤트 개수
int last_id = 0;      // 마지막으로 부여된 이벤트 ID

// 전역 변수 정의
EventInputState *active_state = NULL;

void add_event() {
    char title[50] = {0}, details[100] = {0};
    char start_date[20] = {0}, start_time[10] = {0};
    char end_date[20] = {0}, end_time[10] = {0};
    char reminder[5] = {0};

    // 입력 필드 정의
	InputField fields[] = {
    	{"Enter event title", title, sizeof(title), NULL},  // 제목은 유효성 검사 없음
    	{"Enter start date (YYYY MM DD)", start_date, sizeof(start_date), validate_date_wrapper},
    	{"Enter start time (HH MM)", start_time, sizeof(start_time), validate_time_wrapper},
    	{"Enter end date (YYYY MM DD)", end_date, sizeof(end_date), validate_date_wrapper},
    	{"Enter end time (HH MM)", end_time, sizeof(end_time), validate_time_wrapper},
    	{"Set reminder (1: Yes, 0: No)", reminder, sizeof(reminder), validate_reminder},
    	{"Enter event details", details, sizeof(details), NULL}  // 세부 사항은 유효성 검사 없음
	};

    // UI 화면 정의
    UIScreen screen = {"Add Event", fields, sizeof(fields) / sizeof(fields[0])};

    // 공통 입력 처리 함수 호출
    if (process_user_input(&screen) == 0) {
        Event new_event;

        // 데이터 파싱 및 유효성 검사
        sscanf(start_date, "%d %d %d", &new_event.date_start.year, &new_event.date_start.month, &new_event.date_start.day);
        sscanf(start_time, "%d %d", &new_event.date_start.hour, &new_event.date_start.minute);
        sscanf(end_date, "%d %d %d", &new_event.date_end.year, &new_event.date_end.month, &new_event.date_end.day);
        sscanf(end_time, "%d %d", &new_event.date_end.hour, &new_event.date_end.minute);
        new_event.reminder = atoi(reminder);
        strncpy(new_event.title, title, sizeof(new_event.title));
        strncpy(new_event.details, details, sizeof(new_event.details));

        // 이벤트 저장
        events[event_count++] = new_event;

        popup_message("Event successfully added!");
    }
}

// 일정 수정 함수
void modify_event() {
    if (event_count == 0) {
        popup_message("No events to modify!");
        return;
    }

    // 1. 현재 존재하는 이벤트 목록 출력
    clear();
    mvprintw(2, 10, "Modify Event:");
    mvprintw(4, 10, "Select an event to modify:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 10, "%d. %s (Start: %04d-%02d-%02d %02d:%02d)", 
                 i + 1, events[i].title,
                 events[i].date_start.year, events[i].date_start.month, events[i].date_start.day,
                 events[i].date_start.hour, events[i].date_start.minute);
    }

    mvprintw(7 + event_count, 10, "Enter the number of the event (or :b to go back): ");
    refresh();

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > event_count) {
        popup_message("Invalid choice. Please try again.");
        return;
    }

    Event *event = &events[choice - 1];

    // 3. 수정 UI 준비
    char title[50] = {0}, details[100] = {0};
    char start_date[20] = {0}, start_time[10] = {0};

    snprintf(title, sizeof(title), "%s", event->title);
    snprintf(details, sizeof(details), "%s", event->details);
    snprintf(start_date, sizeof(start_date), "%04d-%02d-%02d", 
             event->date_start.year, event->date_start.month, event->date_start.day);
    snprintf(start_time, sizeof(start_time), "%02d:%02d", 
             event->date_start.hour, event->date_start.minute);

    InputField fields[] = {
        {"Modify title", title, sizeof(title)},
        {"Modify start date (YYYY MM DD)", start_date, sizeof(start_date)},
        {"Modify start time (HH MM)", start_time, sizeof(start_time)},
        {"Modify details", details, sizeof(details)}
    };

    UIScreen modify_screen = {
        "Modify Event Details",
        fields,
        sizeof(fields) / sizeof(fields[0])
    };

    // 4. 공통 입력 처리
    active_screen = &modify_screen; // 현재 UI 화면 설정
    current_step = 0;

    if (process_user_input(&modify_screen) == 0) {
        // 5. 수정 데이터 반영
        strncpy(event->title, title, sizeof(event->title));
        sscanf(start_date, "%d %d %d", &event->date_start.year, &event->date_start.month, &event->date_start.day);
        sscanf(start_time, "%d %d", &event->date_start.hour, &event->date_start.minute);
        strncpy(event->details, details, sizeof(event->details));

        popup_message("Event successfully modified!");
    } else {
        popup_message("Modification canceled.");
    }

    active_screen = NULL; // 현재 UI 화면 초기화
}


// 일정 삭제 함수
void delete_event() {
    if (event_count == 0) {
        popup_message("No events to delete!");
        return;
    }

    // 1. 현재 이벤트 목록 출력
    clear();
    mvprintw(2, 10, "Delete Event:");
    mvprintw(4, 10, "Select an event to delete:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 10, "%d. %s (Start: %04d-%02d-%02d %02d:%02d)", 
                 i + 1, events[i].title,
                 events[i].date_start.year, events[i].date_start.month, events[i].date_start.day,
                 events[i].date_start.hour, events[i].date_start.minute);
    }

    mvprintw(7 + event_count, 10, "Enter the number of the event (or :b to go back): ");
    refresh();

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > event_count) {
        popup_message("Invalid choice. Please try again.");
        return;
    }

    Event *event = &events[choice - 1];

    // 3. 삭제 확인 팝업 메시지 준비
    char confirmation[128] = "Delete \"";
    strcat(confirmation, event->title);
    strcat(confirmation, "\"? (y/n):");

    // 팝업 확인
    int confirm = popup_confirmation(confirmation);
    if (confirm == 0) { // 취소
        popup_message("Deletion canceled.");
        return;
    }

    // 4. 이벤트 삭제
    for (int i = choice - 1; i < event_count - 1; i++) {
        events[i] = events[i + 1]; // 배열 재정렬
    }
    event_count--; // 이벤트 개수 감소

    popup_message("Event successfully deleted!");
}

// 이벤트 관리 서브 메뉴
void event_submenu() {
	current_screen = EVENT_SCREEN;
	draw_event_screen();

	int choice;
	
	while (1) {
        choice = getch();  // 사용자 입력 대기

        if (choice == '1') {
			current_screen = EVENT_ADD;
            add_event();  // 일정 추가
			clear();
			
			current_screen = EVENT_SCREEN;
			draw_event_screen();
        } else if (choice == '2') {
			current_screen = DEFAULT_SCREEN;
            modify_event();  // 일정 수정
			clear();
			
			current_screen = EVENT_SCREEN;
			draw_event_screen();
        } else if (choice == '3') {
			current_screen = DEFAULT_SCREEN;
            delete_event();  // 일정 삭제
			clear();
			
			current_screen = EVENT_SCREEN;
			draw_event_screen();
		} else if (choice == '4') {
			current_screen = DEFAULT_SCREEN;
			add_schedule();  // 오토 스케줄링
			clear();
			
			current_screen = EVENT_SCREEN;
			draw_event_screen();
        } else if (choice == '5') {
        	current_screen = MAIN_SCREEN;
			break;  // 초기화면 복귀
        }
    }
}
