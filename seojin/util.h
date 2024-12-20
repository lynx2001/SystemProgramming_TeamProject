// util.h
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <ncurses.h>
#include <string.h>
#include "event.h"
#include "display.h"

extern bool popup_message_called;

//유효성 테스트용 임시
bool validateDate(int year, int month, int day);
bool validateTime(int hour, int minute);

void popup_message(const char *message);
int popup_confirmation(const char *message);
void handle_resize(int sig);
int get_input(char *buffer, int size);
int process_user_input(UIScreen *screen);
bool validate_reminder(const char *input);
bool validate_date_wrapper(const char *input);
bool validate_time_wrapper(const char *input);
bool validate_weight(const char *input);
bool validate_quantity(const char *input);

extern UIScreen *active_screen;
extern int current_step;

#endif
