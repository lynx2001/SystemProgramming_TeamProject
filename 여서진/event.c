#include <ncurses.h>
#include <string.h>
#include "event.h"
#include "global.h"
#include "util.h"

Event events[MAX_EVENTS];
int event_count = 0;  // 현재 저장된 이벤트 개수
int last_id = 0;      // 마지막으로 부여된 이벤트 ID

void add_event() {
    char title[50], details[100];
    int year, month, day, hour, minute;
    int reminder;

    clear();
    echo();

    mvprintw(2, 10, "Add Event:");

    // 제목 입력
    mvprintw(4, 12, "Enter event title: ");
    getstr(title);

    // 날짜 입력 검증
    while (1) {
        mvprintw(5, 12, "Enter start date (YYYY MM DD): ");
        clrtoeol();  // 현재 줄 지우기
        int result = scanw("%d %d %d", &year, &month, &day);

        if (result == 3 && validateDate(year, month, day)) {
            break;  // 유효한 입력
        }

        popup_message("Invalid date. Please try again.");  // 팝업 메시지 출력
    }

    // 시간 입력 검증
    while (1) {
        mvprintw(6, 12, "Enter start time (HH MM): ");
        clrtoeol();  // 현재 줄 지우기
        int result = scanw("%d %d", &hour, &minute);

        if (result == 2 && validateTime(hour, minute)) {
            break;  // 유효한 입력
        }

        popup_message("Invalid time. Please try again.");  // 팝업 메시지 출력
    }

    // 세부사항 입력
    mvprintw(9, 12, "Enter details: ");
    getstr(details);

    // 리마인더 입력 검증
    while (1) {
        mvprintw(10, 12, "Set reminder (1: Yes, 0: No): ");
        clrtoeol();  // 현재 줄 지우기
        int result = scanw("%d", &reminder);

        if (result == 1 && (reminder == 0 || reminder == 1)) {
            break;  // 유효한 입력
        }

        popup_message("Invalid input. Please enter 1 or 0.");  // 팝업 메시지 출력
    }

    noecho();

    // 새로운 이벤트 데이터 저장
    Event new_event;
    new_event.id = ++last_id;
    strncpy(new_event.title, title, sizeof(new_event.title));
    new_event.date_start.year = year;
    new_event.date_start.month = month;
    new_event.date_start.day = day;
    new_event.date_start.hour = hour;
    new_event.date_start.minute = minute;
    strncpy(new_event.details, details, sizeof(new_event.details));

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
    int choice;
	is_in_submenu = true;  // 서브화면 상태 진입
	current_submenu = EVENT_SUBMENU;  // 현재 서브화면 설정
	
	while (1) {
		clear();
        int height, width;
        getmaxyx(stdscr, height, width);  // 현재 터미널 크기 가져오기

        // 메뉴 옵션 출력
        mvprintw(height / 2 - 4, (width - 25) / 2, "1. Add Event");
        mvprintw(height / 2 - 2, (width - 25) / 2, "2. Modify Event");
        mvprintw(height / 2, (width - 25) / 2, "3. Delete Event");
        mvprintw(height / 2 + 2, (width - 25) / 2, "4. Back to Main Menu");

        refresh();
        choice = getch();  // 사용자 입력 대기

        if (choice == '1') {
            add_event();  // 일정 추가
        } else if (choice == '2') {
            modify_event();  // 일정 수정
        } else if (choice == '3') {
            delete_event();  // 일정 삭제
        } else if (choice == '4') {
            break;  // 초기화면 복귀
        }
    }
	
	current_submenu = 0;  // 서브화면 상태 초기화	
	is_in_submenu = false;  // 서브화면 상태 종료
}
