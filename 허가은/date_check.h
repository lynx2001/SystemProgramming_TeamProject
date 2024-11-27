// date_check.h
#ifndef DATE_CHECK_H
#define DATE_CHECK_H

#include <pthread.h>

// 날짜 변경 이벤트 콜백 함수 타입
typedef void (*date_change_callback_t)(void);

// 초기화 함수: 콜백 함수 등록
void initializeDateMonitor(date_change_callback_t callback);

// 날짜 변경 감지 스레드 시작
void startDateMonitor();

// 날짜 변경 감지 스레드 종료
void stopDateMonitor();                
#endif