#include <ncurses.h>
#include <signal.h>
#include "global.h"
#include "util.h"
#include "display.h"
#include "event.h"
#include "habit.h"
#include "scheduler.h"
//gaeun
#include "date_check.h"
#include "./calendar/calendar.h"


// 현재 활성화된 화면을 추적하는 변수
int current_screen = MAIN_SCREEN;

//gaeun
void handleDateChangeSignal(int signal, siginfo_t *info, void *context) {
    if (signal == SIGUSR1) {
        
        signalresetHabits();
        draw_title();
        draw_main_menu();
        draw_lists();       
    }
}


int main() {
    //gaeun
    loadHabits();
    
    struct sigaction new_handler;

    new_handler.sa_sigaction = handleDateChangeSignal;//시그널발생시 처리 함수
    new_handler.sa_flags = 0; //일단 0으로 세팅
    sigemptyset(&new_handler.sa_mask); 
    
    if (sigaction(SIGUSR1, &new_handler, NULL) == -1) {
        perror("sigaction error");
        return 1;
    }
    initializeDateMonitor();
    startDateMonitor();

//
    initscr();         // ncurses 초기화
    noecho();          // 사용자 입력 숨기기
    curs_set(FALSE);   // 커서 숨기기

    signal(SIGWINCH, handle_resize); // 터미널 크기 변경 감지

    draw_title();           // 타이틀 출력
    draw_main_menu();       // 초기 메뉴 출력
    draw_lists();           // 초기 리마인더 및 습관 목록 출력

    while (1) {
        int choice = getch(); // 사용자 입력 대기

        switch (choice) {
            case '1': // 캘린더 출력
				current_screen = CALENDAR_SCREEN; // 캘린더 화면과 메인화면 창 크기 변경 시 실행 구분 위함
                draw_calendar();        // 캘린더 출력
                current_screen = MAIN_SCREEN; // 초기 화면으로 복귀
                clear();

                draw_title();           // 타이틀 출력
                draw_main_menu();       // 메뉴 유지
                draw_lists();           // 리스트 갱신
				break;
            case '2': // 이벤트 관리
				current_screen = EVENT_SCREEN;
                event_submenu();
                current_screen = MAIN_SCREEN; // 초기 화면으로 복귀
                clear();
                				
                draw_title();          
                draw_main_menu();
                draw_lists();
				break;
            case '3': // 습관 관리
				current_screen = HABIT_SCREEN;
                habit_submenu();
				current_screen = MAIN_SCREEN; // 초기 화면으로 복귀
                clear();
                				
                draw_title();          
                draw_main_menu();
                draw_lists();
                break;
            case 'q': // 프로그램 종료
                //gaeun
                saveHabits();
                stopDateMonitor();
                endwin();
                return 0;
            default:
                break;
        }
    }

    endwin(); // ncurses 종료
    return 0;
}
