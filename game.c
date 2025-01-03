#include<ncurses.h>
#include "ascii_art.h"

void draw_main_screen();

void draw_main_screen(WINDOW* main){
    int sword_heart_posx = 3, sword_heart_posy = 3;
    int rogy_posx = 3, rogy_posy = COLS/2 - 15;
    int bomb_posx = 4, bomb_posy = COLS - 20;

    //draw border of main
    box(main, '|', 0);

    //draw sword heart
    WINDOW* heart_win = newwin(sword_heart_len, 30, sword_heart_posx, sword_heart_posy);
    wattron(heart_win, COLOR_PAIR(2));
    for(int i = 0; i < sword_heart_len; i++){
        wmove(heart_win, i, 0);
        wprintw(heart_win, "%s", sword_heart[i]);
    }
    refresh();
    wrefresh(heart_win);

    //draw Rogy
    WINDOW* rogy_win = newwin(rogy_len, 30, rogy_posx, rogy_posy);
    wattron(rogy_win, A_BLINK);
    wattron(rogy_win, COLOR_PAIR(3));
    for(int i = 0; i < rogy_len; i++){
        wmove(rogy_win, i, 0);
        wprintw(rogy_win, "%s", rogy[i]);
    }
    refresh();
    wrefresh(rogy_win);

    //draw Bomb
    WINDOW* bomb_win = newwin(bomb_len, 15, bomb_posx, bomb_posy);
    wattron(bomb_win, COLOR_PAIR(4));
    for(int i = 0; i < bomb_len; i++){
        wmove(bomb_win, i, 0);
        wprintw(bomb_win, "%s", bomb[i]);
    }
    refresh();
    wrefresh(bomb_win);
}