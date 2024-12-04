#pragma once
#ifndef CALCULATESCHEDULING_H
#define CALCULATESCHEDULING_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "global.h"
#include "SchedulingErrorCheck.h"
#include "AutoScheduling.h"
#include "date_check.h"

extern time_t timer;
extern struct tm* t;

void initTime(Time* x);

void calInterval(Event* event_t);

void calDday(Event* event_t, Time current);

void calWeight(Event* event_t, int importance);

int calculateStartDayOfWeek(int year, int month, int day);

void updateDdayAndWeights(Event events[], int count);

#endif