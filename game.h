#ifndef GAME_H
#define GAME_H

#define number_of_room_min 6
#define number_of_rooms_max 10

// room width and height include border
#define room_width_min 8
#define room_width_max 25
#define room_height_min 7
#define room_height_max 10

#define rooms_min_distancex 15
#define rooms_min_distancey 5

typedef struct Room{
    WINDOW* win;
    int startx, starty;
    int w, h;
} Room;

typedef struct Game{
    int number_of_rooms;
    Room** rooms;
} Game;

Game* game;

Room* create_room(int, int, int, int);
void init_game(WINDOW*, char*);
void fill(Room*);
int check_room_overlap(int, int, int, int);
void generate_rooms();

Room* create_room(int startx, int starty, int w, int h){
    Room* room = (Room*)malloc(sizeof(Room));
    room->win = newwin(h, w, starty, startx);
    //init window
    box(room->win, 0, 0);
    room->startx = startx, room->starty = starty;
    room->w = w, room->h = h;
    fill(room);
    return room;
}
void fill(Room* room){
    for(int i = 1; i < room->h - 1; i++)
        for(int j = 1; j < room->w - 1; j++)
            mvwprintw(room->win, i, j, ".");
}
#endif 