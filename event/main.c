#include <stdio.h>
#include "global.h"
#include "event.h"
#include "util.h"

int showmenu() {
    int choice;
    char buffer[100]; // 입력을 받을 버퍼

    printf("\n메뉴:\n");
    printf("1. 일정 추가\n");
    printf("2. 일정 삭제\n");
    printf("3. 일정 수정\n");
    printf("4. 일정 출력\n");
    printf("5. 스케줄링 일정 추가\n");
    printf("0. 종료\n");
    printf("선택: ");

    while (1) {
        // 문자열 입력 받기
        fgets(buffer, sizeof(buffer), stdin);

        // 문자열을 정수로 변환
        if (sscanf(buffer, "%d", &choice) != 1) {
            printf("잘못된 입력입니다. 숫자를 입력하세요.\n");
            continue;
        }

        // 0~4 범위 확인
        if (choice >= 0 && choice <= 5) {
            break; // 유효한 입력
        } else {
            printf("잘못된 선택입니다. 올바른 숫자를 입력하세요.\n");
        }
    }

    printf("\n");
    return choice;
}

int main() {
    check_and_create_file(EVENT_FILE);
    check_and_create_file(ID_FILE);
    loadEvent();
    last_id = get_last_id();

    int choice;
    while ((choice = showmenu()) != 0) {
        switch (choice) {
            case 1:
                addEvent();
                break;
            case 2:
                //showEvent();
                deleteEvent();
                break;
            case 3:
                //showEvent();
                editEvent();
                break;
            case 4:
                showEvent();
                break;
            case 5:
                addScheduledEvent();
                break;
            default:
                printf("Invalid choice\n");
        }
    }

    // 정보 기록
    saveEvent();
    save_id(last_id);

    return 0;
}
