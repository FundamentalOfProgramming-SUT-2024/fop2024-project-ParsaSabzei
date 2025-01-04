#include<ncurses.h>
#include "ascii_art.h"

void draw_main_screen();
void trig(WINDOW*, int, int*);
void change_status();
int handle_login_input();

WINDOW *new_user_btn, *login_user_btn;
int status = 1, btn1 = 1, btn2 = 0;

void draw_main_screen(WINDOW* main){
    int BRACK = 10;
    int man_posx = 2, man_posy = COLS - 34;
    int who_posx = 5, who_posy = 20;
    int new_user_btn_height = 8, new_user_btn_width = 20;
    int new_user_btn_posx = LINES/2 + 10, new_user_btn_posy = COLS/2 - 30;
    int login_user_btn_posx = LINES/2 + 10, login_user_btn_posy = COLS/2 + 10;

    //init
    curs_set(FALSE);
    keypad(stdscr, true);
    noecho();

    //draw border of main
    box(main, '|', 0);
    for(int i = BRACK - 1; i >= 1; i--){
        move(BRACK - i - 1, 0);
        for(int j = 0; j < i; j++){
            printw("/ ");
        }
    }

    //draw man
    WINDOW* man_win = newwin(login_man_len, 30, man_posx, man_posy);
    wattron(man_win, COLOR_PAIR(2));
    for(int i = 0; i < login_man_len; i++){
        wmove(man_win, i, 0);
        wprintw(man_win, "%s", login_man[i]);
    }
    refresh();
    wrefresh(man_win);

    //draw who are you
    WINDOW* who_win = newwin(who_are_len, 85, who_posx, who_posy);
    wattron(who_win, COLOR_PAIR(3));
    wattron(who_win, A_BLINK);
    for(int i = 0; i < who_are_len; i++){
        wmove(who_win, i, 0);
        wprintw(who_win, "%s", who_are[i]);
    }
    refresh();
    wrefresh(who_win);

    //new user button
    new_user_btn = newwin(new_user_btn_height, new_user_btn_width, new_user_btn_posx, new_user_btn_posy);
    box(new_user_btn, 0, 0);
    mvwprintw(new_user_btn, new_user_btn_height/2, 6, "New User");
    trig(new_user_btn, 4, &btn1);
    refresh();
    wrefresh(new_user_btn);

    //login user btn
    login_user_btn = newwin(new_user_btn_height, new_user_btn_width, login_user_btn_posx, login_user_btn_posy);
    box(login_user_btn, 0, 0);
    mvwprintw(login_user_btn, new_user_btn_height/2, 5, "Login User");
    trig(login_user_btn, 5, &btn2);
    refresh();
    wrefresh(login_user_btn);

    //Status
    change_status();

    move(LINES - 1, COLS/2 - 12);
    printw("Press ENTER to continue...");
}
void change_status(){
    char* msg[2] = {"Login User", "New User"};
    move(LINES/2 + 5, COLS / 2 - 15);
    printw("                                               ");
    move(LINES/2 + 5, COLS / 2 - 15);
    printw("Current Selected : %s", msg[status]);
    status ^= 1;
}

// Press ENTER or 'c' to continue
// returns 1 for newuser and -1 for login btn
int handle_login_input(){
    while(true){
        int c = getch();
        if(c == 'q')
            return 0;
        if(c == KEY_RIGHT && !status){
            trig(new_user_btn, 4, &btn1);
            trig(login_user_btn, 5, &btn2);
            change_status();
        }else if(c == KEY_LEFT && status){
            trig(new_user_btn, 4, &btn1);
            trig(login_user_btn, 5, &btn2);
            change_status();
        }else if(c == 10 || c == 'c'){
            // new user
            if(status == 0)
                return 1;
            //login
            return -1;
        }
    }
}
