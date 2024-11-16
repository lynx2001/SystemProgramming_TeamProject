// habit.h
#ifndef HABIT_H
#define HABIT_H

#define MAX_HABITS 100

typedef struct {
    int is_done;    // 목표 완료여부
    int days;       // 목표 성공 지속일수
    char name[30];  // 목표 이름
} Habit;

extern Habit habits[MAX_HABITS];
extern int habit_count;

#endif