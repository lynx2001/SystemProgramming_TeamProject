#include <ncurses.h>
#include <signal.h>
#include "calendar.h"
#include "display.h"
#include "event.h"
#include "habit.h"
#include "scheduler.h"
#include "global.h"
#include <stdbool.h>

bool is_in_submenu = false;

// 현재 활성화된 서브화면을 추적하는 변수
int current_submenu = 0;

// 터미널 크기 변경 시 호출되는 핸들러
void handle_resize(int sig) {
    endwin();       // ncurses 종료 후 다시 초기화
    refresh();
    clear();

    if (is_in_submenu) {
        // 서브화면 상태라면 서브화면만 갱신
        if (current_submenu == EVENT_SUBMENU) {
            event_submenu();
        } else if (current_submenu == HABIT_SUBMENU) {
            habit_submenu();
        } else if (current_submenu == SCHEDULER_SUBMENU) {
            scheduler_submenu();
        }
    } else {
        // 메인 화면 갱신
        draw_calendar_screen();
        draw_main_menu();
        draw_lists();
    }
}

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
                event_submenu();
                draw_calendar_screen(); // 초기 화면으로 복귀
                draw_main_menu();
                draw_lists();
                break;
            case '3': // 습관 관리
                habit_submenu();
                draw_calendar_screen(); // 초기 화면으로 복귀
                draw_main_menu();
                draw_lists();
                break;
            case '4': // 일정 스케줄러
                scheduler_submenu();
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
