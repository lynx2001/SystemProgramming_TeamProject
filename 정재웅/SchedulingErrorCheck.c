#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "SchedulingErrorCheck.h"

int validateDate(int year, int month, int day) {
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > daysInMonth(month, year)) {
        fprintf(stderr, "��¥ ���� �Ǵ� ������ �ùٸ��� �ʽ��ϴ�. �ٽ� �Է��� �ּ���.\n");
        return 0;
    }
    return 1;
}

int validateImportance(int importance) {
    if (importance < 0 || importance > 5) {
        fprintf(stderr, "�߿䵵�� 0���� 5 ������ ���̾�� �մϴ�. �ٽ� �Է��� �ּ���.\n");
        return 0;
    }
    return 1;
}

int isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int daysInMonth(int month, int year) {
    switch (month) {
    case 4: case 6: case 9: case 11:
        return 30;
    case 2:
        return isLeapYear(year) ? 29 : 28;
    default:
        return 31;
    }
}

void intervalError(Time start, Time end) {
    if (start.year > end.year) {
        fprintf(stderr, "���ۿ����� ������������ �ʽ��ϴ�.\n");
        exit(1);
    }
    else if (start.year == end.year && start.day > end.day) {
        fprintf(stderr, "���ۿ��� ���������� �ʽ��ϴ�.\n");
        exit(1);
    }
    else if (start.year == end.year && start.month == end.month && start.day > end.day) {
        fprintf(stderr, "�������� �����Ϻ��� �ʽ��ϴ�.\n");
        exit(1);
    }
    else {
        printf("Success interval\n");
    }
}