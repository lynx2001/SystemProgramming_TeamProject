#include <stdio.h>
#include <string.h>
#include <curses.h>
#include "calendar_display.h"

// 달 이름과 요일 이름
char months_to_string[][20] = {"Januray", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
char day_of_the_week[][4] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
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

// 요일을 출력하는 함수
void print_week(int week_row, int *week_col, int cell_width)
{
    for (int i = 0; i < 7; i++) {
        int col = week_col[i] + (cell_width - 3) / 2;

        // 일요일은 빨간색, 토요일은 파란색 표시
        if (i == 0)
            attron(COLOR_PAIR(1));
        else if (i == 6)
            attron(COLOR_PAIR(2));

        mvprintw(week_row, col, "%s", day_of_the_week[i]);

        if (i == 0) 
            attroff(COLOR_PAIR(1));
        else if (i == 6) 
            attroff(COLOR_PAIR(2));
    }
}

// 날짜를 출력하는 함수
void print_day(int row, int col, int day, int current_day, int wday)
{
    // 현재 날짜를 표시하는 경우
    // 평일, 토, 일을 각각 다르게 표시
    if (current_day == day)
    {   
        if (wday == 0) 
            attron(COLOR_PAIR(4));
        else if (wday == 6) 
            attron(COLOR_PAIR(5));
        else
            attron(COLOR_PAIR(3));

        mvprintw(row, col, "%2d", current_day);

        if (wday == 0) 
            attroff(COLOR_PAIR(4));
        else if (wday == 6) 
            attroff(COLOR_PAIR(5));
        else
            attroff(COLOR_PAIR(3));
    }
    // 현재 날짜를 표시하지 않는 경우
    // 평일, 토, 일을 각각 다르게 표시
    else
    {
        if (wday == 0) 
            attron(COLOR_PAIR(1));
        else if (wday == 6) 
            attron(COLOR_PAIR(2));

        mvprintw(row, col, "%2d", current_day);

        if (wday == 0) 
            attroff(COLOR_PAIR(1));
        else if (wday == 6) 
            attroff(COLOR_PAIR(2));
    }
}

// 달력 형태의 달 기준 캘린더(큰 화면)
void show_big_month(int year, int month, int day, struct winsize *wbuf) 
{
    int start_day = get_start_day_of_month(year, month); // 첫 번째 요일 계산
    int days = days_in_month[month - 1]; // 해당 월의 일수
	
	// 달력 한 칸의 x, y 좌표
	int x_coordinate[6][7];
	int y_coordinate[6][7];

    // 윤년이고 2월인 경우, 하루를 추가
    if (month == 2 && is_leap_year(year)) {
        days = 29;
    }

    // 타이틀(월, 년도)
	char month_year[40];
	snprintf(month_year, sizeof(month_year), "%s %d", months_to_string[month-1], year);

    // 현재 창의 row, col
	int max_rows = wbuf->ws_row;
    int max_cols = wbuf->ws_col;

	// 현재 창의 row, col 크기에 따라 각 요소의 위치를 자동으로 조절
    int title_row = max_rows / 10; 
    int title_col = (max_cols - strlen(months_to_string[month - 1]) - 5) / 2;
    int weekday_row = title_row + 2;
	int cell_width = (max_cols - 20) / 7;
    int cell_height = (max_rows - weekday_row - 4) / 6; 

	// 달력 칸 당 좌표 입력
	int day_row = weekday_row + 2;
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 7; j++) {
            x_coordinate[i][j] = day_row + i * cell_height;
            y_coordinate[i][j] = j * cell_width + 10;
        }
    }
    
	// 월과 연도 출력
    attron(A_BOLD);
    mvprintw(title_row, title_col, "%s %d", months_to_string[month - 1], year);
    attroff(A_BOLD);

	// 요일 출력
    print_week(weekday_row, y_coordinate[0], cell_width);

	mvhline(weekday_row+1, y_coordinate[0][0], '-', cell_width*7);
	
	// 일 출력
	int week_day = start_day;
    for (int i = 1; i <= days; i++) {
        int row = x_coordinate[(i + start_day - 1) / 7][(i + start_day - 1) % 7];
        int col = y_coordinate[(i + start_day - 1) / 7][(i + start_day - 1) % 7];

        print_day(row, col, day, i, week_day);        

        week_day = (week_day + 1) % 7;
    }
    refresh();
}

// 표 형태의 달 기준 캘린더(작은 화면)
void show_small_month(int year, int month, struct winsize *wbuf)
{ 
    // 타이틀 (월, 년도)
	char month_year[40];
	snprintf(month_year, sizeof(month_year), "%s %d", months_to_string[month-1], year);

    // 현재 창의 row, col
	int max_rows = wbuf->ws_row;
    int max_cols = wbuf->ws_col;

    // 현재 창의 row, col 크기에 따라 각 요소의 위치를 자동으로 조절
    int title_row = max_rows / 10; 
    int title_col = (max_cols - strlen(month_year) - 5) / 2;
	int header_col = max_cols / 6;
	// int schedule_row, schedule_col, date_col, t_col; // 스케줄 row와 스케줄, 날짜, 시간 col;

	// 월과 연도 출력
    attron(A_BOLD);
    mvprintw(title_row, title_col, "%s %d", months_to_string[month - 1], year);
    attroff(A_BOLD);

    // 일정표 상단 부분 출력
	mvprintw(title_row + 2, header_col + (header_col - 4) / 2, "date");
	mvprintw(title_row + 2, header_col * 2 + (header_col * 2 - 8) / 2, "schedule");
	mvprintw(title_row + 2, header_col * 4 + (header_col - 4) / 2, "time");

	mvhline(title_row + 3, header_col, '-', header_col * 4);

	// 리스트 형식의 스케줄 좌표
	// schedule_row = title_row + 4;
	// schedule_col = header_col * 2;
	// date_col = header_col;
    // t_col = header_col * 5;

	/*
	for (int i = 0; i < cnt; i++)
	{
		// 날짜와 스케줄 받아서 출력
	}
	*/

    refresh();
}

// 주 기준 캘린더 
void show_week(int year, int month, int day, struct winsize *wbuf)
{
	int start_day = get_start_day_of_month(year, month); // 첫 번째 요일 계산
	int wday = (day + start_day - 1) % 7;	// 인자로 받은 날짜의 요일 

    // 주간 날짜 계산 (일요일부터 토요일까지)
    int week_start_day = day - wday;  // 일요일을 기준으로 한 주의 시작일
    if (week_start_day <= 0) {
        // 주 시작이 이전 달의 날짜로 넘어가는 경우
		if (month - 1 == 0)
		{
			year -= 1;
			month = 12;
		}
		else
			month -= 1;
			
        week_start_day = days_in_month[month - 1] + week_start_day;  // 이전 달에서 날짜 계산

		// 윤년이고 2월인 경우, 하루를 추가
		if (month == 2 && is_leap_year(year)) {
			week_start_day += 1;
		}
	}

    // 타이틀(월, 년)
    char week_title[40];
    snprintf(week_title, sizeof(week_title), "%s %d", months_to_string[month-1], year);

    // 현재 창의 row, col
    int max_rows = wbuf->ws_row;
    int max_cols = wbuf->ws_col;

	// 현재 창의 row, col 크기에 따라 각 요소의 위치를 자동으로 조절
    int title_row = max_rows / 10; 
    int title_col = (max_cols - strlen(week_title) - 5) / 2;
    int weekday_row = title_row + 2;
    int cell_width = (max_cols - 20) / 7;

    // 한 주의 칸 당 좌표 입력
    int x_coordinate; 
    int y_coordinate[7];
    
    for (int i = 0; i < 7; i++) {
        x_coordinate = weekday_row + 2;
        y_coordinate[i] = i * cell_width + 10;
    }

    // 주간 제목 출력
    attron(A_BOLD);
    mvprintw(title_row, title_col, "%s", week_title);
    attroff(A_BOLD);

    // 요일 출력
    print_week(weekday_row, y_coordinate, cell_width);

    mvhline(weekday_row + 1, y_coordinate[0], '-', cell_width * 7);

    // 주간 날짜 출력 (일요일부터 토요일까지)
    for (int i = 0; i < 7; i++) {
        int current_day = week_start_day + i;
        int row = x_coordinate;
        int col = y_coordinate[i];
        
		// 다음 달 날짜 처리
		// 현재 윤년이고 2월인 경우와 그렇지 않은 경우에 따라 처리방식 구분
        if (current_day > days_in_month[month - 1] && (month == 2 && is_leap_year(year))) {
            if (current_day > 29)
				current_day -= 29;
		}
		else if(current_day > days_in_month[month - 1])
		{
			current_day -= days_in_month[month - 1];
		}
        
        // 요일에 따라 색상 적용
        print_day(row, col, day, current_day, i);        

    }
    refresh();
}

// 일 기준 캘린더
void show_day(int year, int month, int day, struct winsize *wbuf)
{
	int start_day = get_start_day_of_month(year, month); // 첫 번째 요일 계산
	int wday = (day + start_day - 1) % 7;	// 인자로 받은 날짜의 요일 

    // 타이틀(월, 년도)
	char week_title[40];
    snprintf(week_title, sizeof(week_title), "%s %d", months_to_string[month-1], year);
    
    // 현재 창의 row, col
    int max_rows = wbuf->ws_row;
    int max_cols = wbuf->ws_col;

	// 현재 창의 row, col 크기에 따라 각 요소의 위치를 자동으로 조절
    int title_row = max_rows / 10; 
    int title_col = (max_cols - strlen(week_title) - 5) / 2;
    int day_row = title_row + 2;
    int day_col = max_cols / 6;

    // 일간 켈린더 제목
	attron(A_BOLD);
    mvprintw(title_row, title_col, "%s", week_title);
    attroff(A_BOLD);

    // 평일, 토, 일을 각각 다르게 출력
	if (wday == 0) {
		attron(COLOR_PAIR(4));
	}
	else if (wday == 6) {
		attron(COLOR_PAIR(5));
	}
	else {
		attron(COLOR_PAIR(3));
	}

	mvprintw(day_row, day_col, " %-2d %s ", day, day_of_the_week[wday]);
	
	if (wday == 0) {
		attroff(COLOR_PAIR(4));
	}
	else if (wday == 6) {
		attroff(COLOR_PAIR(5));
	}
    else {
        attroff(COLOR_PAIR(3));
	}
	
	mvprintw(day_row + 2, day_col + (day_col - 8) / 2, "schedule");
	mvprintw(day_row + 2, day_col * 2 + (day_col - 4) / 2, "time");
	mvprintw(day_row + 2, day_col * 3 + (day_col * 2 - 4) / 2, "memo");

	mvhline(day_row + 3, day_col, '-', day_col * 4);


    refresh();
}