// event_io.h
#ifndef EVENT_IO_H
#define EVENT_IO_H

#include "global.h"
#include <stdbool.h>

int getIntInput(const char *prompt);
double getDoubleInput(const char *prompt);
void getStringInput(const char *prompt, char *input, int size, int allowEmpty);
int isLeapYear(int year);
bool validateDate(int year, int month, int day);
bool validateTime(int hour, int minute);
void getDateTimeInput();

#endif
