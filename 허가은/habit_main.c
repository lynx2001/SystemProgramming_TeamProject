#include <stdio.h>
#include <string.h>

#include "habit_function.h"


int main() {
    char command[10], name[30];

    // 프로그램 시작 시 파일에서 로드
    loadHabits();

    pthread_create(&date_check_thread, NULL, dateCheckThread, NULL);

    printf("===습관 형성 프로그램===\n");
    
    printf("사용 방법:\n");
    printf(" - add [과제이름]: 새로운 습관 추가 \n");
    printf(" - done [과제이름]: 습관 완료 처리 \n");
    printf(" - delete [과제이름]: 습관 삭제 \n");
    printf(" - show : 오늘 해야 할 과제 목록 확인\n");
    printf(" - exit : 프로그램 종료\n");
    printf("---------------------\n\n");

    while (1) {
        // 날짜 변경 확인 및 초기화
        resetHabitsIfDateChanged();

        printf("\n> ");
        scanf("%s", command);

        if (strcmp(command, "add") == 0) {
            scanf("%s", name);
            addHabit(name);
        } else if (strcmp(command, "done") == 0) {
            scanf("%s", name);
            markHabitDone(name);
        } else if (strcmp(command, "delete") == 0) {

            scanf("%s", name);
            deleteHabit(name);
        } else if (strcmp(command, "show") == 0) {
            showHabits();
        }  else if (strcmp(command, "exit") == 0) {
            // 스레드 종료 및 저장
            program_running = 0;  // 스레드 종료 신호
            pthread_join(date_check_thread, NULL);  // 스레드 종료 대기
            saveHabits();
            printf("프로그램을 종료합니다.\n");
            break;
        } else {
            printf("잘못된 명령어입니다.\n");
        }
    }

    return 0;
}