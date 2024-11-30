#include <ncurses.h>
#include <signal.h>
#include "global.h"
#include "util.h"
#include "display.h"
#include "calendar.h"
#include "event.h"
#include "habit.h"
#include "scheduler.h"

// 현재 활성화된 화면을 추적하는 변수
int current_screen = MAIN_SCREEN;

int main() {
    initscr();         // ncurses 초기화
    noecho();          // 사용자 입력 숨기기
    curs_set(FALSE);   // 커서 숨기기

    signal(SIGWINCH, handle_resize); // 터미널 크기 변경 감지

    init_calendar();   // 캘린더 초기화
    draw_calendar_screen(); // 초기 캘린더 텍스트 출력
    draw_main_menu();       // 초기 메뉴 출력
    draw_lists();           // 초기 리마인더 및 습관 목록 출력

    while (1) {
        int choice = getch(); // 사용자 입력 대기

        switch (choice) {
            case '1': // 달력 형식 변경
                change_calendar_format();
                draw_calendar_screen(); // 달력 텍스트 갱신
                draw_main_menu();       // 메뉴 유지
                draw_lists();           // 리스트 갱신
				break;
            case '2': // 이벤트 관리
				current_screen = EVENT_SCREEN;
                event_submenu();

                current_screen = MAIN_SCREEN;
				draw_calendar_screen(); // 초기 화면으로 복귀
                draw_main_menu();
                draw_lists();
				break;
            case '3': // 습관 관리
				current_screen = HABIT_SCREEN;
                habit_submenu();
				current_screen = MAIN_SCREEN;
                draw_calendar_screen(); // 초기 화면으로 복귀
                draw_main_menu();
                draw_lists();
                break;
            case 'q': // 프로그램 종료
                endwin();
                return 0;
            default:
                break;
        }
    }

    endwin(); // ncurses 종료
    return 0;
}
