#pragma once
// event.h
#ifndef EVENT_H
#define EVENT_H

#define MAX_EVENTS 100

typedef struct {    // 시간 관리
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Time;

typedef struct {
    int id;                 // 고유 id - 삭제, 수정 등 이용     >> PRIMARY_KEY
    char name[50];          // 이름(제목)
    Time date_start;    // 시작일
    Time date_end;      // 종료일
    double interval;        // 마감일 - 시작일 (시간 제외)
    double Dday;            // 마감일 - 현재일 (시간 제외)
    double weight;          // 우선순위
    double quantity;        // 분량
    char details[100];      // 세부사항(짧은 메모)
} Event;

extern Event events[MAX_EVENTS];
extern int event_count;

extern int last_id;     // 마지막 id 값 (id 값 자동생성을 위함)

#endif