#include<ncurses.h>
#include "game.h"

char *username;

void init_game(WINDOW* main, char*_username){
    username = _username;

    //init
    clear();
    noecho();
    curs_set(false);

    game = (Game*)malloc(sizeof(Game));
    generate_rooms();
}

void generate_rooms(){
    refresh();
    int room_count = random_num(number_of_room_min, number_of_rooms_max);
    game->number_of_rooms = 0;
    game->rooms = (Room**)malloc(sizeof(Room*) * room_count);
    for(int i = 0; i < room_count; i++){
        int w = random_num(room_width_min, room_width_max);
        int h = random_num(room_height_min, room_height_max);
        int startx = random_num(1, COLS - 1);
        int starty = random_num(1, LINES - 1);
        if(!check_room_overlap(startx, starty, w, h)){
            game->rooms[i] = create_room(startx, starty, w, h);
            wrefresh(game->rooms[i]->win);
            refresh();
            game->number_of_rooms++;
        }else 
            i--;
    }
}

int check_room_overlap(int startx, int starty, int w, int h){
    if(startx + w - 1 >= COLS-3 || starty + h - 1 >= LINES-2)
        return 1;
    for(int i = 0; i < game->number_of_rooms; i++){
        int stx = game->rooms[i]->startx, wx = game->rooms[i]->w;
        int sty = game->rooms[i]->starty, hx = game->rooms[i]->h;
        int xcolid = 1, ycolid = 1;
        if(startx > stx + wx - 1 || startx + w - 1 < stx)
            xcolid = 0;
        if(starty > sty + hx - 1 || starty + h - 1 < sty)
            ycolid = 0; 

        // Room shouldn't be too close
        if(abs(startx - (stx + wx - 1)) <= rooms_min_distancex || abs(startx + w - 1 - stx) <= rooms_min_distancex)
            return 1;
        if(abs(starty - (sty + hx - 1)) <= rooms_min_distancey || abs(starty + h - 1 - sty) <= rooms_min_distancey)
            return 1;
        if(xcolid && ycolid)
            return 1;
    }
    return 0;
}