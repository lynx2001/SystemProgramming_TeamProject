#ifndef EVENT_H
#define EVENT_H

// event_submenu 함수 선언
void event_submenu();

// 입력 상태 관리용 구조체
typedef struct {
    char title[50];
    char details[100];
	struct {
        int year, month, day, hour, minute;
    } date_start, date_end;
    int reminder;
    int current_step;  // 현재 입력 단계 추적
} EventInputState;

// 전역 변수 선언
extern EventInputState *active_state;

#endif

