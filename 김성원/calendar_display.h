#ifndef CALENDAR_DISPLAY_H
#define CALENDAR_DISPLAY_H

#include <sys/ioctl.h>

extern char months_to_string[12][20];	// 달 이름
extern char day_of_the_week[7][4];	// 요일 이름
extern int days_in_month[12];  // 각 월의 일수를 저장한 배열 (윤년이 아닌 경우)

// 윤년인지 확인하는 함수
int is_leap_year(int year);

// 연도와 월을 입력받아 해당 월의 첫 번째 요일을 계산하는 함수
// 0: 일요일, 1: 월요일, ..., 6: 토요일
// Zeller's Congruence를 사용하여 요일 계산
int get_start_day_of_month(int year, int month);

void print_week(int week_row, int *week_col, int cell_width); // 요일을 출력하는 함수
void print_day(int row, int col, int day, int current_day, int wday); // 날짜를 출력하는 함수

void show_big_month(int year, int month, int day, struct winsize *wbuf); // 달력 형태의 달 기준 캘린더(큰 화면)
void show_small_month(int year, int month, struct winsize *wbuf);		 // 표 형태의 달 기준 캘린더(작은 화면)
void show_week(int year, int month, int day, struct winsize *wbuf);		 // 주 기준 캘린더 
void show_day(int year, int month, int day, struct winsize *wbuf);		 // 일 기준 캘린더

#endif
