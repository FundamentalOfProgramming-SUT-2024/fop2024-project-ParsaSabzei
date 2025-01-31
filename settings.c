#include <ncurses.h>
#include "settings.h"

void settings_draw_main_screen(WINDOW*, char*);
void settings_handle_input();


Settings setting;
WINDOW* bts[6];
char* username;
int btn_cnt = 6;
int colors[6];

void settings_draw_main_screen(WINDOW* main, char* _username){
    username = _username;

    setting = load_settings(username);

    int sword_heart_posx = 3, sword_heart_posy = 3;
    int rogy_posx = 3, rogy_posy = COLS/2 - 15;
    int bomb_posx = 4, bomb_posy = COLS - 20;

    //draw border of main
    box(main, 0, 0);

    //draw sword heart
    WINDOW* color_win = newwin(10, COLS - 4, 2, 2);
    box(color_win, 0, 0);

    char* color = "Red";
    if(setting.Player_Color == 1)
        color = "Yellow";
    else if(setting.Player_Color == 2)
        color = "Blue";

    mvwprintw(color_win, 0, 3, "Current Player Color: %s", color);
    refresh();
    wrefresh(color_win);

    colors[0] = red(), colors[1] = yellow(), colors[2] = blue();

    WINDOW* red_btn = create_button(5, COLS/3 - 18, 5, 13, colors[0], "Red");
    WINDOW* yellow_btn = create_button(5, COLS/3 - 18, 5, 26 + COLS/3 - 18, colors[1], "Yellow");
    WINDOW* blue_btn = create_button(5, COLS/3 - 18, 5, 39 + 2*(COLS/3 - 18), colors[2], "Blue");

    WINDOW* dif_win = newwin(10, COLS - 4, 12, 2);
    box(dif_win, 0, 0);

    char* difi = "Easy";
    if(setting.Difficulty == 1)
        difi = "Normal";
    else if(setting.Difficulty == 2)
        difi = "Hard";
        
    mvwprintw(dif_win, 0, 3, "Current Difficulity: %s", difi);
    refresh();
    wrefresh(dif_win);

    for(int i = 3; i < 6; i++)
        colors[i] = i + 2;
    WINDOW* easy_btn = create_button(5, COLS/3 - 18, 15, 13, colors[3], "Easy");
    WINDOW* normal_btn = create_button(5, COLS/3 - 18, 15, 26 + COLS/3 - 18, colors[4], "Normal");
    WINDOW* hard_btn = create_button(5, COLS/3 - 18, 15, 39 + 2*(COLS/3 - 18), colors[5], "Hard");
 
    bts[0] = red_btn, bts[1] = yellow_btn, bts[2] = blue_btn;
    bts[3] = easy_btn, bts[4] = normal_btn, bts[5] = hard_btn;

    settings_handle_input();
}

 int curr_btn = 0;

// 0 -> create new game
// 1 -> resume game
// 2 -> settings
// 3 - > scoreboard
void settings_handle_input(){
    int active[btn_cnt];
    for(int i = 0; i < btn_cnt; i++)
        active[i] = 1;
    trig(bts[0], colors[0], &active[0]);
    while(true){
        int c = getch();
        int des = 0, prev = curr_btn;

        if(c == KEY_RIGHT || c == KEY_DOWN){
            curr_btn = (curr_btn + 1) % btn_cnt;
            des = 1;
        }else if(c == KEY_LEFT || c == KEY_UP){
            curr_btn = (curr_btn + btn_cnt - 1) % btn_cnt;
            des = 1;
        }else if(c == 10 || c == 'c'){
            if(curr_btn >= 3)
                setting.Difficulty = curr_btn - 3;
            else
                setting.Player_Color = curr_btn;
            save_settings(setting, username);
            return;
        }else   //Ignore others
            continue;

        if(!des)
            continue;

        //deselect current button
        trig(bts[prev], colors[prev], &active[prev]);

        //select new button
        trig(bts[curr_btn], colors[curr_btn], &active[curr_btn]);
    }
} 