#ifndef UTILS_H
#define UTILS_H

#include<regex.h>

#define ROOM_BORDER_COLOR 15
#define ROOM_INSIDE_COLOR 16
#define PILLAR_COLOR 18
#define DOOR_COLOR 17
#define TRAP_COLOR 19
#define STAIR_COLOR 20
#define BLACK_GOLD_COLOR 21

int is_dig(char ch){
    return (ch >= '0' && ch <= '9');
}
int is_upper(char ch){
    return (ch >= 'A' && ch <= 'Z');
}
int is_lower(char ch){
    return (ch >= 'a' && ch <= 'z');
}

char* email_regex = "([a-zA-Z0-9_\\-\\.]+)@([a-zA-Z0-9_\\-\\.]+)\\.([a-zA-Z]+)$";
int regex_match(char*msg, char*reg){
    regex_t reegex;
    int value;
    value = regcomp( &reegex, reg, REG_EXTENDED);
    value = regexec( &reegex, msg,
                    0, NULL, 0);
    if (value == 0) 
        return 1;
    return 0;
}

char* tostring(int N) {
    if(N == 0)
        return "0";
    int i = 0;
    char* str = malloc(100);
    int sign = N;
    while (N > 0) {
        str[i++] = N % 10 + '0';
      	N /= 10;
    } 
    str[i] = '\0';
    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;
    }
    return str;
}

char* append(char* a, char* b){
    char* ans = malloc(strlen(a) + strlen(b));
    strcpy(ans, a);
    strcat(ans, b);
    return ans;
}

void swap(int*a, int*b){
    int tmp = *a;
    *a = *b;
    *b = tmp;
}
//color pair 0 -> black
//color pair 1 -> white
// .. random

int random_num(int min, int max){
    return rand() % (max - min + 1) + min;
}
int st_color();
void random_color();
const int RANDOM_COLORS = 8;

int st_color(){
    start_color();
    random_color();  
    init_color(ROOM_BORDER_COLOR, 1000, 372, 121);
    init_pair(ROOM_BORDER_COLOR, ROOM_BORDER_COLOR, 0);

    init_color(ROOM_INSIDE_COLOR, 0, 1000, 121);
    init_pair(ROOM_INSIDE_COLOR, ROOM_INSIDE_COLOR, 0);

    init_color(DOOR_COLOR, 1000, 1000, 0);
    init_pair(DOOR_COLOR, DOOR_COLOR, 0);

    init_color(PILLAR_COLOR, 1000, 500, 500);
    init_pair(PILLAR_COLOR, PILLAR_COLOR, 0);

    init_color(TRAP_COLOR, 0, 500, 500);
    init_pair(TRAP_COLOR, TRAP_COLOR, 0);

    init_color(STAIR_COLOR, 456, 100, 500);
    init_pair(STAIR_COLOR, STAIR_COLOR, 0);

    init_color(BLACK_GOLD_COLOR, 1000, 900, 1000);
    init_pair(BLACK_GOLD_COLOR, BLACK_GOLD_COLOR, 0);
    return can_change_color();
}
void random_color(){
    init_color(0, 0, 0, 0);
    init_color(1, 1000, 1000, 1000);
    init_pair(1, 1, 0);
    for(int i = 2; i < RANDOM_COLORS + 2; i++){
        int r = rand() % 800 + 200;
        int b = rand() % 800 + 200;
        int g = rand() % 800 + 200;
        init_color(i, r, g, b);
        init_pair(i, i, 0);
    }
}
int red(){
    int i = rand() % 50 + 100;
    init_color(i, 1000, 0, 0);
    init_pair(i, i, 0);
    return i;
}
int blue(){
    int i = rand() % 50 + 100;
    init_color(i, 0, 0, 1000);
    init_pair(i, i, 0);
    return i;
}
int yellow(){
    int i = rand() % 50 + 100;
    init_color(i, 1000, 1000, 0);
    init_pair(i, i, 0);
    return i;
}
#endif