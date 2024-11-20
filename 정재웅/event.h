#pragma once
// event.h
#ifndef EVENT_H
#define EVENT_H

#define MAX_EVENTS 100

typedef struct {    // �ð� ����
    int year;
    int month;
    int day;
    int hour;
    int minute;
} Time;

typedef struct {
    int id;                 // ���� id - ����, ���� �� �̿�     >> PRIMARY_KEY
    char name[50];          // �̸�(����)
    Time date_start;    // ������
    Time date_end;      // ������
    double interval;        // ������ - ������ (�ð� ����)
    double Dday;            // ������ - ������ (�ð� ����)
    double weight;          // �켱����
    double quantity;        // �з�
    char details[100];      // ���λ���(ª�� �޸�)
} Event;

extern Event events[MAX_EVENTS];
extern int event_count;

extern int last_id;     // ������ id �� (id �� �ڵ������� ����)

#endif