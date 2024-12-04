#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include "global.h"
#include "util.h"
#include "display.h"
#include "calendar.h"
#include "event.h"
#include "habit.h"

// popup_message 호출 여부를 기록하는 전역 변수
bool popup_message_called = false;

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

//화면 크기 조절 감지 핸들러
void handle_resize(int sig) {
    endwin();
    refresh();
    clear();

	// 터미널 크기 갱신
	int height, width;
	getmaxyx(stdscr, height, width);

	(void)height;
	(void) width;
	
    if (current_screen == MAIN_SCREEN) {
        draw_calendar_screen();
        draw_main_menu();
        draw_lists();
	} else if (current_screen == EVENT_SCREEN) {
		event_submenu();
    } else if (current_screen == HABIT_SCREEN) {
        habit_submenu();
    }

//:	refresh();
}

/**
 * get_input: 입력을 받고 뒤로가기 (:b)을 감지
 * @param prompt: 입력 전 출력할 메시지
 * @param buffer: 입력값을 저장할 버퍼
 * @param size: 버퍼 크기
 * @return 1: 정상 입력
 *        -1: 뒤로가기 (:b 입력)
 */

int get_input(const char *prompt, char *buffer, int size) {
    static int current_y = 5;

    mvprintw(LINES - 1, 0, ":b return to previous page");
    
    if (popup_message_called) {
        move(current_y + 1, 0);  // 현재 줄 삭제
        clrtoeol();
        refresh();
		popup_message_called = false;
	} else {
		current_y += 2;
	}
		
	mvprintw(current_y, 10, "%s", prompt);
    clrtoeol();
    refresh();

    // 사용자 입력
    echo();
    mvgetnstr(current_y + 1, 10, buffer, size);
    noecho();

    if (strcmp(buffer, ":b") == 0) {
        return -1;
    }

    return 1;
}
