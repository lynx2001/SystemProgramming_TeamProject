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
        fprintf(file, "%d|%s|%d-%02d-%02d %02d:%02d|%d-%02d-%02d %02d:%02d|%d|%.2lf|%.2lf|%.2lf|%.2lf|%d|%s\n",
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
}

// 일정 추가
void addEvents() {
    if (event_count >= MAX_EVENTS) {
        popup_message("Event list is full. Cannot add more events.");
        return;
    }

    // curreny_y 초기화 (util.c에 정의된 함수)
    get_input("RESET", NULL, 0);

    clear();
    echo();

    /* 아래는 event 추가 관련 */
    // 초기화 된 이벤트 생성
    Event temp_event = {0};
    char buffer[128];
    int current_step = 0;

    mvprintw(2, 10, "Add Event:");
    while (current_step < 7) {
        switch (current_step) {
            memset(buffer, 0, sizeof(buffer)); // buffer 초기화
            case 0:  // 제목 입력
                if (get_input("Enter event title: ", temp_event.title, sizeof(temp_event.title)) == -1) return;
                // 제목이 빈 경우나 공백으로만 이루어진 경우 처리
                if (strlen(temp_event.title) == 0 || strspn(temp_event.title, " \t\r\n") == strlen(temp_event.title)) {
                    popup_message("Cannot be empty or whitespace only. Please try again.");
                } else {
                    current_step++;
                }
                break;

            case 1:  // 시작 날짜 입력
                if (get_input("Enter start date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d %d", &temp_event.date_start.year, &temp_event.date_start.month, &temp_event.date_start.day) == 3 && validateDate(temp_event.date_start.year, temp_event.date_start.month, temp_event.date_start.day)) {
                    current_step++;
                } else {
                    popup_message("Invalid start date. Please try again.");
                }
                break;

            case 2:  // 시작 시간 입력
                if (get_input("Enter start time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
                if (buffer[0] == '\0') {
                    temp_event.date_start.hour = 24;
                    temp_event.date_start.minute = 0;
                    current_step++;
                } else if (sscanf(buffer, "%d %d", &temp_event.date_start.hour, &temp_event.date_start.minute) == 2 && validateTime(temp_event.date_start.hour, temp_event.date_start.minute)) {
                    current_step++;
                } else {
                    popup_message("Invalid start time. Please try again.");
                }
                break;

            case 3:  // 종료 날짜 입력
                if (get_input("Enter end date (YYYY MM DD): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d %d %d", &temp_event.date_end.year, &temp_event.date_end.month, &temp_event.date_end.day) == 3 && validateDate(temp_event.date_end.year, temp_event.date_end.month, temp_event.date_end.day)) {
                    current_step++;
                } else {
                    popup_message("Invalid end date. Please try again.");
                }
                break;

            case 4:  // 종료 시간 입력
                if (get_input("Enter end time (HH MM): ", buffer, sizeof(buffer)) == -1) return;
                if (buffer[0] == '\0') {
                    temp_event.date_start.hour = 24;
                    temp_event.date_start.minute = 0;
                    current_step++;
                } else if (sscanf(buffer, "%d %d", &temp_event.date_end.hour, &temp_event.date_end.minute) == 2 && validateTime(temp_event.date_end.hour, temp_event.date_end.minute)) {
                    current_step++;
                } else {
                    popup_message("Invalid end time. Please try again.");
                }
                break;

            case 5:  // 리마인더 입력
                if (get_input("Set reminder (1: Yes, 0: No): ", buffer, sizeof(buffer)) == -1) return;
                if (sscanf(buffer, "%d", &temp_event.reminder) == 1 && (temp_event.reminder == 0 || temp_event.reminder == 1)) {
                    current_step++;
                } else {
                    popup_message("Invalid reminder. Please try again.");
                }
                break;
            case 6:  // 세부 사항 입력
                if (get_input("Enter event details: ", temp_event.details, sizeof(temp_event.details)) == -1) return;
                current_step++;
                break;

			default:
                popup_message("Invalid step. Please try again.");
				return;
		}
	}
    noecho();

    // Dday 계산
    Time current;
    initTime(&current);
    calDday(&temp_event, current);   // schedule.c 에서 정의된 함수

    // Event 저장
    Event new_event;
    new_event.id = ++last_event_id;
    strncpy(new_event.title, temp_event.title, sizeof(new_event.title));
    new_event.date_start = temp_event.date_start;
    new_event.date_end = temp_event.date_end;
    new_event.importance = -1;
    new_event.quantity = -1;
    new_event.interval = -1;
    new_event.Dday = temp_event.Dday;
    new_event.weight = -1;
    new_event.reminder = temp_event.reminder;
    strncpy(new_event.details, temp_event.details, sizeof(new_event.details));
    events[event_count++] = new_event;

    popup_message("Event successfully added!");  // 성공 메시지 출력
}

// 일정 수정
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

    if (choice < 1 || choice > event_count) {
        popup_message("Invalid event number!");
        return;
    }

    /* 아래는 event 수정 관련 */
    Event *original_event = &events[choice - 1];
    // 초기화 된 이벤트 생성
    Event temp_event = {0};
    char buffer[128];
    int current_step = 0;

    // 이벤트 수정 view 다시 로딩
    get_input("RESET", NULL, 0);    // current_y 초기화
    clear();

    mvprintw(2, 10, "Modify Event:");
    mvprintw(4, 12, "Editing event: %s", original_event->title);
    while (current_step < 7) {
        memset(buffer, 0, sizeof(buffer)); // buffer 초기화
        switch (current_step) {
            case 0:  // 제목 수정
                if (get_input("Enter new event title (leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0 && strspn(buffer, " \t\r\n") != strlen(buffer)) {
                    strncpy(temp_event.title, buffer, sizeof(temp_event.title));
                }
                current_step++;
                break;

            case 1:  // 시작 날짜 수정
                if (get_input("Enter new start date (YYYY MM DD, leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0 && sscanf(buffer, "%d %d %d", &temp_event.date_start.year, &temp_event.date_start.month, &temp_event.date_start.day) == 3 && validateDate(temp_event.date_start.year, temp_event.date_start.month, temp_event.date_start.day)) {
                    // 날짜가 유효한 경우에만 업데이트
                } else if (strlen(buffer) > 0) {
                    popup_message("Invalid start date. Please try again.");
                    continue;
                }
                current_step++;
                break;

            case 2:  // 시작 시간 수정
                if (get_input("Enter new start time (HH MM, leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0) {
                    if (sscanf(buffer, "%d %d", &temp_event.date_start.hour, &temp_event.date_start.minute) == 2 && validateTime(temp_event.date_start.hour, temp_event.date_start.minute)) {
                        // 시간 유효성 검사 통과
                    } else {
                        popup_message("Invalid start time. Please try again.");
                        continue;
                    }
                }
                current_step++;
                break;

            case 3:  // 종료 날짜 수정
                if (get_input("Enter new end date (YYYY MM DD, leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0 && sscanf(buffer, "%d %d %d", &temp_event.date_end.year, &temp_event.date_end.month, &temp_event.date_end.day) == 3 && validateDate(temp_event.date_end.year, temp_event.date_end.month, temp_event.date_end.day)) {
                    // 날짜가 유효한 경우에만 업데이트
                } else if (strlen(buffer) > 0) {
                    popup_message("Invalid end date. Please try again.");
                    continue;
                }
                current_step++;
                break;

            case 4:  // 종료 시간 수정
                if (get_input("Enter new end time (HH MM, leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0) {
                    if (sscanf(buffer, "%d %d", &temp_event.date_end.hour, &temp_event.date_end.minute) == 2 && validateTime(temp_event.date_end.hour, temp_event.date_end.minute)) {
                        // 시간 유효성 검사 통과
                    } else {
                        popup_message("Invalid end time. Please try again.");
                        continue;
                    }
                }
                current_step++;
                break;

            case 5:  // 리마인더 수정
                if (get_input("Set new reminder (1: Yes, 0: No, leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0) {
                    if (sscanf(buffer, "%d", &temp_event.reminder) == 1 && (temp_event.reminder == 0 || temp_event.reminder == 1)) {
                        // 리마인더 유효성 검사 통과
                    } else {
                        popup_message("Invalid reminder. Please try again.");
                        continue;
                    }
                }
                current_step++;
                break;

            case 6:  // 세부 사항 수정
                if (get_input("Enter new event details (leave blank to keep current): ", buffer, sizeof(buffer)) == -1) return;
                if (strlen(buffer) > 0) {
                    strncpy(temp_event.details, buffer, sizeof(temp_event.details));
                }
                current_step++;
                break;

            default:
                popup_message("Invalid step. Please try again.");
                return;
        }
    }
    noecho();

    // Dday 계산
    Time current;
    initTime(&current);
    calDday(&temp_event, current);   // schedule.c 에서 정의된 함수

    // 수정된 필드만 원래 이벤트에 복사
    if (strlen(temp_event.title) > 0) { // title
        strncpy(original_event->title, temp_event.title, sizeof(original_event->title));
    }
    if (temp_event.date_start.year != 0 || temp_event.date_start.month != 0 || temp_event.date_start.day != 0) {  // date_start
        original_event->date_start = temp_event.date_start;
    }
    if (temp_event.date_start.hour != 0 || temp_event.date_start.minute != 0) {  // start time
        original_event->date_start.hour = temp_event.date_start.hour;
        original_event->date_start.minute = temp_event.date_start.minute;
    }
    if (temp_event.date_end.year != 0 || temp_event.date_end.month != 0 || temp_event.date_end.day != 0) {  // date_end
        original_event->date_end = temp_event.date_end;
    }
    if (temp_event.date_end.hour != 0 || temp_event.date_end.minute != 0) {  // end time
        original_event->date_end.hour = temp_event.date_end.hour;
        original_event->date_end.minute = temp_event.date_end.minute;
    }
    if (temp_event.reminder != 0) {  // reminder
        original_event->reminder = temp_event.reminder;
    }
    if (strlen(temp_event.details) > 0) {  // details
        strncpy(original_event->details, temp_event.details, sizeof(original_event->details));
    }

    popup_message("Event successfully modified!");  // 성공 메시지 출력
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
        memset(&events[event_count], 0, sizeof(Event));  // 마지막 이벤트 초기화
        popup_message("Event successfully deleted!");  // 성공 메시지 출력
    } else {
        popup_message("Invalid choice!");  // 유효하지 않은 선택
    }

    noecho();
}
