#include<ncurses.h>

#define btn_counts 4
#define start_color_index 5

void pregame_draw_main_screen(WINDOW*, char*);
int pregame_handle_input();

char* username;
WINDOW** btns;

void pregame_draw_main_screen(WINDOW* main, char* _username){
    username = _username;

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

    //Welcome user
    move(0, 3);
    printw("Hello %s!", username);

    //Create new game btn
    WINDOW* create_game_btn = create_button(7, 25, 17, COLS/2 - 50, 5, "Create new game");

    //resume game btn
    WINDOW* resume_game_btn = create_button(7, 25, 17, COLS/2 + 15, 6, "Resume previous game");

    //Setting btn
    WINDOW* setting_btn = create_button(7, 25, 27, COLS/2 - 50, 7, "Settings");

    //Scoreboard
    WINDOW* scoreboard_btn = create_button(7, 25, 27, COLS/2 + 15, 8, "Scoreboard");

    btns = (WINDOW**)malloc(sizeof(WINDOW*) * btn_counts);
    btns[0]= create_game_btn, btns[1] = resume_game_btn, btns[2] = setting_btn, btns[3] = scoreboard_btn;
}

int cur_btn = 0;

// 0 -> create new game
// 1 -> resume game
// 2 -> settings
// 3 - > scoreboard
int pregame_handle_input(){
    int active[btn_counts];
    for(int i = 0; i < btn_counts; i++)
        active[i] = 1;
    trig(btns[0], start_color_index, &active[0]);
    while(true){
        int c = getch();
        int des = 0, prev = cur_btn;

        if(c == KEY_RIGHT || c == KEY_DOWN){
            cur_btn = (cur_btn + 1) % btn_counts;
            des = 1;
        }else if(c == KEY_LEFT || c == KEY_UP){
            cur_btn = (cur_btn + btn_counts - 1) % btn_counts;
            des = 1;
        }else if(c == 10 || c == 'c'){
            return cur_btn;
        }else   //Ignore others
            continue;

        if(!des)
            continue;

        //deselect current button
        trig(btns[prev], start_color_index + prev, &active[prev]);

        //select new button
        trig(btns[cur_btn], start_color_index + cur_btn, &active[cur_btn]);
    }
}