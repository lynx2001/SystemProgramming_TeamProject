// event_io.h
#ifndef UTIL_H
#define UTIL_H

#include "global.h"
#include <stdbool.h>
#include <ncurses.h>
#include <string.h>

bool validateDate(int year, int month, int day);
bool validateTime(int hour, int minute);
void popup_message(const char *message);

#endif
