#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_EVENTS 100
#define EVENT_FILE "event.txt"
#define ID_FILE "last_id.txt"

// 화면 상수
#define MAIN_SCREEN 0
#define EVENT_SCREEN 1
#define HABIT_SCREEN 2

typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Time;

typedef struct {
    int id;
    char title[50];
    Time date_start;
    Time date_end;
    int interval;    // 마감일 - 시작일
    double Dday;        // 마감일 - 현재일(now)
    double quantity;    // 입력된 값
    double weight;      // 가중치 식에 따라 계산 (지역변수 importance를 사용해서 계산) >> default : -1
    char details[100];

    /*
    가중치 일정 입력 시 고려할 사항,
    시프과제 3분할 -> 시프과제 1, 시프과제 2, 시프과제 3
    예를 들어서 interval 3 -> 3분할 / 분량 90 -> 30 * 3아니고 그냥 90 3번
    X    id - 3개로 쌓여야하고
    X    title - 에 숫자 달아주기 (해도되고 안해도 되고)
    O    Time 2개는 그대로(똑같아도 됨)
    O    interval도 그대로
    O    Dday도 그대로 (현재 시간을 확인하는 과정 필요 (HB))
    O    가중치 - 3개 별개
    O    분량 - 그대로
    O    detail은 없는걸로

    추가로 진행도가 title에 숫자 다는걸로 대체되어도 상관없음
    */
} Event;

extern Event events[MAX_EVENTS];
extern int event_count;
extern int last_id;

extern int current_screen;

#endif
