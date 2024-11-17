#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "event.h"

void displayReminders() {
    printf("=== 미리 알림 ===\n");
    
    time_t now;
    struct tm* tm_now;
    char today[20];

	

    time(&now);  
    tm_now = localtime(&now);
    strftime(today, 20, "%Y-%m-%d", tm_now);  // yyyy-mm-dd 형식
    
    // 
     for (int i = 0; i < event_count; i++) {
        
        struct tm event_date_tm;
        time_t event_time;
        char event_date[20];

        // 이벤트 종료 날짜 변환
        if (strptime(events[i].date_end, "%Y-%m-%d", &event_date_tm) == NULL) {
            printf("error : error in converting eventdate\n");
            continue;
        }
        event_time = mktime(&event_date_tm);

        // 오늘로부터 이벤트 종료일까지의 남은 날 계산
        double diff_days = difftime(event_time, now) / (60 * 60 * 24);  
        

        // 알림 날짜 vs 오늘 날짜 비교
        if (diff_days <= 3 && diff_days > 0) {
            printf("일정: %s\n", events[i].name);
            printf("마감까지: %.0f일\n", diff_days);
            //printf("세부사항: %s\n", events[i].details);
            printf("===================\n");
        }
    }

}
