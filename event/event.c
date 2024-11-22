// event.c
#include "global.h"
#include "event.h"
#include "util.h"
#include "event_io.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Event 배열 채우기
void loadEvent() {
    FILE *file = fopen(EVENT_FILE, "r");
    if (file == NULL) {
        printf("파일 열기 오류\n");
        return;
    }

    char buffer[512];
    event_count = 0;

    while (fgets(buffer, sizeof(buffer), file) && event_count < MAX_EVENTS) {
        Event *event = &events[event_count];
        sscanf(buffer,
               "%d|%49[^|]|%d-%d-%d %d:%d|%d-%d-%d %d:%d|%d|%lf|%lf|%lf|%99[^\n]",
               &event->id, event->title,
               &event->date_start.year, &event->date_start.month, &event->date_start.day,
               &event->date_start.hour, &event->date_start.minute,
               &event->date_end.year, &event->date_end.month, &event->date_end.day,
               &event->date_end.hour, &event->date_end.minute,
               &event->interval, &event->Dday, &event->weight, &event->quantity,
               event->details);

        event_count++;
    }

    fclose(file);
    printf("%d개의 이벤트가 로드되었습니다.\n", event_count);
}


// Event 파일에 저장
void saveEvent() {
    FILE *file = fopen(EVENT_FILE, "w");
    if (file == NULL) {
        printf("파일 열기 오류\n");
        return;
    }

    for (int i = 0; i < event_count; i++) {
        Event *event = &events[i];
        fprintf(file, "%d|%s|%d-%02d-%02d %02d:%02d|%d-%02d-%02d %02d:%02d|%.2d|%.2lf|%.2lf|%.2lf|%s\n",
                event->id, event->title,
                event->date_start.year, event->date_start.month, event->date_start.day,
                event->date_start.hour, event->date_start.minute,
                event->date_end.year, event->date_end.month, event->date_end.day,
                event->date_end.hour, event->date_end.minute,
                event->interval, event->Dday, event->weight, event->quantity,
                event->details);
    }

    fclose(file);
    printf("일정 저장이 완료되었습니다.\n");
}


// 일정 입력 받기
Event getEventInput(int is_scheduled) {
    // is_scheduled: 0 -> 일반 일정, 1 -> auto scheduling 일정
    Event event;
    event.id = last_id + 1;
    char date_input[20];    // 잘못된 입력 방지를 위해 크게 설정
    char time_input[10];

    // 일정 제목 입력
    getStringInput("일정 제목: ", event.title, sizeof(event.title), 0); // 공백 불가

    // interval 계산을 위한 넓은 while문
    while (1) {
        // 시작 날짜 입력
        while (1) {
            printf("시작 날짜 (YYYY-MM-DD): ");
            fgets(date_input, sizeof(date_input), stdin);
            date_input[strcspn(date_input, "\n")] = 0; // 개행 문자 제거

            if (sscanf(date_input, "%d-%d-%d",
                    &event.date_start.year, &event.date_start.month, &event.date_start.day) == 3 &&
                validateDate(event.date_start.year, event.date_start.month, event.date_start.day)) {
                break; // 유효한 날짜이면 반복 종료
            }

            printf("잘못된 날짜 형식입니다. 다시 입력하세요.\n");
        }

        // 종료 시간 입력
        while (1) {
            printf("시작 시간 (HH:MM, 빈 입력시 24:00으로 설정): ");
            fgets(time_input, sizeof(time_input), stdin);
            time_input[strcspn(time_input, "\n")] = 0; // 개행 문자 제거

            // 빈 입력 체크
            if (strlen(time_input) == 0) {
                event.date_start.hour = 24;
                event.date_start.minute = 0;
                break;
            }

            // 올바른 시간 형식
            if (sscanf(time_input, "%d:%d",
                    &event.date_start.hour, &event.date_start.minute) == 2 &&
                validateTime(event.date_start.hour, event.date_start.minute)) {
                break;
            }
            printf("잘못된 시간 형식입니다. 다시 입력하세요 (HH:MM).\n");
        }

        // 종료 날짜 입력
        while (1) {
            printf("종료 날짜 (YYYY-MM-DD): ");
            fgets(date_input, sizeof(date_input), stdin);
            date_input[strcspn(date_input, "\n")] = 0; // 개행 문자 제거

            if (sscanf(date_input, "%d-%d-%d",
                    &event.date_end.year, &event.date_end.month, &event.date_end.day) == 3 &&
                validateDate(event.date_end.year, event.date_end.month, event.date_end.day)) {
                break; // 유효한 날짜이면 반복 종료
            }

            printf("잘못된 날짜 형식입니다. 다시 입력하세요.\n");
        }

        // 종료 시간 입력
        while (1) {
            printf("종료 시간 (HH:MM, 빈 입력시 24:00으로 설정): ");
            fgets(time_input, sizeof(time_input), stdin);
            time_input[strcspn(time_input, "\n")] = 0; // 개행 문자 제거

            // 빈 입력 체크
            if (strlen(time_input) == 0) {
                event.date_end.hour = 24;
                event.date_end.minute = 0;
                break;
            }

            // 올바른 시간 형식
            if (sscanf(time_input, "%d:%d",
                    &event.date_end.hour, &event.date_end.minute) == 2 &&
                validateTime(event.date_end.hour, event.date_end.minute)) {
                break;
            }
            printf("잘못된 시간 형식입니다. 다시 입력하세요 (HH:MM).\n");
        }

        // interval 계산 및 유효성 확인
        event.interval = calculateInterval(event.date_start, event.date_end);
        if (event.interval >= 0)
            break;

        printf("\n종료 날짜가 시작 날짜보다 빠릅니다. 다시 입력하세요.\n\n");
    }

    // 남은 기간 계산
    event.Dday = calculateDday(event.date_end);

    // 분량 입력
    event.quantity = getDoubleInput("분량 (실수): ");

    // 일정 유형에 따른 가중치 설정
    if (is_scheduled) {
        // auto scheduling 일정은 가중치 계산 필요
        event.weight = getDoubleInput("가중치 (0 ~ 5): ");
    }
    else {
        // 일반 일정은 가중치 -1
        event.weight = -1;
    }

    getStringInput("세부 내용 (선택 사항): ", event.details, sizeof(event.details), 1); // 공백 허용

    return event;
}

void addScheduledEvent() {
    // 가중치가 있는 event 입력을 통해
    // 가중치, 분량 계산 후 일정 배열에 추가
    printf("스케줄링 일정 추가입니다\n");
    Event event = getEventInput(1);
    int cnt = event.interval + 1;
    int importance = event.weight;

    for (int i = 0; i < cnt; i++) {
        Event temp_event = event;
        // 변경되는 값 : id, weight, quantity
        // 나머지 값은 그대로 유지 : title, date_start, date_end, interval, Dday, details
        // date_start, date_end을 그대로 유지해서, 아마도 interval 계산에 문제가 생기지는 않을거 같습니다

        // id 증가 (총 개수만큼)
        temp_event.id = event.id + i;   // event에서 id가 +1 되었기에, 첫 id는 0을 더해줘도 됨
        last_id++;

        // 가중치 계산 (CalculateScheduling > calWeight 함수 참고))
        const double a = 100.0;
        const double b = 1.0;

        if (event.Dday == 0)    // 남은 기간이 0일 경우
            temp_event.weight = importance;
        else
            temp_event.weight = a * (1.0 / temp_event.Dday) + b * importance;

        temp_event.quantity = round((event.quantity / (double)cnt * (i + 1)) * 100) / 100.0;    // 소수점 둘째자리까지 저장

        events[event_count] = temp_event;
        event_count++;
    }
}

// 일정 배열에 추가
void addEvent() {
    // event 입력 받기
    Event event = getEventInput(0);
    events[event_count] = event;
    event_count++;  // 이벤트 카운트 증가
    last_id++;      // last_id 증가
    
    printf("일정이 추가되었습니다.\n");
}

// 일정 배열에서 삭제
void deleteEvent() {
    // event 확인
    if (event_count == 0)
        return;

    int target_id;
    target_id = getIntInput("수정할 일정의 번호를 입력해주세요: ");
    // target_id 변환 (사용자 선택 -> 저장된 id)
    target_id = events[target_id - 1].id;

    int found = 0;
    for (int i = 0; i < event_count; i++) {
        if (events[i].id == target_id) {
            found = 1;
            // 이벤트 삭제: 뒤의 이벤트를 앞으로 이동
            for (int j = i; j < event_count - 1; j++) {
                events[j] = events[j + 1];
            }
            (event_count)--; // 이벤트 수 감소
            break; // 삭제 후 루프 종료
        }
    }

    if (found) {
        printf("ID %d에 해당하는 이벤트가 삭제되었습니다.\n", target_id);
    } else {
        printf("ID %d에 해당하는 이벤트를 찾을 수 없습니다.\n", target_id);
    }
}

// 일정 배열에서 수정
void editEvent() {
    // event 확인
    if (event_count == 0)
        return;

    int target_id;
    target_id = getIntInput("삭제할 일정의 번호를 입력해주세요: ");
    // target_id 변환 (사용자 선택 -> 저장된 id)
    target_id = events[target_id - 1].id;

    int found = 0;
    for (int i = 0; i < event_count; i++) {
        if (events[i].id == target_id) {
            printf("새로운 일정을 입력해주세요.\n");
            Event updated_event = getEventInput(0);
            events[i] = updated_event; // 이벤트 수정
            last_id++;      // last_id 증가
            found = 1;      // 탐색 성공 여부
            break; // 수정 후 루프 종료
        }
    }

    if (found) {
        printf("ID %d에 해당하는 이벤트가 수정되었습니다.\n", target_id);
    } else {
        printf("ID %d에 해당하는 이벤트를 찾을 수 없습니다.\n", target_id);
    }
}

// 기간 계산
int calculateInterval(Time start, Time end) {
    const int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = 0;
    
    // 시작일부터 하루씩 더하면서 계산
    int year = start.year;
    int month = start.month;
    int day = start.day;
    
    while (1) {
        if (year == end.year && month == end.month && day == end.day)
            break;
            
        days++;
        day++;
        
        // 월의 마지막 날짜 체크
        int maxDays = daysInMonth[month - 1];
        // 2월이고 윤년이면 29일
        if (month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0))
            maxDays = 29;
            
        if (day > maxDays) {
            day = 1;
            month++;
            if (month > 12) {
                month = 1;
                year++;
            }
        }
    }
    
    return days;
}

// 남은 기간 계산
double calculateDday(Time end) {
    time_t now = time(NULL);
    struct tm *today = localtime(&now);
    
    // 현재 시간을 당일 00:00:00으로 설정
    today->tm_hour = 0;
    today->tm_min = 0;
    today->tm_sec = 0;
    
    // 목표 날짜 설정 (시간은 00:00:00)
    struct tm tm_end = { 
        .tm_year = end.year - 1900,
        .tm_mon = end.month - 1,
        .tm_mday = end.day,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0
    };
    
    return (double)(difftime(mktime(&tm_end), mktime(today)) / (60 * 60 * 24));
}

// 이벤트 출력 함수
void showEvent() {
    if (event_count == 0) {
        printf("저장된 이벤트가 없습니다.\n");
        return;
    }

    printf("현재 저장된 이벤트 목록:\n");
    for (int i = 0; i < event_count; i++) {
        Event *event = &events[i];
        printf("----------------------\n");
        printf("목록 번호: %d\n", i + 1);
        printf("ID: %d\n", event->id);
        printf("제목: %s\n", event->title);
        printf("시작일: %d-%02d-%02d %02d:%02d\n",
               event->date_start.year, event->date_start.month, event->date_start.day,
               event->date_start.hour, event->date_start.minute);
        printf("종료일: %d-%02d-%02d %02d:%02d\n",
               event->date_end.year, event->date_end.month, event->date_end.day,
               event->date_end.hour, event->date_end.minute);
        printf("기간(일): %.2d\n", event->interval);
        printf("남은 기간(D-day): %.2lf\n", event->Dday);
        printf("우선순위: %.2lf\n", event->weight);
        printf("분량: %.2lf\n", event->quantity);
        printf("세부사항: %s\n", event->details);
    }
}


void showScheduledEvent() {
    if (event_count == 0) {
        printf("저장된 이벤트가 없습니다.\n");
        return;
    }

    printf("현재 저장된 Auto Scheduling 이벤트 목록:\n");
    for (int i = 0; i < event_count; i++) {
        Event *event = &events[i];
        if (event->weight != -1) { // 가중치가 -1이 아닌 경우만 출력
            printf("----------------------\n");
            printf("목록 번호: %d\n", i + 1);
            printf("ID: %d\n", event->id);
            printf("제목: %s\n", event->title);
            printf("시작일: %d-%02d-%02d %02d:%02d\n",
                event->date_start.year, event->date_start.month, event->date_start.day,
                event->date_start.hour, event->date_start.minute);
            printf("종료일: %d-%02d-%02d %02d:%02d\n",
                event->date_end.year, event->date_end.month, event->date_end.day,
                event->date_end.hour, event->date_end.minute);
            printf("기간(일): %.2d\n", event->interval);
            printf("남은 기간(D-day): %.2lf\n", event->Dday);
            printf("우선순위: %.2lf\n", event->weight);
            printf("분량: %.2lf\n", event->quantity);
            printf("세부사항: %s\n", event->details);
        }
    }
}