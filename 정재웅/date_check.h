#ifndef DATE_CHECK_H
#define DATE_CHECK_H

#include <signal.h>
#include "global.h"
#include "AutoScheduling.h"
#include "CalculateScheduling.h"
#include "SchedulingErrorCheck.h"

void initializeDateMonitor();

void startDateMonitor();

void stopDateMonitor();

#endif // DATE_CHECK_H