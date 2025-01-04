#include<ncurses.h>

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//Unicode
#include <wchar.h>
#include <locale.h>

//time
#include<time.h>

#include "file_manager.c"
#include "input.c"
#include "utils.c"
#include "intro_page.c"
#include "newuser.c"
#include "loginuser.c"
#include "pregame.c"

int main(){
    //init
    srand(time(0));
    WINDOW* mn = initscr();

    if(!st_color()){
        endwin();
        printf("Color doesn't supported");
        return 0;
    }
    
    draw_main_screen(mn);
    int op = handle_login_input();
    if(op == 1){
        clear();
        newuser_draw_main_screen(mn);
        newuser_handle_input();
        clear();
        main();
    }else if(op == -1){
        clear();
        loginuser_draw_main_screen(mn);
        char* username = loginuser_handle_uinput();
        clear();
        pregame_draw_main_screen(mn, username);
        pregame_handle_input();
    }
    getch();
    endwin();
    return 0;
}
