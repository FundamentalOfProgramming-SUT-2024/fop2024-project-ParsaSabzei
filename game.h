#ifndef GAME_H
#define GAME_H
#include "utils.h"

#define N 200

#define MIN_NUMBER_OF_FLOORS 4
#define MAX_NUMBER_OF_FLOORS 5

#define Padding 4

#define HOW_FAR_SEE 5

#define CORRIDOR_RANDOMNESS 5

#define number_of_pillar_min 0
#define number_of_pillar_max 2

#define number_of_gold_min 0
#define number_of_gold_max 8

#define number_of_traps_min 0
#define number_of_traps_max 1

// x in 10
#define black_gold_prob 1

#define number_of_room_min 8
#define number_of_rooms_max 5

// room width and height include border
#define room_width_min 8
#define room_width_max 25
#define room_height_min 7
#define room_height_max 12

#define rooms_min_distancex 5
#define rooms_min_distancey 4

enum type{
    Nothing,
    Wall,
    Floor,
    Corridor,
    Door,
    Pillar,
    Trap,
    StairUp,
    StairDown,
    Gold,
    Black_Gold
};



typedef struct Player{
    int x, y;
    int health;
    int Golds;
    int black_golds;
} Player;

typedef struct Point{
    enum type type;
    int discovered;
    int activated;
} Point;

typedef struct Room{
    WINDOW* win;
    int startx, starty;
    int w, h;
    int discovered;
} Room;

typedef struct Tabaghe{
    int number_of_rooms;
    Room** rooms;
    Point* map[N][N];
} Tabaghe;

typedef struct Game{
    int floor_count;
    int cf; // current floor
    Tabaghe** floors;
    Player* player;
    bool ignore_hiding;
} Game;



Game* game;
WINDOW* status_win;
Settings setting;

const char* GOLD_UNICODE = "\u2756";
const char* BLACK_GOLD_UNICODE = "\u25c8";
const char* ARROW_UP_UNICODE = "\u2191";
const char* ARROW_DOWN_UNICODE = "\u2193";

int basic_colors[N];
int dx[4] = {+1, -1, 0, 0}, dy[4] = {0, 0, +1, -1};
int inf = 1e9;

Room* create_room(int, int, int, int);
void init_game(WINDOW*, char*, int);
void fill(Room*);
int check_room_overlap(int, int, int, int, int);
void generate_rooms(int, int);
void generate_corridors();
int is_game_connected();
void dfs(int, int, int);
int bfs(int, int, int);
void make_corridor(int, int, int);
void handle_input();
void draw_item(int, int, enum type);
void draw_gold(int, int);
int valid(int, int);
void draw_player();
void generate_pillar();
void draw_map();
void draw_pillar(int, int);
void draw_corridor(int, int);
void draw_room(Room*);
void draw_stair_up(int, int);
void draw_stair_down(int, int);
int get_room_from_point(int, int, int);
void activate_room(int, int);
void activate_corridor(int, int, int);
int dist(int, int, int, int);
void generate_traps(int);
void generate_floor(int, int);
void generate_stair_up(int);
void generate_golds(int);
void generate_black_golds(int);
void move_to_floor(int);
void init_status_bar();
void show_message(char* msg);
void clear_buttom_status_bar();

Pair find_in_room(int room_id, int floor_id, enum type tp){
    int sx = game->floors[floor_id]->rooms[room_id]->startx, ex = game->floors[floor_id]->rooms[room_id]->h;
    int sy = game->floors[floor_id]->rooms[room_id]->starty, ey = game->floors[floor_id]->rooms[room_id]->w;
    for(int i = sx; i < sx + ex; i++)
        for(int j = sy; j < sy + ey; j++)
            if(game->floors[floor_id]->map[i][j]->type == tp){
                Pair p = {i, j};
                return p;
            }
    Pair p = {-1, -1};
    return p;
}
Room* create_room(int startx, int starty, int w, int h){
    Room* room = (Room*)malloc(sizeof(Room));
    room->win = newwin(h, w, startx, starty);
    room->startx = startx, room->starty = starty;
    room->w = w, room->h = h;
    room->discovered = 0;
    return room;
}
void draw_room(Room* room){
    wattron(room->win, COLOR_PAIR(ROOM_BORDER_COLOR));
    box(room->win, 0, 0);
    refresh();
    wrefresh(room->win);
    wattroff(room->win, COLOR_PAIR(ROOM_BORDER_COLOR));
    wattron(room->win, COLOR_PAIR(ROOM_INSIDE_COLOR));
    for(int i = 1; i < room->h - 1; i++)
        for(int j = 1; j < room->w - 1; j++)
            mvwprintw(room->win, i, j, ".");
    wattroff(room->win, COLOR_PAIR(ROOM_INSIDE_COLOR));
    wrefresh(room->win);
    refresh();

}
void draw_pillar(int i, int j){
    attron(COLOR_PAIR(PILLAR_COLOR));
    mvprintw(i, j, "O");
    attroff(COLOR_PAIR(PILLAR_COLOR));
}
void draw_corridor(int i, int j){
    attron(COLOR_PAIR(1));
    mvprintw(i, j, "#");
    attroff(COLOR_PAIR(1));
}
void draw_door(int i, int j){
    attron(COLOR_PAIR(DOOR_COLOR));
    mvprintw(i, j, "+");
    attroff(COLOR_PAIR(DOOR_COLOR));    
}
void draw_trap(int i, int j){
    if(game->floors[game->cf]->map[i][j]->activated == 0){
        attron(COLOR_PAIR(ROOM_INSIDE_COLOR));
        mvprintw(i, j, ".");
        attroff(COLOR_PAIR(ROOM_INSIDE_COLOR));
    }else{
        attron(COLOR_PAIR(TRAP_COLOR));
        mvprintw(i, j, "^");
        attroff(COLOR_PAIR(TRAP_COLOR)); 
    }
}
void draw_gold(int i, int j){
    attron(COLOR_PAIR(basic_colors[1]));
    mvaddstr(i, j, GOLD_UNICODE);
    attroff(COLOR_PAIR(basic_colors[1]));
}
void draw_black_gold(int i, int j){
    attron(COLOR_PAIR(BLACK_GOLD_COLOR) | A_BLINK);
    mvaddstr(i, j, BLACK_GOLD_UNICODE);
    attroff(COLOR_PAIR(BLACK_GOLD_COLOR) | A_BLINK);
}
void draw_stair_up(int i, int j){
    attron(COLOR_PAIR(STAIR_COLOR));
    mvprintw(i, j, "%s", ARROW_UP_UNICODE);
    attroff(COLOR_PAIR(STAIR_COLOR));    
}
void draw_stair_down(int i, int j){
    attron(COLOR_PAIR(STAIR_COLOR));
    mvprintw(i, j, "%s",ARROW_DOWN_UNICODE);
    attroff(COLOR_PAIR(STAIR_COLOR));    
}
void draw_item(int i, int j, enum type tp){
    switch(tp){
        case Corridor:
            draw_corridor(i, j);
            break;
        case Pillar:
            draw_pillar(i, j);
            break;
        case Door:
            draw_door(i, j);
            break;
        case Floor:
             attron(COLOR_PAIR(ROOM_INSIDE_COLOR));
             mvprintw(i, j, ".");
             attroff(COLOR_PAIR(ROOM_INSIDE_COLOR));
             break;
        case Trap:
            draw_trap(i, j);
            break;
        case StairUp:
            draw_stair_up(i, j);
            break;
        case StairDown:
            draw_stair_down(i, j);
            break;
        case Gold:
            draw_gold(i, j);
            break;
        case Black_Gold:
            draw_black_gold(i, j);
            break;
    }
}
void draw_player(){
    attron(COLOR_PAIR(basic_colors[setting.Player_Color]));
    mvprintw(game->player->x, game->player->y, "@");
    attroff(COLOR_PAIR(basic_colors[setting.Player_Color]));
}
int valid(int x, int y){
    return x >= 0 && y >= 0 && x < LINES && y < COLS;
}
int get_room_from_point(int x, int y, int id){
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int stx = game->floors[id]->rooms[i]->startx, enx = game->floors[id]->rooms[i]->h + stx;
        int sty =  game->floors[id]->rooms[i]->starty, eny = game->floors[id]->rooms[i]->w + sty;
        if(x >= stx && x < enx && y >= sty && y < eny)
            return i;
    }
    return EOF;
}
void activate_room(int i, int id){
    int stx = game->floors[id]->rooms[i]->startx, enx = game->floors[id]->rooms[i]->h + stx;
    int sty =  game->floors[id]->rooms[i]->starty, eny = game->floors[id]->rooms[i]->w + sty;
    game->floors[id]->rooms[i]->discovered = 1;
    for(int x = stx; x < enx; x++)
        for(int y = sty; y < eny; y++)
            game->floors[id]->map[x][y]->discovered = 1;
}
void activate_corridor(int x, int y, int id){
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            if(game->floors[id]->map[i][j]->type == Corridor && dist(x, y, i, j) <= HOW_FAR_SEE)
                game->floors[id]->map[i][j]->discovered = 1;
}
void show_message(char*msg){
    mvprintw(LINES - 1, (COLS - strlen(msg))/2, "%s", msg);
}
void clear_buttom_status_bar(){
    for(int i = 0; i < COLS; i++)
        mvprintw(LINES - 1, i, " ");
}
//Calculate distance between two point
int dist(int x, int y, int nx, int ny){
    return abs(x - nx) + abs(ny - y);
}

int min(int x, int y){
    return x > y ? y : x;
}
int max(int x, int y){
    return x > y ? x : y;
}
#endif 