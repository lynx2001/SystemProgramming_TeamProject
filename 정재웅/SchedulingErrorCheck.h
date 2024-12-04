#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "global.h"
#include "CalculateScheduling.h"
#include "AutoScheduling.h"
#include "date_check.h"

int validateDate(int year, int month, int day);

int validateImportance(int importance);

int isLeapYear(int year);

int daysInMonth(int month, int year);

void intervalError(Time start, Time end);