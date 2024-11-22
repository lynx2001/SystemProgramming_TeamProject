// util.c
#include "global.h"
#include "util.h"
#include <stdio.h>

// 파일 init
void check_and_create_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        printf("파일이 이미 존재합니다: %s\n", filename);
    } else {
        // 파일이 존재하지 않는 경우, 파일 생성
        file = fopen(filename, "w");
        if (file) {
            printf("파일이 생성되었습니다: %s\n", filename);
            fclose(file);
        } else {
            perror("파일 생성 실패");
        }
    }
}

// last_id 가져오기
int get_last_id() {
    FILE *file = fopen(ID_FILE, "r");
    int id = 0;  // 기본 ID 값
    
    if (file == NULL) {
        return id;
    }

    if (fscanf(file, "%d", &id) != 1) { // 읽기 오류
        printf("ID 파일 읽기 오류, 기본값 0 사용\n");
        id = 0;
    }

    fclose(file);
    return id;
}

// last_id 기록
void save_id(int id) {
    FILE *file = fopen(ID_FILE, "w");   // write 방식 확인
    if (file == NULL) {
        perror("ID 파일 저장 오류");
        return;
    }
    fprintf(file, "%d", id);
    fclose(file);
}
