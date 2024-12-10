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
			current_screen = DEFAULT_SCREEN;
            addEvents();  // 일정 추가
			clear();

			current_screen = EVENT_SCREEN;
			draw_event_screen();
        } else if (choice == '2') {
			current_screen = DEFAULT_SCREEN;
            modifyEvents();  // 일정 수정
			clear();

			current_screen = EVENT_SCREEN;
			draw_event_screen();
        } else if (choice == '3') {
			current_screen = DEFAULT_SCREEN;
            deleteEvents();  // 일정 삭제
			clear();

			current_screen = EVENT_SCREEN;
			draw_event_screen();
		} else if (choice == '4') {
			current_screen = DEFAULT_SCREEN;
            printf("Auto Scheduling here\n");
			add_schedule();  // 오토 스케줄링
			clear();

			current_screen = EVENT_SCREEN;
			draw_event_screen();
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

    char buffer[512];
    event_count = 0;
	
    // 파일이 존재하는 경우, 파일에서 값 읽기
    if (fscanf(file, "%d", &last_event_id) != 1) {   // 읽을 값이 없는 경우
        last_event_id = 0;
    }
    else // 읽을 값이 있는 경우
    {
        fgets(buffer, sizeof(buffer), file);
    }

    while (fgets(buffer, sizeof(buffer), file) && event_count < MAX_EVENTS) {
        Event *event = &events[event_count];
        sscanf(buffer,
               "%d|%49[^|]|%d-%d-%d %d:%d|%d-%d-%d %d:%d|%d|%lf|%lf|%lf|%lf|%d|%99[^\n]",
               &event->id, event->title,
               &event->date_start.year, &event->date_start.month, &event->date_start.day,
               &event->date_start.hour, &event->date_start.minute,
               &event->date_end.year, &event->date_end.month, &event->date_end.day,
               &event->date_end.hour, &event->date_end.minute,
               &event->importance, &event->quantity, &event->interval,
               &event->Dday, &event->weight, &event->reminder,
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
        fprintf(file, "%d|%s|%d-%02d-%02d %02d:%02d|%d-%02d-%02d %02d:%02d|%.2d|%.2lf|%.2lf|%.2lf|%.2lf|%d|%s\n",
                event->id, event->title,
                event->date_start.year, event->date_start.month, event->date_start.day,
                event->date_start.hour, event->date_start.minute,
                event->date_end.year, event->date_end.month, event->date_end.day,
                event->date_end.hour, event->date_end.minute,
                event->importance, event->quantity, event->interval,
                event->Dday, event->weight, event->reminder,
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

    Event event = {0}; // 초기화
    Time current;
    initTime(&current); // 현재 시간 초기화

    char start_date[20] = {0}, start_time[10] = {0};
    char end_date[20] = {0}, end_time[10] = {0};
    char reminder[5] = {0};
    char details[100] = {0};

    // 입력 필드 정의
    InputField fields[] = {
        {"Enter event title", event.title, sizeof(event.title), NULL}, // 제목은 유효성 검사 없음
        {"Enter start date (YYYY MM DD)", start_date, sizeof(start_date), validate_date_wrapper},
        {"Enter start time (HH MM)", start_time, sizeof(start_time), validate_time_wrapper},
        {"Enter end date (YYYY MM DD)", end_date, sizeof(end_date), validate_date_wrapper},
        {"Enter end time (HH MM)", end_time, sizeof(end_time), validate_time_wrapper},
        {"Set reminder (1: Yes, 0: No)", reminder, sizeof(reminder), validate_reminder},
        {"Enter event details", details, sizeof(details), NULL} // 세부 사항은 유효성 검사 없음
    };

    // UI 화면 정의
    UIScreen screen = {"Add Event", fields, sizeof(fields) / sizeof(fields[0])};

    // 공통 입력 처리 함수 호출
    active_screen = &screen;
    current_step = 0;

    if (process_user_input(&screen) == 0) {
        // 데이터 파싱 및 유효성 검사
        sscanf(start_date, "%d %d %d", &event.date_start.year, &event.date_start.month, &event.date_start.day);
        sscanf(start_time, "%d %d", &event.date_start.hour, &event.date_start.minute);
        sscanf(end_date, "%d %d %d", &event.date_end.year, &event.date_end.month, &event.date_end.day);
        sscanf(end_time, "%d %d", &event.date_end.hour, &event.date_end.minute);
        event.reminder = atoi(reminder);
        strncpy(event.details, details, sizeof(event.details));

        // D-day 계산
        calDday(&event, current);

        // 기타 값 초기화 (일반 일정 기본값 설정)
        event.importance = 0; 
        event.quantity = -1;
        event.interval = -1;
        event.weight = -1;

        // Event 저장
        event.id = ++last_event_id;
        events[event_count++] = event;

        popup_message("Event successfully added!"); // 성공 메시지 출력
    } else {
        popup_message("Event creation canceled."); // 취소 메시지 출력
    }

    active_screen = NULL;
}

// 일정 수정 (삭제 후 추가)
void modifyEvents() {
    if (event_count == 0) {
        popup_message("No events to modify!");
        return;
    }

    // 1. 현재 이벤트 리스트 출력
    clear();
    mvprintw(2, 10, "Modify Event:");
    mvprintw(4, 12, "Select an event to modify:");

    for (int i = 0; i < event_count; i++) {
        mvprintw(6 + i, 12, "%d. %s (Start: %02d-%02d | End: %02d-%02d)", 
                 i + 1, events[i].title,
                 events[i].date_start.month, events[i].date_start.day,
                 events[i].date_end.month, events[i].date_end.day);
    }

    // 2. 사용자 입력 처리
    char buffer[128] = {0};
    mvprintw(7 + event_count, 12, "Enter event number: ");
    if (get_input(buffer, sizeof(buffer)) == -1) {
        return; // 뒤로가기 처리
    }

    int choice = atoi(buffer);
    if (choice < 1 || choice > event_count) {
        popup_message("Invalid choice! Please try again.");
        return;
    }

    Event *event = &events[choice - 1];

    // 3. 입력 필드 정의
    char title[50] = {0}, details[100] = {0}, start_date[20] = {0}, start_time[10] = {0};
    char end_date[20] = {0}, end_time[10] = {0}, importance[5] = {0}, reminder[5] = {0};

    snprintf(title, sizeof(title), "%s", event->title);
    snprintf(details, sizeof(details), "%s", event->details);
    snprintf(start_date, sizeof(start_date), "%04d %02d %02d", 
             event->date_start.year, event->date_start.month, event->date_start.day);
    snprintf(start_time, sizeof(start_time), "%02d %02d", 
             event->date_start.hour, event->date_start.minute);
    snprintf(end_date, sizeof(end_date), "%04d %02d %02d", 
             event->date_end.year, event->date_end.month, event->date_end.day);
    snprintf(end_time, sizeof(end_time), "%02d %02d", 
             event->date_end.hour, event->date_end.minute);
    snprintf(importance, sizeof(importance), "%d", event->importance);
    snprintf(reminder, sizeof(reminder), "%d", event->reminder);

    InputField fields[] = {
        {"Modify title", title, sizeof(title), NULL},
        {"Modify start date (YYYY MM DD)", start_date, sizeof(start_date), validate_date_wrapper},
        {"Modify start time (HH MM)", start_time, sizeof(start_time), validate_time_wrapper},
        {"Modify end date (YYYY MM DD)", end_date, sizeof(end_date), validate_date_wrapper},
        {"Modify end time (HH MM)", end_time, sizeof(end_time), validate_time_wrapper},
        {"Modify importance (range: 0-5)", importance, sizeof(importance), validate_importance},
        {"Modify reminder (1: Yes, 0: No)", reminder, sizeof(reminder), validate_reminder},
        {"Modify details", details, sizeof(details), NULL}
    };

    UIScreen screen = {"Modify Event", fields, sizeof(fields) / sizeof(fields[0])};

    // 4. 입력 처리
    active_screen = &screen;
    current_step = 0;

    if (process_user_input(&screen) == 0) {
        // 5. 수정 데이터 반영
        strncpy(event->title, title, sizeof(event->title));
        sscanf(start_date, "%d %d %d", &event->date_start.year, &event->date_start.month, &event->date_start.day);
        sscanf(start_time, "%d %d", &event->date_start.hour, &event->date_start.minute);
        sscanf(end_date, "%d %d %d", &event->date_end.year, &event->date_end.month, &event->date_end.day);
        sscanf(end_time, "%d %d", &event->date_end.hour, &event->date_end.minute);
        event->importance = atoi(importance);
        event->reminder = atoi(reminder);
        strncpy(event->details, details, sizeof(event->details));

        popup_message("Event successfully modified!");
    } else {
        popup_message("Modification canceled.");
    }

    active_screen = NULL;
}


// 일정 삭제
void deleteEvents() {
    if (event_count == 0) {
        popup_message("No events to delete!");
        return;
    }

    // 1. 현재 이벤트 리스트를 필드로 생성
    char event_choices[MAX_EVENTS][100] = {0};
    for (int i = 0; i < event_count; i++) {
        snprintf(event_choices[i], sizeof(event_choices[i]), "%d. %s (Start: %04d-%02d-%02d %02d:%02d)", 
                 i + 1, events[i].title,
                 events[i].date_start.year, events[i].date_start.month, events[i].date_start.day,
                 events[i].date_start.hour, events[i].date_start.minute);
    }

    char choice_buffer[10] = {0};
    InputField fields[] = {
        {"Select an event to delete", choice_buffer, sizeof(choice_buffer), validate_event_choice}
    };

    UIScreen screen = {"Delete Event", fields, sizeof(fields) / sizeof(fields[0])};

    // 2. 현재 이벤트 리스트를 화면에 출력
    clear();
    mvprintw(2, 10, "Delete Event:");
    for (int i = 0; i < event_count; i++) {
        mvprintw(4 + i, 10, "%s", event_choices[i]);
    }
    mvprintw(5 + event_count, 10, "Enter the number of the event (or :b to go back):");
    refresh();

    // 3. 사용자 입력 처리
    active_screen = &screen;
    current_step = 0;

    if (process_user_input(&screen) == 0) {
        // 4. 유효한 입력 처리
        int choice = atoi(choice_buffer);
        if (choice >= 1 && choice <= event_count) {
            // 이벤트 삭제
            for (int i = choice - 1; i < event_count - 1; i++) {
                events[i] = events[i + 1];  // 배열 재정렬
            }
            event_count--;  // 이벤트 개수 감소
            popup_message("Event successfully deleted!");
        } else {
            popup_message("Invalid choice!");
        }
    } else {
        popup_message("Deletion canceled.");
    }

    active_screen = NULL;
}
