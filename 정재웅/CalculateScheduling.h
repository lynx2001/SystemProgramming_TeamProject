#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "event.h"

time_t timer;
struct tm* t;

void initTime(Time* x);

void calInterval(Event* event_t);

void calDday(Event* event_t, Time current);

void calWeight(Event* event_t, int importance);

int calculateStartDayOfWeek(int year, int month, int day);