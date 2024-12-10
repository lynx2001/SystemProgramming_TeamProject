#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "global.h"
#include "util.h"
#include "display.h"
#include "event.h"
#include "habit.h"
#include "calendar/calendar.h"
#include "calendar/calendar_control.h"

// popup_message 호출 여부를 기록하는 전역 변수
bool popup_message_called = false;

// 사용자 입력 줄의 Y 오프셋 (서브-서브 초기화 시 재설정 가능)
static int input_y_offset = 0;

// 초기화: 활성화된 화면 없음
UIScreen *active_screen = NULL;

// 초기화: 첫 번째 단계
int current_step = 0;

// 팝업 메시지 출력 함수
void popup_message(const char *message) {
    int height, width;
    getmaxyx(stdscr, height, width);  // 터미널 크기 가져오기
	
	popup_message_called = true;  // 팝업 실행 표시
	
	// 현재 화면 백업
    WINDOW *backup = dupwin(stdscr);  // 현재 화면 복사

    // 팝업 윈도우 생성
    WINDOW *popup = newwin(5, width / 2, height / 2 - 2, width / 4);
    box(popup, 0, 0);  // 테두리 그리기
    mvwprintw(popup, 2, (width / 2 - strlen(message)) / 2, "%s", message);  // 메시지 출력
    wrefresh(popup);  // 팝업 윈도우 갱신

    flushinp();       // 입력 버퍼 초기화
    wgetch(popup);    // 팝업 윈도우에서 사용자 입력 대기

    delwin(popup);    // 팝업 윈도우 삭제
	clear();

    // 이전 화면 복구
    overwrite(backup, stdscr);  // 백업한 내용을 현재 화면에 복사
    delwin(backup);             // 백업 윈도우 삭제
	refresh();                  // 화면 갱신
}

int popup_confirmation(const char *message) {
    int height, width;
    getmaxyx(stdscr, height, width);  // 터미널 크기 가져오기

    // 현재 화면 백업
    WINDOW *backup = dupwin(stdscr);

    // 팝업 윈도우 생성
    WINDOW *popup = newwin(5, width / 2, height / 2 - 2, width / 4);
    box(popup, 0, 0);  // 테두리 그리기
    mvwprintw(popup, 2, (width / 2 - strlen(message)) / 2, "%s", message);  // 메시지 출력
    wrefresh(popup);  // 팝업 윈도우 갱신

    flushinp();       // 입력 버퍼 초기화
    int choice;
    while (1) {
        choice = wgetch(popup);
        if (choice == 'y' || choice == 'n') {
            break;  // y 또는 n 입력 시 루프 종료
        }
    }

    delwin(popup);    // 팝업 윈도우 삭제
    clear();

    // 이전 화면 복구
    overwrite(backup, stdscr);  // 백업한 내용을 현재 화면에 복사
    delwin(backup);             // 백업 윈도우 삭제
    refresh();                  // 화면 갱신

    // 반환값을 1 (y) 또는 0 (n)으로 매핑
    return (choice == 'y') ? 1 : 0;
}

bool validateDate(int year, int month, int day) {
    // 기본 범위 검사
    if (year < 1900 || year > 2100 ||
        month < 1 || month > 12 ||
        day < 1 || day > 31) {
        return false;
    }

    // 월별 일수 검사
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // 윤년 처리
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    return day <= daysInMonth[month];
}

bool validateTime(int hour, int minute) {
    // 24:00은 특별한 경우로 허용
    if (hour == 24) {
        return minute == 0;
    }
    
    // 일반적인 시간 범위 검사
    return hour >= 0 && hour < 24 &&
           minute >= 0 && minute < 60;
}

bool validate_date_wrapper(const char *input) {
    int year, month, day;
    if (sscanf(input, "%d %d %d", &year, &month, &day) == 3) {
        return validateDate(year, month, day);
    }
    return false;
}

bool validate_time_wrapper(const char *input) {
    int hour, minute;
    if (sscanf(input, "%d %d", &hour, &minute) != 2) {
        return false;  // 입력 형식이 잘못된 경우
    }
    return validateTime(hour, minute);
}

bool validate_reminder(const char *input) {
    return strcmp(input, "0") == 0 || strcmp(input, "1") == 0;
}

bool validate_importance(const char *input) {
    int value = atoi(input);  // 문자열을 정수로 변환
    return value >= 0 && value <= 5;  // 0~5 사이 값만 유효
}

bool validate_quantity(const char *input) {
    char *endptr;
    strtol(input, &endptr, 10);
    return *endptr == '\0'; // 입력이 정수형인지 확인
}

// 서브-서브 화면 진입 시 호출
void reset_input_position() {
    input_y_offset = 0;  // 입력 위치 초기화
}

//화면 크기 조절 감지 핸들러
void handle_resize(int sig) {
    if (current_screen == DEFAULT_SCREEN) {
		return;
	}

	endwin();
    refresh();
    clear();

	flushinp();	// 입력 버퍼 비우기

	// 터미널 크기 갱신
	int height, width;
	getmaxyx(stdscr, height, width);

	(void) height;
	(void) width;
	
    if (current_screen == MAIN_SCREEN) {
        draw_title();
        draw_main_menu();
        draw_lists();
	} else if (active_screen != NULL) {
		draw_ui_screen(active_screen, current_step);
		int y = 5 + current_step * 3;  // 프롬프트 아래 줄
        int x = 14 +  strlen(active_screen->fields[current_step].buffer);	// ">" 기호 뒤 입력 시작 위치
        move(y, x);                    // 커서 복원
    } else if (current_screen == EVENT_SCREEN) {
    	draw_event_screen();
	} else if (current_screen == HABIT_SCREEN) {
        draw_habit_screen();
    } else if (current_screen == CALENDAR_SCREEN) {
        color = ((color + 2) % 3);
        prev_first_color = cur_first_color;
        cur_first_color = color;
        show_calendar();
    }

	refresh();
}

int get_input(char *buffer, int size) {
	// 사용자 입력
    echo();
    mvgetnstr(getcury(stdscr), getcurx(stdscr), buffer, size);  // 현재 커서 위치에서 입력
	noecho();

    // 뒤로가기 (:b) 입력 감지
    if (strcmp(buffer, ":b") == 0) {
        return -1; // 뒤로가기 신호 반환
    }

    return 1; // 정상 입력 반환
}

// 공통 입력 처리 함수
int process_user_input(UIScreen *screen) {
    active_screen = screen;
	int current_step = 0;

    while (current_step < screen->field_count) {
        draw_ui_screen(screen, current_step);

        char buffer[128];

		// 커서를 현재 프롬프트 아래로 이동
        int y = 5 + current_step * 3;  // 프롬프트 아래 줄 계산
        int x = 14 + strlen(screen->fields[current_step].buffer);	// ">" 이후의 입력 시작 위치
        move(y, x);                    // 커서 이동
		refresh();

        if (get_input(buffer, sizeof(buffer)) == -1) {
            active_screen = NULL;
			return -1;  // 뒤로가기 처리
        }

		// 입력값이 비어 있으면 현재 필드의 값을 유지하고 넘어감
        if (strlen(buffer) == 0) {
            current_step++;
            continue;
        }

        // 유효성 검사
        if (screen->fields[current_step].validator &&
            !screen->fields[current_step].validator(buffer)) {
            popup_message("Invalid input. Please try again.");
            continue;  // 재입력 요청
        }

        // 유효한 입력을 필드에 저장
        strncpy(screen->fields[current_step].buffer, buffer, screen->fields[current_step].buffer_size);
        current_step++;
    }

	active_screen = NULL;
    return 0;  // 모든 입력 완료
}

//Sort By Weight
int compareByWeight(const void* a, const void* b) {
    const Event* todoA = (const Event*)a;
    const Event* todoB = (const Event*)b;

    if (todoA->weight < todoB->weight) return 1;
    if (todoA->weight > todoB->weight) return -1;
    return 0;
}

//QuickSort
void sortTodo(Event* event_t, int count) {
    qsort(event_t, count, sizeof(Event), compareByWeight);
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
