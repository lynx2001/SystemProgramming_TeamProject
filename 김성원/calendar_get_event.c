#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "calendar.h"
#include "calendar_get_event.h"

typedef struct info{
    char event_name[50];
	int start_week;
	int start_wday;
    int end_week;
    int end_wday;
	int start_year;
	int start_month;
	int start_day;
    char start_time[10];
	int end_year;
	int end_month;
	int end_day;
    char memo[100];
}info;

// 각 월의 일수를 저장한 배열 (윤년이 아닌 경우)
int days_in_month[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 윤년인지 확인하는 함수
int is_leap_year(int year) {
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
        return 1;
    }
    return 0;
}

// 연도와 월을 입력받아 해당 월의 첫 번째 요일을 계산하는 함수
// 0: 일요일, 1: 월요일, ..., 6: 토요일
// Zeller's Congruence를 사용하여 요일 계산
int get_start_day_of_month(int year, int month) {
    int day = 1; // 기준일
    int y = year, m = month;

    if (month < 3) {
        m += 12;
        y -= 1;
    }

	int k = year % 100; // 연도 뒷 2자리
	int j = year / 100; // 연도 앞 2자리
	
	// Zellar's Congruenc 공식 사용
    int h = (day + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) - (j * 2)) % 7;

	// 계산된 값을 0-6 범위로 조정(일~토)
	if (h == 0)
		return 6;
	else
		return h - 1;
}

void get_line(char *line, struct info* e, int type) {
    char *token = strtok(line, "|");
    token = strtok(NULL, "|"); // 일정 이름
    strcpy(e->event_name, token);

    token = strtok(NULL, "|"); // 시작 날짜
    sscanf(token, "%4d-%2d-%2d", &e->start_year, &e->start_month, &e->start_day);

    token = strtok(NULL, "|"); // 종료 날짜
    if (type) {
        sscanf(token, "%4d-%2d-%2d", &e->end_year, &e->end_month, &e->end_day);
    } else {
        e->end_year = e->end_month = e->end_day = 0; // 종료일이 없는 경우 초기화
    }

    token = strtok(NULL, "|"); // 시작 시간
    if (token != NULL) {
        strncpy(e->start_time, token, sizeof(e->start_time) - 1);
        e->start_time[sizeof(e->start_time) - 1] = '\0'; // 널 종단 보장
    }
    else 
        strcpy(e->start_time, "");   

    token = strtok(NULL, "|"); // 반복 주기 (현재 사용 안 함)
    token = strtok(NULL, "|"); // 기타 정보 (현재 사용 안 함)

    token = strtok(NULL, "|"); // 메모
    if (token != NULL) {
        strncpy(e->memo, token, sizeof(e->memo) - 1);
        e->memo[sizeof(e->memo) - 1] = '\0'; // 널 종단 보장
    }
    else 
        strcpy(e->memo, "");    
}

// 일정 출력이 가능한 칸 찾기
int is_used(int week, int wday, int used_coordinate[6][7][3]) {

	if (used_coordinate[week][wday][0] == 1){
		return 0;
	}
    // 사용 가능한 줄 확인
    else if(used_coordinate[week][wday][1] ==  0){
        used_coordinate[week][wday][1] = 1;
        return 1;
    }
    else if(used_coordinate[week][wday][2] ==  0){
        used_coordinate[week][wday][2] = 1;
        return 2;
    }
    // 사용 가능한 줄이 없는 경우 0 리턴
    used_coordinate[week][wday][0] = 1;

	return 0;
}

void exceed_event(int row, int col,int day, int event_cnt[]) {
    event_cnt[day] += 1;
	
	attron(COLOR_PAIR(6));
	mvprintw(row, col + 3, "%d+", event_cnt[day]);
	attroff(COLOR_PAIR(6));
}

int manage_schedule_cell(int op, int done, int row, int col, int color, char* name, char*blank) {
               
    if (!done)
    {
        attron(COLOR_PAIR(color));
        mvprintw(row + op, col, "%s", name);
        done = 1;
        attroff(COLOR_PAIR(color));
    }
    else
    {
        attron(COLOR_PAIR(color));
        mvprintw(row + op, col, "%s", blank);
        attroff(COLOR_PAIR(color));
    }
    return done;       
}

int print_event_table(struct info* event, int width, int x, int y_coordinate[3])
{
    char name[width+1];

    if (strlen(event->event_name) <= width+1){
		snprintf(name, width+1, "%-*s", width, event->event_name);
    }    
	else {
		char temp[width+1];
		strncpy(temp, event->event_name, width - 2);
		temp[width - 2] = '\0';  
		snprintf(name, width, "%s..", temp);
	}

    if (state == DAILY_CALENDAR)
    {
        mvprintw(x, y_coordinate[0], "%s", name);
        mvprintw(x, y_coordinate[1], "%s", event->start_time);

        for (int i = 0; i < strlen(event->memo); i += width * 2)
        {
            // 화면의 높이를 초과하면 중단
            if (x >= LINES - 3) {
                break;
            }

            mvwprintw(stdscr, x, y_coordinate[2], "%.*s", width * 2, &event->memo[i]);
        }
    }
    else if (state == MONTHLY_CALENDAR)
    {
        if (event->end_day == 0)
            mvprintw(x, y_coordinate[0], "%4d-%02d-%02d", event->start_year, event->start_month, event->start_day);
        else
            mvprintw(x, y_coordinate[0], "%4d-%02d-%02d ~ %4d-%02d-%02d", 
            event->start_year, event->start_month, event->start_day, event->end_year, event->end_month, event->end_day);


        mvprintw(x, y_coordinate[1], "%s", event->event_name);
        mvprintw(x, y_coordinate[2], "%s", event->start_time);
    }

    return x;
}

int print_event_week(struct info *e, const int width, int x, int cnt, int y_coordinate[], int used_coordinate[][7]) 
{
    char name[width+1];
    char blank[width+1];
    memset(blank, ' ', width+1);
    blank[width] = '\0';

    if (strlen(e->event_name) <= width+1) {
        snprintf(name, width+1, "%-*s", width, e->event_name);
    }
	else {
        char temp[width + 1];
		strncpy(temp, e->event_name, width - 2);
		temp[width - 2] = '\0';  
		snprintf(name, width+1, "%s..", temp);
	}
    
    if (e->end_day != 0){
        for (int i = e->end_wday; i >= e->start_wday ; i--)
        {
            int row = x;
            int col = y_coordinate[i];

            attron(COLOR_PAIR(color+7));
            if(i == e->start_wday)
                if (strlen(e->event_name) <= width+1)
                    mvprintw(row, col, "%s", name);
                else
                    mvprintw(row, col, "%s", e->event_name);
            else
                mvprintw(row, col, "%s", blank);
            attroff(COLOR_PAIR(color+7));

            mvprintw(0,0, "%d %d", e->start_wday, e->end_wday);
        }
        x++;
        return x; 
    }
    else
    {
        for (int i = cnt; i < 9; i++)
        {
            if (!used_coordinate[i][e->start_wday])
            {
                int row = x+i;
                int col = y_coordinate[e->start_wday];
                mvprintw(row, col, "%s", name);

                used_coordinate[i][e->start_wday] = 1;
                return x;
            }          
        }
        
    }
    return x;
}


// 달력에 연속일정을 출력하는 함수
void print_continuous_event(struct info *event, const int width, int color, int x_coordinate[6][7], int y_coordinate[6][7], int used_coordinate[6][7][3], int event_cnt[]) {
	char name[width+1];
    char temp[width+1];
	int op = -1, done = 0;
    int day = event->start_day;
    int row, col;
	if (strlen(event->event_name) <= width+1) {
        snprintf(name, width+1, "%-*s", width, event->event_name);
    }
	else {
		strncpy(temp, event->event_name, width - 2);
		temp[width - 2] = '\0';  
		snprintf(name, width+1, "%s..", temp);
	}

    memset(temp, ' ', width+1);
    temp[width] = '\0';

    for (int i = event->start_week; i <= event->end_week; i++)
    {
        if (i == event->start_week) // 첫째 주인 경우
        {
            int end = 6;
            if (event->start_week == event->end_week)
                end = event->end_wday;
            for (int j = event->start_wday; j <= end; j++)
            {
                row = x_coordinate[i][j];
                col = y_coordinate[i][j];

                if (op < 1)
                    op = is_used(i, j, used_coordinate);

                if (op == 0)
                    exceed_event(row, col, day, event_cnt);
                else {
                    if (j == 6)
                        done = manage_schedule_cell(op, done, row, col, color, name, temp);
                    else{
                        if (strlen(event->event_name) <= width+1)
                            done = manage_schedule_cell(op, done, row, col, color, name, temp);
                        else
                            done = manage_schedule_cell(op, done, row, col, color, event->event_name, temp);
                    }
                }
                used_coordinate[i][j][op] = 1; // 줄 사용 표시
                day++;
            }
        }
        else if (i == event->end_week) // 마지막 주인 겨우
        {
            for (int j = 0; j <= event->end_wday; j++)
            {
                row = x_coordinate[i][j];
                col = y_coordinate[i][j];

                if (op < 1)
                    op = is_used(i, j, used_coordinate);
                
                if (op == 0)
                    exceed_event(row, col, day, event_cnt);
                else
                {
                    done = manage_schedule_cell(op, done, row, col, color, event->event_name, temp);
                }

                used_coordinate[i][j][op] = 1; // 줄 사용 표시
                day++;
            }
        }
        else // 나머지 주들
        {
            for (int j = 0; j < 7; j++)
            {
                row = x_coordinate[i][j];
                col = y_coordinate[i][j];

                if (op < 1)
                    op = is_used(i, j, used_coordinate);
                
                if (op == 0)
                    exceed_event(row, col, day, event_cnt);
                else
                {
                    done = manage_schedule_cell(op, done, row, col, color, event->event_name, temp);
                }
                used_coordinate[i][j][op] = 1; // 줄 사용 표시
                day++;
            }
        }
    }

	return;
}

// 달력에 단일일정을 출력하는 함수
void print_single_event(struct info *event, const int width, int x_coordinate[6][7], int y_coordinate[6][7], int used_coordinate[6][7][3], int event_cnt[]) {
	mvprintw(0, 0, "%d", event->start_day);
	char name[width+1];
	int row, col, op;

	if (strlen(event->event_name) <= width+1){
		snprintf(name, width+1, "%-*s", width, event->event_name);
    }    
	else {
		char temp[width+1];
		strncpy(temp, event->event_name, width - 2);
		temp[width - 2] = '\0';  
		snprintf(name, width, "%s..", temp);
	}
	
	row = x_coordinate[event->start_week][event->start_wday];
    col = y_coordinate[event->start_week][event->start_wday];

    op = is_used(event->start_week, event->start_wday, used_coordinate);

    if (op == 0) {
	    event_cnt[event->start_day] += 1;
        attron(COLOR_PAIR(6));
	    mvprintw(row, col + 3, "%d+", event_cnt[event->start_day]);
	    attroff(COLOR_PAIR(6));
    }
    else
    {
        mvprintw(row + op, col, "%s", name);
    }


	return;
}

// 연속 일정을 읽고 달력에 출력하는 함수
void get_continuous_event(int year, int month, int start_wday, int width, int x_coordinate[6][7], int y_coordinate[6][7], int used_coordinate[6][7][3], int event_cnt[32]) {
    FILE *file;
	info e;
    char line[256];

    // 파일 열기
    if ((file = fopen(EVENT_FILE2, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        // 한 줄 파싱
        get_line(line, &e, 1);

        // 이번 달 일정인지 확인
        if ((e.start_year == year && e.start_month == month) || (e.end_year == year && e.end_month == month)) {
            // 이번 달 안에만 연속적인 일정인 경우
            if ((e.start_year == year && e.start_month == month) && (e.end_year == year && e.end_month == month)) 
            {
                e.start_wday = (e.start_day + start_wday - 1) % 7; // 일정 시작하는 요일 계산
                e.start_week = (e.start_day + start_wday - 1) / 7; // 일정 시작하는 주 계산
                e.end_week = (e.end_day + start_wday - 1) / 7; // 일정 끝나는 주 계산
                e.end_wday = (e.end_day + start_wday - 1) % 7; // 일정 끝나는 요일 계산
            }
            // 이번달과 다음달이 이어지는 일정인 경우
            else if ((e.start_year == year && e.start_month == month))
            {
                int days = days_in_month[month-1];
                // 윤년이고 2월인 경우, 하루를 추가
                if (month == 2 && is_leap_year(year)) {
                    days = 29;
                }
    
                // 일정 끝나는 요일 계산
                e.end_wday = get_start_day_of_month(e.end_year, e.end_month) - 1;
                if (e.end_wday < 0)
                    e.end_wday = 6;

                e.end_week = (days + start_wday - 1) / 7; //일정 끝나는 주 계산

                e.start_week = (e.start_day + start_wday - 1) / 7; // 일정 시작하는 주 계산
                e.start_wday = (e.start_day + start_wday - 1) % 7; // 일정 시작하는 요일 계산
            }
            else if ((e.end_year == year && e.end_month == month))
            {
                e.start_wday = get_start_day_of_month(e.end_year, e.end_month); // 일정 시작하는 요일 계산
                e.start_week = 0; // 일정 시작하는 주 계산
                e.end_week = (e.end_day + e.start_wday - 1) / 7; // 일정 끝나는 주 계산
                e.end_wday = (e.end_day + e.start_wday - 1) % 7; // 일정 끝나는 요일 계산
            }

			print_continuous_event(&e, width, color + 7, x_coordinate, y_coordinate, used_coordinate, event_cnt);
            color = ((color + 1) % 3) ;
		}
        
    }

    fclose(file); // 파일 닫기
}

// 단일 일정을 읽고 달력에 출력하는 함수
void get_single_event(int year, int month, int start_wday, int width, int x_coordinate[6][7], int y_coordinate[6][7], int used_coordinate[6][7][3], int event_cnt[32]) {
    FILE *file;
	info e;
    char line[256];

    // 파일 열기
    if ((file = fopen(EVENT_FILE, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        get_line(line, &e, 0);

        // 이번 달 일정인지 확인
        if (e.start_year == year && e.start_month == month) {
            e.start_week = (e.start_day + start_wday - 1) / 7; // 주차 계산
            e.start_wday = (e.start_day + start_wday - 1) % 7;// 요일 계산

			print_single_event(&e, width, x_coordinate, y_coordinate, used_coordinate, event_cnt);
		}
        
    }

    fclose(file); // 파일 닫기
}

void get_event_week(int year, int month, int start_day, int end_day, int width, int x, int y_coordinate[7], int used_coordinate[][7]) {
    FILE *file;
	info e;
    char line[256];
    int cnt = -1;
    // 파일 열기
    if ((file = fopen(EVENT_FILE2, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE2);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        // 한 줄 파싱
        get_line(line, &e, 1);

        int start_date = e.start_year * 10000 + e.start_month * 100 + e.start_day;
        int end_date = e.end_year * 10000 + e.end_month * 100 + e.end_day;
        int w_start_date = year * 10000 + month * 100 + start_day;
        int w_end_date = (start_day < end_day) ? year * 10000 + month * 100 + end_day : 
                         (e.end_month == 1) ? (year + 1) * 10000 + (1) * 100 + end_day : (year) * 10000 + (month+1) * 100 + end_day;
        
        if (start_date < w_start_date && w_end_date < end_date)
        {
            e.start_wday = 0;
            e.end_wday = 6;
            x = print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            cnt++;
            color = ((color + 1) % 3);
        }
        else if ((start_date >= w_start_date && start_date <= w_end_date) && w_end_date < end_date)
        {
            int days = days_in_month[month-1];
                // 윤년이고 2월인 경우, 하루를 추가
            if (month == 2 && is_leap_year(year)) {
                days = 29;
            }

            if (e.start_month == month)
                e.start_wday = e.start_day - start_day;
            else if (e.start_month == month+1 || e.start_month == 1)
                e.start_wday = days - start_day + e.start_day;
            
            e.end_wday = 6;

            x = print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            cnt++;
            color = ((color + 1) % 3);
        }
        else if (start_date < w_start_date && (w_end_date >= end_date && end_date >= w_start_date))
        {
            int days = days_in_month[month-1];
                // 윤년이고 2월인 경우, 하루를 추가
            if (month == 2 && is_leap_year(year)) {
                days = 29;
            }
            if (e.end_month == month)
                e.end_wday = e.end_day - start_day;
            else if (e.end_month == month+1 || e.end_month == 1)
                e.end_wday = days - start_day + e.end_day;
            
            e.start_wday = 0;
            
            x = print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            cnt++;
            color = ((color + 1) % 3);
        }
        else if ((start_date >= w_start_date && start_date <= w_end_date) && (w_end_date >= end_date && end_date >= w_start_date))
        {
            int days = days_in_month[month-1];
                // 윤년이고 2월인 경우, 하루를 추가
            if (month == 2 && is_leap_year(year)) {
                days = 29;
            }

            if (e.start_month == month)
                e.start_wday = e.start_day - start_day;
            else if (e.start_month == month+1 || e.start_month == 1)
                e.start_wday = days - start_day + e.start_day;

            if (e.end_month == month)
                e.end_wday = e.end_day - start_day;
            else if (e.end_month == month+1 || e.end_month == 1)
                e.end_wday = days - start_day + e.end_day;
            
            x = print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            cnt++;
            color = ((color + 1) % 3);

        }
    }

    fclose(file);

// 파일 열기
    if ((file = fopen(EVENT_FILE, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        // 한 줄 파싱
        get_line(line, &e, 1);

        if (start_day < end_day)
        {
            if ((e.start_year == year && e.start_month == month) &&
                (e.start_day >= start_day && e.start_day <= end_day))
            {
                e.start_wday = e.start_day - start_day;
                e.end_day = 0;
                print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            }     
        }
        else
        {
            int days = days_in_month[month-1];
                // 윤년이고 2월인 경우, 하루를 추가
            if (month == 2 && is_leap_year(year)) {
                days = 29;
            }
            
            if ((e.start_year == year && e.start_month == month) &&
                (e.start_day >= start_day && e.start_day <= days))
            {
                e.start_wday = e.start_day - start_day;
                e.end_day = 0;
                print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);

            }
            else if (((e.start_year == year && e.start_month == month + 1) || (e.start_year == year + 1 && e.start_month == 1)) &&
                    (e.start_day >= 1 && e.start_day <= end_day))
            {
                e.start_wday = days - start_day + e.start_day;
                e.end_day = 0;
                print_event_week(&e, width, x, cnt, y_coordinate, used_coordinate);
            }
        }
    }

    fclose(file); // 파일 닫기
}

void get_event_in_table(int year, int month, int day, int width, int x, int y_coordinate[])
{
    FILE *file;
	info e;
    char line[256];

    // 파일 열기
    if ((file = fopen(EVENT_FILE2, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        // 한 줄 파싱
        get_line(line, &e, 1);
        if (state == DAILY_CALENDAR){
        // 이번 달 일정인지 확인
            int start_date = e.start_year * 10000 + e.start_month * 100 + e.start_day;
            int end_date = e.end_year * 10000 + e.end_month * 100 + e.end_day;
            int current_date = year * 10000 + month * 100 + day;

            if (current_date >= start_date && current_date <= end_date) {
                x = print_event_table(&e, width, x, y_coordinate);
                x++;
            }
        }
        else if (state == MONTHLY_CALENDAR)
        {
            struct winsize wbuf;
            // 터미널 화면정보 다시 가져오기
                if(ioctl(0, TIOCGWINSZ, &wbuf) != -1) 
                {
                    if (wbuf.ws_row < 20 || wbuf.ws_col < 80){
                        x = print_event_table(&e, width, x, y_coordinate);
                        x++;
                    }
                }
        }
        
    }
    fclose(file); // 파일 닫기

    // 파일 열기
    if ((file = fopen(EVENT_FILE, "r")) == NULL) {
        endwin();
        fprintf(stderr, "Error: 일정을 불러올 수 없습니다.\n");
        perror(EVENT_FILE);
        exit(1);
    }

    // 파일 읽기
    while (fgets(line, sizeof(line), file) != NULL) {
        // 한 줄 파싱
        get_line(line, &e, 0);

        // 이번 달 일정인지 확인
        if(state == DAILY_CALENDAR){
            if (e.start_year == year && e.start_month == month && e.start_day == day) {
                

                x = print_event_table(&e, width, x, y_coordinate);
                x++;
            }
        }
        else if (state == MONTHLY_CALENDAR)
        {
            struct winsize wbuf;
            // 터미널 화면정보 다시 가져오기
                if(ioctl(0, TIOCGWINSZ, &wbuf) != -1) 
                {
                    if (wbuf.ws_row < 20 || wbuf.ws_col < 80){
                        x = print_event_table(&e, width, x, y_coordinate);
                        x++;
                    }
                }
        } 
    }

    fclose(file); // 파일 닫기
}