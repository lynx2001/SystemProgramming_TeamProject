#ifndef HABIT_H
#define HABIT_H

#define MAX_HABITS 100

void getCurrentDate(char *date);
void loadHabits();
void saveHabits();
int hasDateChanged();
void resetHabitsIfDateChanged();
void addHabit(const char *name);
void markHabitDone(const char *name);
void deleteHabit(const char *name);
void showHabits();

#endif 
