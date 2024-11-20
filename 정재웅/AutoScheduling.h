#pragma once
#include "event.h"

void initTime(Time* x);

void ask(char* message);

int isLeapYear(int year);

int daysInMonth(int month, int year);

void intervalError(Time start, Time end);

void calInterval(Event* todo);

void calDday(Event* todo, Time current);

void calWeight(Event* todo, int importance);

void printCalendar(Event* todo);