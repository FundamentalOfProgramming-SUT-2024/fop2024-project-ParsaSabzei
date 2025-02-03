#include <ncurses.h>
#include "settings.h"

#include "simpletables.h"
#include "player_info.h"

void scoreboard_draw_main_screen(WINDOW*, char*);
void scoreboard_handle_input();

char* username;
const int COLUMN = 6;

//special names near the 3 best players' names
char* special_names[] = {"Champion", "Hero", "Master"};

void scoreboard_draw_main_screen(WINDOW* main, char* _username){
    username = _username;
    clear();
    st_table *table = st_create_table("Score board:", COLS, COLUMN);

    st_set_column(table, 0, "Rank", 0.167);
    st_set_column(table, 1, "Player", 0.169);
    st_set_column(table, 2, "Total score", 0.167);
    st_set_column(table, 3, "Total golds", 0.167);
    st_set_column(table, 4, "Total games played", 0.17);
    st_set_column(table, 5, "Exprience(hours)", 0.17);

    char** usernames = get_all_users();

    PlayerInfo info[N];
    int ids[N];

    int tot = user_count();
    for(int i = 0; i < tot; i++)
        info[i] = load_info(usernames[i]), ids[i] = i;

    for(int i = 0; i < tot; i++)
        for(int j = i + 1; j < tot; j++){
            if(info[ids[i]].total_scores < info[ids[j]].total_scores){
                swap(&ids[i], &ids[j]);
            }
        }
    for(int i = 0; i < tot; i++){
        char** c = malloc(COLUMN * sizeof(char*));
        c[0] = tostring(i + 1);
        c[1] = usernames[ids[i]];
        if(i < 3)
            c[1] = append(append(special_names[i], ": "), c[1]);
        c[2] = tostring(info[ids[i]].total_scores);
        c[3] = tostring(info[ids[i]].total_golds);
        c[4] = tostring(info[ids[i]].total_game_played);
        int secs = time(NULL) - info[ids[i]].registration_time;
        c[5] = append(tostring(secs / 3600), "h");
        st_add_row(table, c);
    }
    st_print(table);

    scoreboard_handle_input();

    getch();
}

void scoreboard_handle_input(){

} 