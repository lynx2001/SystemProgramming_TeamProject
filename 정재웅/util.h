// util.h
#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <ncurses.h>
#include <string.h>

extern bool popup_message_called;

//유효성 테스트용 임시
bool validateDate(int year, int month, int day);
bool validateTime(int hour, int minute);

void popup_message(const char *message);
void handle_resize(int sig);
int get_input(const char *prompt, char *buffer, int size);

#endif