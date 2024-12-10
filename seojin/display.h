#ifndef DISPLAY_H
#define DISPLAY_H

void draw_main_menu();       // 메뉴 출력
void draw_lists();           // 리마인더와 습관 목록 출력
void draw_event_screen();	 // 이벤트 서브메뉴 출력
void draw_habit_screen();	 // 습관 서브메뉴 출력

typedef struct {
    const char *prompt;          // 입력 메시지
    char *buffer;                // 사용자 입력 저장소
    int buffer_size;             // 입력 버퍼 크기
    _Bool (*validator)(const char *);  // 유효성 검사 함수 포인터
} InputField;

typedef struct {
    const char *title;       // 화면 제목
    InputField *fields;      // 입력 필드 배열
    int field_count;         // 필드 개수
} UIScreen;

void draw_ui_screen(const UIScreen *screen, int current_step);

#endif

