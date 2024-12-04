#ifndef HABIT_H
#define HABIT_H

#define MAX_HABITS 100

typedef struct {
    int id;             // 습관 ID
    char name[50];      // 습관 이름
    int streak; 
    int is_done;        // 유지 일수
} Habit;

extern Habit habits[MAX_HABITS];
extern int habit_count;
extern int last_habit_id;

void habit_submenu();  // 습관 관리 서브 화면
void loadHabits();
void signalresetHabits(); 
void saveHabits();      

#endif

