#include <ncursesw/ncurses.h> 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
//Unicode
#include <wchar.h>
#include <locale.h>

//time
#include<time.h>

#include "effect.c"
#include "file_manager.c"
#include "input.c"
#include "utils.h"
#include "intro_page.c"
#include "newuser.c"
#include "loginuser.c"
#include "pregame.c"
#include "game.c"
#include "settings.c"
#include "scoreboard.c"

int main(){
    //Enable emoji
    setlocale(LC_ALL, "");

    //init
    srand(time(0));

   // do_art();
    clear();
    
    WINDOW* mn = initscr();

    if(!st_color()){
        endwin();
        printf("Color doesn't supported");
        return 0;
    }
    
    while(true){
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
            int scr = pregame_handle_input();
            if(scr == 0 || scr == 1) //create new game button
                init_game(mn, username, scr);
            else if(scr == 2 || scr == 1){
                settings_draw_main_screen(mn, username);
                break;
            }else if(scr == 3){  //scoreboard
                scoreboard_draw_main_screen(mn, username);
            }
        }
    }

    endwin();
    return 0;
}
