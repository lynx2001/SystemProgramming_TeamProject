#pragma once
#include "global.h"
#include "SchedulingErrorCheck.h"
#include "CalculateScheduling.h"
#include "date_check.h"
#include "util.h"

void initTime(Time* x);

void ask(char* message);

int isLeapYear(int year);

int daysInMonth(int month, int year);

void intervalError(Time start, Time end);

void calInterval(Event* todo);

void calDday(Event* todo, Time current);

void calWeight(Event* todo, int importance);

void printCalendar(Event* todo);

void handleDateChangeSignal(int signal);