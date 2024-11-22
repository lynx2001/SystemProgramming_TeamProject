// event.h
#ifndef EVENT_H
#define EVENT_H

#include "global.h"

void loadEvent();
void saveEvent();
Event getEventInput(int is_scheduled);
void addScheduledEvent();
void addEvent();
void deleteEvent();
void editEvent();
int calculateInterval(Time, Time);
double calculateDday(Time);
void showEvent();
void showScheduledEvent();

#endif
