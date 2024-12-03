#ifndef CALENDAR_CONTROL_H
#define CALENDAR_CONTROL_H

#include "calendar.h"

void show_calendar();
void change_date(struct tm *date, int num);
void handle_winch(int sig);

#endif
