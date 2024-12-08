#include <ncurses.h>
#include <string.h>
#include "event.h"
#include "global.h"
#include "util.h"
#include "scheduler.h"
#include "display.h"

// 이벤트 관리 서브 메뉴
void event_submenu() {
	current_screen = EVENT_SCREEN;
    draw_event_screen();

	int choice;
	
	while (1) {
        choice = getch();  // 사용자 입력 대기

        if (choice == '1') {
            addEvents();  // 일정 추가
			current_screen = DEFAULT_SCREEN;
        } else if (choice == '2') {
            modifyEvents();  // 일정 수정
			current_screen = DEFAULT_SCREEN;
        } else if (choice == '3') {
            deleteEvents();  // 일정 삭제
			current_screen = DEFAULT_SCREEN;
		} else if (choice == '4') {
            printf("Auto Scheduling here\n");
			//add_schedule();  // 오토 스케줄링
			current_screen = DEFAULT_SCREEN;
        } else if (choice == '5') {
            break;  // 초기화면 복귀
        }
        // 변경된 event 및 last_event_id 를 txt에 바로 반영
        saveEvents();
    }
}

// 일정 txt에서 불러오기
void loadEvents() {
    FILE *file = fopen(EVENT_FILE, "r");
    if (file == NULL) {
        // 파일이 존재하지 않는 경우, 파일 생성 및 초기 세팅
        file = fopen(EVENT_FILE, "w");
        if (!file) {
            perror("파일 생성 실패");
            return;
        }
        
        last_event_id = 0;
        event_count = 0;
        return;
    }

    // 파일이 존재하는 경우, 파일에서 값 읽기
    if (fscanf(file, "%d", last_event_id) != 1) {   // 읽을 값이 없는 경우
        last_event_id = 0;
    }
    
    char buffer[512];
    event_count = 0;

    while (fgets(buffer, sizeof(buffer), file) && event_count < MAX_EVENTS) {
        Event *event = &events[event_count];
        sscanf(buffer,
               "%d|%49[^|]|%d-%d-%d %d:%d|%d-%d-%d %d:%d|%d|%lf|%lf|%lf|%lf|%99[^\n]",
               &event->id, event->title,
               &event->date_start.year, &event->date_start.month, &event->date_start.day,
               &event->date_start.hour, &event->date_start.minute,
               &event->date_end.year, &event->date_end.month, &event->date_end.day,
               &event->date_end.hour, &event->date_end.minute,
               &event->importance, &event->quantity, &event->interval, &event->Dday, &event->weight,
               event->details);

        event_count++;
    }

    fclose(file);
    printf("Debug: %d개의 이벤트가 로드되었습니다.\n", event_count);
}

// 일정 txt에 저장하기
void saveEvents() {
    FILE *file = fopen(EVENT_FILE, "w");
    if (file == NULL) {
        printf("파일 열기 오류\n");
        return;
    }

    // 첫 줄: 마지막 Event ID
    fprintf(file, "%d\n", last_event_id);

    for (int i = 0; i < event_count; i++) {
        Event *event = &events[i];
        fprintf(file, "%d|%s|%d-%02d-%02d %02d:%02d|%d-%02d-%02d %02d:%02d|%.2d|%.2lf|%.2lf|%.2lf|%.2lf|%s\n",
                event->id, event->title,
                event->date_start.year, event->date_start.month, event->date_start.day,
                event->date_start.hour, event->date_start.minute,
                event->date_end.year, event->date_end.month, event->date_end.day,
                event->date_end.hour, event->date_end.minute,
                event->importance, event->quantity, event->interval, event->Dday, event->weight,
                event->details);
    }

    fclose(file);
    printf("Debug: 일정 저장이 완료되었습니다.\n");
}

// 일정 추가
void addEvents() {
    if (event_count >= MAX_EVENTS) {
        popup_message("Event list is full. Cannot add more events.");
        return;
    }

    Event event;
    char buffer[128];
    int year, month, day, hour, minute;
    int reminder;

    clear();
    echo();

    mvprintw(2, 10, "Add Event:");

    // 제목 입력
    if (get_input("Enter event title: ", event.title, sizeof(event.title)) == -1) return;

    // 시작 날짜 입력
    while (1) {
        if (get_input("Enter start date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year, &month, &day) == 3 && validateDate(year, month, day)) {
            event.date_start.year = year;
            event.date_start.month = month;
            event.date_start.day = day;
            break;
        }
        popup_message("Invalid start date. Please try again.");
    }

    // 시작 시간 입력
    while (1) {
        if (get_input("Enter start time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d", &hour, &minute) == 2 && validateTime(hour, minute)) {
            event.date_start.hour = hour;
            event.date_start.minute = minute;
            break;
        }
        popup_message("Invalid start time. Please try again.");
    }

    // 종료 날짜 입력
    while (1) {
        if (get_input("Enter end date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d %d", &year, &month, &day) == 3 && validateDate(year, month, day)) {
            event.date_end.year = year;
            event.date_end.month = month;
            event.date_end.day = day;
            break;
        }
        popup_message("Invalid end date. Please try again.");
    }

    // 종료 시간 입력
    while (1) {
        if (get_input("Enter end time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d %d", &hour, &minute) == 2 && validateTime(hour, minute)) {
            event.date_end.hour = hour;
            event.date_end.minute = minute;
            break;
        }
        popup_message("Invalid end time. Please try again.");
    }

    // 중요도 입력
    if (get_input("Enter importance (0-5): ", buffer, sizeof(buffer)) == -1) return;
    sscanf(buffer, "%d", &event.importance);

    // 기타 값 입력 (기본값 -1 설정)
    event.quantity = -1;
    event.interval = -1;
    event.Dday = -1;
    event.weight = -1;

    // 세부 사항 입력
    if (get_input("Enter event details: ", event.details, sizeof(event.details)) == -1) return;

    // 리마인더 입력 (Question? 저장해야 하는 필드인가요)
    while (1) {
        if (get_input("Set reminder (1: Yes, 0: No): ", buffer, sizeof(buffer)) == -1) return;
        if (sscanf(buffer, "%d", &reminder) == 1 && (reminder == 0 || reminder == 1)) {
            break;
        }
        popup_message("Invalid input. Please enter 1 or 0.");
    }

    noecho();

    // Event 저장
    event.id = ++last_event_id;
    events[event_count++] = event;

    popup_message("Event successfully added!");  // 성공 메시지 출력
}

// 일정 수정 (삭제 후 추가)
void modifyEvents() {
    if (event_count == 0) {
        popup_message("No events to modify!");
        return;
    }

    clear();
    echo();


    // 수정할 이벤트 리스트 출력
    mvprintw(2, 10, "Modify Event:");
    mvprintw(4, 12, "Select an event to modify:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Start: %02d-%02d | End: %02d:%02d)", 
            i + 1, events[i].title,
            events[i].date_start.month, events[i].date_start.day,
            events[i].date_end.month, events[i].date_end.day);
    }

    int choice;
    mvprintw(6 + event_count, 12, "Enter event number: ");
    scanw("%d", &choice);  // 수정할 이벤트 번호 선택

    if (choice >= 1 && choice <= event_count) {
        Event *event = &events[choice - 1];

        // 선택된 이벤트 수정
        // 제목 수정
        mvprintw(7 + event_count, 12, "Modify title (current: %s): ", event->title);
        getstr(event->title);

        // 시작 날짜 수정
        while (1) {
            mvprintw(8 + event_count, 12, "Modify start date (YYYY MM DD) (current: %04d-%02d-%02d): ",
                     event->date_start.year, event->date_start.month, event->date_start.day);
            scanw("%d %d %d", &event->date_start.year, &event->date_start.month, &event->date_start.day);
            if (validateDate(event->date_start.year, event->date_start.month, event->date_start.day)) {
                break;
            }
            popup_message("Invalid date. Please try again.");
        }

        // 시작 시간 수정
        while (1) {
            mvprintw(9 + event_count, 12, "Modify start time (HH MM) (current: %02d:%02d): ",
                     event->date_start.hour, event->date_start.minute);
            scanw("%d %d", &event->date_start.hour, &event->date_start.minute);
            if (validateTime(event->date_start.hour, event->date_start.minute)) {
                break;
            }
            popup_message("Invalid time. Please try again.");
        }

        // 종료 날짜 수정
        while (1) {
            mvprintw(10 + event_count, 12, "Modify end date (YYYY MM DD) (current: %04d-%02d-%02d): ",
                     event->date_end.year, event->date_end.month, event->date_end.day);
            scanw("%d %d %d", &event->date_end.year, &event->date_end.month, &event->date_end.day);
            if (validateDate(event->date_end.year, event->date_end.month, event->date_end.day)) {
                break;
            }
            popup_message("Invalid date. Please try again.");
        }

        // 종료 시간 수정
        while (1) {
            mvprintw(11 + event_count, 12, "Modify end time (HH MM) (current: %02d:%02d): ",
                     event->date_end.hour, event->date_end.minute);
            scanw("%d %d", &event->date_end.hour, &event->date_end.minute);
            if (validateTime(event->date_end.hour, event->date_end.minute)) {
                break;
            }
            popup_message("Invalid time. Please try again.");
        }

        // 중요도 수정
        mvprintw(12 + event_count, 12, "Modify importance (current: %d, range: 0-5): ", event->importance);
        scanw("%d", &event->importance);

        // 세부 사항 수정
        mvprintw(13 + event_count, 12, "Modify details (current: %s): ", event->details);
        getstr(event->details);

        popup_message("Event successfully modified!"); // 성공 메시지 출력
    } else {
        popup_message("Invalid choice!"); // 유효하지 않은 선택
    }

    noecho();
}

// 일정 삭제
void deleteEvents() {
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
