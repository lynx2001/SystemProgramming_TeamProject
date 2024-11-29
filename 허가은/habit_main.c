#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include "date_check.h"
#include "habit_function.h"
#define HABIT_FILE "habit.txt"

void handleDateChangeSignal(int signal) {
    if (signal == SIGUSR1) {
        signalresetHabits(); //시그널 발생시 수행해야할 함수(동작)
    }
}


int main() {
    char command[10], name[30];


    struct sigaction new_handler;

    new_handler.sa_sigaction = handleDateChangeSignal;//시그널발생시 처리 함수
    new_handler.sa_flags = 0; //일단 0으로 세팅
    sigemptyset(&new_handler.sa_mask); 
    
    if (sigaction(SIGUSR1, &new_handler, NULL) == -1) {
        perror("sigaction error");
        return 1;
    }
    ////////////////////////////
    loadHabits();
    initializeDateMonitor();
    startDateMonitor();

    printf("===습관 형성 프로그램===\n");
    
    printf("사용 방법:\n");
    printf(" - add [과제이름]: 새로운 습관 추가 \n");
    printf(" - done [과제이름]: 습관 완료 처리 \n");
    printf(" - delete [과제이름]: 습관 삭제 \n");
    printf(" - show : 오늘 해야 할 과제 목록 확인\n");
    printf(" - exit : 프로그램 종료\n");
    printf("---------------------\n\n");

    while (1) {
        fflush(stdin);
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
            saveHabits();
            printf("프로그램을 종료합니다.\n");
            break;
        } else {
            printf("잘못된 명령어입니다.\n");
        }
    }

    return 0;
}