#include <ncurses.h>
#include <string.h>
#include "event.h"
#include "global.h"
#include "util.h"
#include "scheduler.h"
#include "display.h"

Event events[MAX_EVENTS];
int event_count = 0;  // 현재 저장된 이벤트 개수
int last_id = 0;      // 마지막으로 부여된 이벤트 ID

// 전역 변수 정의
EventInputState *active_state = NULL;

void add_event() {
	EventInputState state = {0};  // 입력 상태 초기화
 	active_state = &state;  // 전역 포인터에 로컬 변수 주소 저장

	char buffer[128];
	echo();

	while (state.current_step < 7) {
        // 화면 출력
        draw_add_event_ui(&state);

        switch (state.current_step) {
            case 0:  // 제목 입력
                if (get_input("Enter event title: ", state.title, sizeof(state.title)) == -1) return;
                state.current_step++;
                break;

            case 1:  // 시작 날짜 입력
                if (get_input("Enter start date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d %d", &state.date_start.year, &state.date_start.month, &state.date_start.day) == 3 && validateDate(state.date_start.year, state.date_start.month, state.date_start.day)) {
                    state.current_step++;
                } else {
                    popup_message("Invalid start date. Please try again.");
                }
                break;

            case 2:  // 시작 시간 입력
                if (get_input("Enter start time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d", &state.date_start.hour, &state.date_start.minute) == 2 && validateTime(state.date_start.hour, state.date_start.minute)) {
                    state.current_step++;
                } else {
                    popup_message("Invalid start time. Please try again.");
                }
                break;

            case 3:  // 종료 날짜 입력
                if (get_input("Enter end date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d %d", &state.date_end.year, &state.date_end.month, &state.date_end.day) == 3 && validateDate(state.date_end.year, state.date_end.month, state.date_end.day)) {
                    state.current_step++;
                } else {
                    popup_message("Invalid start date. Please try again.");
                }
                break;

            case 4:  // 종료 시간 입력
                if (get_input("Enter end time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d", &state.date_end.hour, &state.date_end.minute) == 2 && validateTime(state.date_end.hour, state.date_end.minute)) {
                    state.current_step++;
                } else {
                    popup_message("Invalid start time. Please try again.");
                }
                break;

            case 5:  // 리마인더 입력
                if (get_input("Set reminder (1: Yes, 0: No): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d", &state.reminder) == 1 && (state.reminder == 0 || state.reminder == 1)) {
                    state.current_step++;
                } else {
                    popup_message("Invalid start time. Please try again.");
                }
                break;
            case 6:  // 세부 사항 입력
                if (get_input("Enter event details: ", state.details, sizeof(state.details)) == -1) return;
                state.current_step++;
                break;

			default:
				return;
		}
	}

	active_state = NULL;	// 포인터 초기화
	noecho();

    // 이벤트 데이터 저장
    Event new_event;
    strncpy(new_event.title, state.title, sizeof(new_event.title));
    new_event.date_start.year = state.date_start.year;
    new_event.date_start.month = state.date_start.month;
    new_event.date_start.day = state.date_start.day;
    new_event.date_start.hour = state.date_start.hour;
    new_event.date_start.minute = state.date_start.minute;
    strncpy(new_event.details, state.details, sizeof(new_event.details));
	new_event.interval = 0;
    new_event.Dday = 0;
    events[event_count++] = new_event;

    popup_message("Event successfully added!");  // 성공 메시지 출력
}

// 일정 수정 함수
void modify_event() {
    clear();
    echo();

    // 수정할 이벤트 리스트 출력
    mvprintw(2, 10, "Modify Event:");
    mvprintw(4, 12, "Select an event to modify:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Start: %04d-%02d-%02d %02d:%02d)", 
            i + 1, events[i].title,
            events[i].date_start.year, events[i].date_start.month, events[i].date_start.day,
            events[i].date_start.hour, events[i].date_start.minute);
    }

    int choice;
    mvprintw(6 + event_count, 12, "Enter event number: ");
    scanw("%d", &choice);  // 수정할 이벤트 번호 선택

    if (choice >= 1 && choice <= event_count) {
        Event *event = &events[choice - 1];

        // 선택된 이벤트 수정
        mvprintw(7 + event_count, 12, "Modify title (current: %s): ", event->title);
        getstr(event->title);

        mvprintw(8 + event_count, 12, "Modify start date (YYYY MM DD): ");
        scanw("%d %d %d", &event->date_start.year, &event->date_start.month, &event->date_start.day);

        mvprintw(9 + event_count, 12, "Modify start time (HH MM): ");
        scanw("%d %d", &event->date_start.hour, &event->date_start.minute);

        mvprintw(10 + event_count, 12, "Modify details (current: %s): ", event->details);
        getstr(event->details);

        popup_message("Event successfully modified!");  // 성공 메시지 출력
    } else {
        popup_message("Invalid choice!");  // 유효하지 않은 선택
    }

    noecho();
}

// 일정 삭제 함수
void delete_event() {
    clear();
    echo();

    // 삭제할 이벤트 리스트 출력
    mvprintw(2, 10, "Delete Event:");
    mvprintw(4, 12, "Select an event to delete:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Start: %04d-%02d-%02d %02d:%02d)", 
            i + 1, events[i].title,
            events[i].date_start.year, events[i].date_start.month, events[i].date_start.day,
            events[i].date_start.hour, events[i].date_start.minute);
    }

    int choice;
    mvprintw(6 + event_count, 12, "Enter event number: ");
    scanw("%d", &choice);  // 삭제할 이벤트 번호 선택

    if (choice >= 1 && choice <= event_count) {
        // 선택된 이벤트 삭제
        for (int i = choice - 1; i < event_count - 1; i++) {
            events[i] = events[i + 1];  // 배열 재정렬
        }
        event_count--;  // 이벤트 개수 감소
        popup_message("Event successfully deleted!");  // 성공 메시지 출력
    } else {
        popup_message("Invalid choice!");  // 유효하지 않은 선택
    }

    noecho();
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
        } else if (choice == '3') {
			current_screen = DEFAULT_SCREEN;
            delete_event();  // 일정 삭제
			clear();
			
			current_screen = EVENT_SCREEN;
		} else if (choice == '4') {
			current_screen = DEFAULT_SCREEN;
			add_schedule();  // 오토 스케줄링
			clear();
			
			current_screen = EVENT_SCREEN;
        } else if (choice == '5') {
        	current_screen = MAIN_SCREEN;
			break;  // 초기화면 복귀
        }
    }
}
