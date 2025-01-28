#ifndef GAME_H
#define GAME_H


#define N 200

#define Padding 2

#define HOW_FAR_SEE 5

#define CORRIDOR_RANDOMNESS 5

#define number_of_pillar_min 0
#define number_of_pillar_max 2

#define number_of_traps_min 0
#define number_of_traps_max 1

#define number_of_room_min 6
#define number_of_rooms_max 8

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
    Trap
};

//  0
//3   1
//  2
typedef struct Player{
    int x, y;
    int dir;
    int health;
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

typedef struct Game{
    int number_of_rooms;
    Room** rooms;
    Point* map[N][N];
    Player* player;
} Game;



Game* game;
int dx[4] = {+1, -1, 0, 0}, dy[4] = {0, 0, +1, -1};
int inf = 1e9;

Room* create_room(int, int, int, int);
void init_game(WINDOW*, char*);
void fill(Room*);
int check_room_overlap(int, int, int, int);
void generate_rooms();
void generate_corridors();
int is_game_connected();
void dfs();
int bfs(int, int);
void make_corridor(int, int);
void handle_input();
void draw_item(int, int, enum type);
int valid(int, int);
void draw_player();
void generate_pillar();
void draw_map();
void draw_pillar(int, int);
void draw_corridor(int, int);
void draw_room(Room*);
int get_room_from_point(int, int);
void activate_room(int);
void activate_corridor(int, int);
int dist(int, int, int, int);
void generate_traps();

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
    wattroff(room->win, COLOR_PAIR(ROOM_BORDER_COLOR));
     wattron(room->win, COLOR_PAIR(ROOM_INSIDE_COLOR));
    for(int i = 1; i < room->h - 1; i++)
        for(int j = 1; j < room->w - 1; j++)
            mvwprintw(room->win, i, j, ".");
    wattroff(room->win, COLOR_PAIR(ROOM_INSIDE_COLOR));
    refresh();
    wrefresh(room->win);
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
    if(game->map[i][j]->activated == 0){
        attron(COLOR_PAIR(ROOM_INSIDE_COLOR));
        mvprintw(i, j, ".");
        attroff(COLOR_PAIR(ROOM_INSIDE_COLOR));
    }else{
        attron(COLOR_PAIR(TRAP_COLOR));
        mvprintw(i, j, "^");
        attroff(COLOR_PAIR(TRAP_COLOR)); 
    }
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
    }
}
void draw_player(){
     mvprintw(game->player->x, game->player->y, "@");
}
int valid(int x, int y){
    return x >= 0 && y >= 0 && x < LINES && y < COLS;
}
int get_room_from_point(int x, int y){
    for(int i = 0; i < game->number_of_rooms; i++){
        int stx = game->rooms[i]->startx, enx = game->rooms[i]->h + stx;
        int sty =  game->rooms[i]->starty, eny = game->rooms[i]->w + sty;
        if(x >= stx && x < enx && y >= sty && y < eny)
            return i;
    }
    return EOF;
}
void activate_room(int i){
    int stx = game->rooms[i]->startx, enx = game->rooms[i]->h + stx;
    int sty =  game->rooms[i]->starty, eny = game->rooms[i]->w + sty;
    game->rooms[i]->discovered = 1;
    for(int x = stx; x < enx; x++)
        for(int y = sty; y < eny; y++)
            game->map[x][y]->discovered = 1;
}
void activate_corridor(int x, int y){
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            if(game->map[i][j]->type == Corridor && dist(x, y, i, j) <= HOW_FAR_SEE)
                game->map[i][j]->discovered = 1;
}

//Calculate distance between two point
int dist(int x, int y, int nx, int ny){
    return abs(x - nx) + abs(ny - y);
}
#endif 