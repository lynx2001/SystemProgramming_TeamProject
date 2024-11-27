#include "date_check.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static pthread_t date_monitor_thread;
static int running = 1; 
static date_change_callback_t callback = NULL; 

// 현재 날짜 가져오기
static void getCurrentDate(char *date) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(date, 11, "%Y-%m-%d", tm_info);
}

// 날짜 변경 감지 스레드
static void *dateMonitorThread(void *arg) {
    char last_checked_date[11] = {0};
    getCurrentDate(last_checked_date);

    while (running) {
        sleep(10);

        char current_date[11];
        getCurrentDate(current_date);

        if (strcmp(last_checked_date, current_date) != 0) {
            strcpy(last_checked_date, current_date);
            if (callback) {
                callback(); // 콜백 함수 호출
            }
        }
    }

    return NULL;
}

// 초기화 함수
void initializeDateMonitor(date_change_callback_t cb) {
    callback = cb;
}

// 스레드 시작
void startDateMonitor() {
    running = 1;
    pthread_create(&date_monitor_thread, NULL, dateMonitorThread, NULL);
}

// 스레드 종료
void stopDateMonitor() {
    running = 0;
    pthread_join(date_monitor_thread, NULL);
}
