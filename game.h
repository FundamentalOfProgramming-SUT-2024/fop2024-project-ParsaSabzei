#ifndef GAME_H
#define GAME_H
#include "utils.h"

#define N 200

const int inf = 1e9;

//Parameters which determine game difficulty
int min_deamons, max_deamons;
int min_fire_breathing, max_fire_breathing;
int min_giant, max_giant;
int min_snake, max_snake;
int min_undeed, max_undeed;
int min_food, max_food;
int secs_to_hunger;
int secs_increase_heart;
int secs_to_stand_spell;
int init_hunger, init_health;

//higher value force snake to move mroe optimal
#define SNAKE_RANDOMNESS_MOVE 16

#define weapon_page_size 20

//x in 10
#define DAGGER_PROB 5

//x in 100
#define SWORD_PROB 10

//x in 100
#define ARROW_PROB 20

//x in 100  
#define MAGIC_WAND_PROB 10

//x in 100
#define SPELL_PROB 50

#define SPELLS_IN_ENCHANT_ROOM 20

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

#define traps_inc_in_treasure_room 10

//x in 100
#define HIDDEN_DOOR_PROB 10

// x in 10
#define black_gold_prob 1

#define number_of_room_min 4
#define number_of_rooms_max 5

// room width and height include border
#define room_width_min 10
#define room_width_max 30
#define room_height_min 10
#define room_height_max 15

#define rooms_min_distancex 5
#define rooms_min_distancey 4

#define enchant_room_size 20

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
    Black_Gold,
    Food,
    Endpoint,
    HiddenDoor
};

enum MonsterType{
    Deamon,
    Fire_Breathing,
    Giant,
    Snake,
    Undeed
};

#define WEAPON_TYPES 5
enum WeaponType{
    Mace,
    Dagger,
    Magic_Wand,
    Arrow,
    Sword,
    NoWeapon
};

#define SPELL_TYPES 3
enum SpellType{
    Health, 
    Speed,
    Damage
};

int weapon_damage[] = {5, 12, 15, 5, 10, 0};
char* weapon_name[] = {"Mace", "Dagger", "Magic_Wand", "Arrow", "Sword", "No weapon"};
char* weapon_icon[] = {"⚒", "🗡️", "🪄", "🏹", "⚔️", "𝕏"};
int weapon_count_per_collect[] = {1, 10, 8, 12, 1, 1};
int monsters_health[] = {5, 10, 15, 20, 30};
int monsters_damage[] = {1, 2, 3, 4, 6};
char* monsters_name[] = {"Deamon Monster", "Fire breathing Monster", "Giant Monster", "Snake", "Undeed Monster"};
int followings[] = {inf, inf, 3, inf, 5};

char* spell_name[] = {"Health", "Speed", "Damage"};
char* spell_icon[] = {"🧪", "🌿", "🔮"};

typedef struct Spell{
    enum SpellType type;
    int count;
    int x, y;
    int active;
} Spell;

typedef struct Weapon{
    enum WeaponType type;
    int count;
    int x,y;
    int active;
} Weapon;

typedef struct Monster{
    enum MonsterType type;
    int x, y;
    int health;
    int active;
    int following;
    int number_of_move_following;
} Monster;

typedef struct Bag
{
    Weapon* weapons[N];
    Spell* spells[N];
    int foods;
} Bag;

typedef struct Pair{
    int x;
    int y;
} Pair;

typedef struct Player{
    int x, y;
    int health;
    int Golds;
    int black_golds;
    int weapon;
    int dir;
    int hunger;
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

typedef struct EnchantRoom{
    Room* room; //Need to save
    Spell* Spells_In_Enchant_room[N]; //Need to save;
    int endpoint_x, endpoint_y;
} EnchantRoom;

typedef struct Tabaghe{
    int number_of_rooms;
    Room** rooms;
    EnchantRoom* enchant_room; //need to save

    Point* map[N][N];
    int monster_count;
    Monster* monsters[N];
    int weapon_count;
    Weapon* weapons_on_ground[N];
    int spell_count;
    Spell* spells_on_ground[N];
} Tabaghe;

typedef struct Game{
    int floor_count;
    int cf; // current floor
    int ignore_hiding;
    int ignore_picking;
    Player* player;
    Tabaghe** floors;
    Bag* bag;
    int active_spells[N]; 
} Game;


Game* game;
WINDOW* status_win;
WINDOW* bag_win, *weapon_win, *spell_win, *food_win;
Settings setting;
time_t last_time_attacked;
time_t last_time_spells[SPELL_TYPES];
int treasure_room;

char* GOLD_UNICODE = "\u2756";
char* BLACK_GOLD_UNICODE = "\u25c8";
char* ARROW_UP_UNICODE = "\u2191";
char* ARROW_DOWN_UNICODE = "\u2193";
char* MAGIC_WAND_BULLET = "✹";
char* ARROW_UNICODE = "⚆";
char* FOOD_UNICODE = "🍖";
char* ENDPOINT_UNICODE = "🕊️";

int basic_colors[N];
int dx[] = {+1, 0, -1, 0, 0}, dy[] = {0, -1, 0, +1, 0};
int dxx[8] = {+1, -1, 0, 0, +1, +1, -1, -1}, dyy[8] = {0, 0, +1, -1, +1, -1, +1, -1};

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
void draw_endpoint(int, int);
void draw_hidden_door(int, int);
void draw_deamon(int, int);
void draw_undeed(int, int);
void draw_fire_breathing(int, int);
void draw_giant(int, int);
void draw_weapons(int);
void draw_spell_room(int);
int get_room_from_point(int, int, int);
void activate_room(int, int);
void activate_corridor(int, int, int);
int dist(int, int, int, int);
void generate_traps(int);
void generate_floor(int, int);
void set_treasure_room();
void generate_stair_up(int);
void generate_dagger(int);
void generate_fire_breathing(int);
void generate_demon(int);
void generate_giant(int);
void generate_golds(int);
void generate_black_golds(int);
void generate_hidden_doors(int);
void generate_deamon(int);
void generate_magic_wand(int);
void generate_sword(int);
void generate_arrow(int);
void generate_snake(int);
void generate_undeed(int);
void generate_food(int);
void generate_spell(int, enum SpellType);
void generate_enchant_room(int);

void move_to_floor(int);
void init_status_bar();
void show_message(char*, char*);
void clear_buttom_status_bar();
void draw_monsters(int);
int is_monster_on(int, int, int);
int is_weapon_on(int, int, int);
void show_bag();
void show_weapons_page();
void show_food_page();
void activate_monster(int, int);
void activate_weapon(int, int);
void activate_spell(int, int);
void pick_weapon(int);
void kill_monster(int, int);
void remove_weapon(int, int);
void enter_new_room(int, int);
void set_game_difficulty();
void lose();
void ywin();

int is_monster_on(int id, int nx, int ny){
    for(int i = 0; i < game->floors[id]->monster_count; i++)
        if(game->floors[id]->monsters[i]->x == nx && game->floors[id]->monsters[i]->y == ny)
            return i;
    return -1;
}
int is_spell_on(int id, int nx, int ny){
    for(int i = 0; i < game->floors[id]->spell_count; i++)
        if(game->floors[id]->spells_on_ground[i]->x == nx && game->floors[id]->spells_on_ground[i]->y == ny)
            return i;
    return -1;
}
int is_weapon_on(int id, int nx, int ny){
    for(int i = 0; i < game->floors[id]->weapon_count; i++)
        if(game->floors[id]->weapons_on_ground[i]->x == nx && game->floors[id]->weapons_on_ground[i]->y == ny)
            return i;
    return -1;
}
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
void draw_hidden_door(int i, int j){
    if(game->floors[game->cf]->map[i][j]->activated == 0){
        attron(COLOR_PAIR(ROOM_INSIDE_COLOR));
        mvprintw(i, j, "0");
        attroff(COLOR_PAIR(ROOM_INSIDE_COLOR));
    }else{
        attron(COLOR_PAIR(TRAP_COLOR));
        mvprintw(i, j, "]");
        attroff(COLOR_PAIR(TRAP_COLOR)); 
    }
}
void draw_gold(int i, int j){
    attron(COLOR_PAIR(basic_colors[1]));
    mvaddstr(i, j, GOLD_UNICODE);
    attroff(COLOR_PAIR(basic_colors[1]));
}
void draw_endpoint(int i, int j){
    mvaddstr(i, j, ENDPOINT_UNICODE);
}
void draw_deamon(int i, int j){
    attron(COLOR_PAIR(basic_colors[0]));
    mvaddstr(i, j, "D");
    attroff(COLOR_PAIR(basic_colors[0]));
}
void draw_undeed(int i, int j){
    attron(COLOR_PAIR(7));
    mvaddstr(i, j, "U");
    attroff(COLOR_PAIR(7));
}
void draw_fire_breathing(int i, int j){
    attron(COLOR_PAIR(basic_colors[1]));
    mvaddstr(i, j, "F");
    attroff(COLOR_PAIR(basic_colors[1]));
}
void draw_giant(int i, int j){
    attron(COLOR_PAIR(basic_colors[2]));
    mvaddstr(i, j, "G");
    attroff(COLOR_PAIR(basic_colors[2]));
}
void draw_snake(int i, int j){
    mvaddstr(i, j, "S");
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
void draw_food(int i, int j){
    mvprintw(i, j, "%s", "~");
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
        case Food:
            draw_food(i, j);
            break;
        case Endpoint:
            draw_endpoint(i, j);
            break;
        case HiddenDoor:
            draw_hidden_door(i, j);
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
    activate_monster(i, id);
    activate_weapon(i, id);
    activate_spell(i, id);
}
//get current room of jth monster in the id floor
int get_monster_room(int j, int id){
    int x = game->floors[id]->monsters[j]->x;
    int y = game->floors[id]->monsters[j]->y;
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int stx = game->floors[id]->rooms[i]->startx, enx = game->floors[id]->rooms[i]->h + stx;
        int sty =  game->floors[id]->rooms[i]->starty, eny = game->floors[id]->rooms[i]->w + sty;
        if(x >= stx && x < enx && y >= sty && y < eny){
            return i;
        }
    }
    return -1;
}
//get current room of jth weapon in the id floor
int get_weapon_room(int j, int id){
    int x = game->floors[id]->weapons_on_ground[j]->x;
    int y = game->floors[id]->weapons_on_ground[j]->y;
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int stx = game->floors[id]->rooms[i]->startx, enx = game->floors[id]->rooms[i]->h + stx;
        int sty =  game->floors[id]->rooms[i]->starty, eny = game->floors[id]->rooms[i]->w + sty;
        if(x >= stx && x < enx && y >= sty && y < eny){
            return i;
        }
    }
    return -1;
}
//get current room of jth spell in the id floor
int get_spell_room(int j, int id){
    int x = game->floors[id]->spells_on_ground[j]->x;
    int y = game->floors[id]->spells_on_ground[j]->y;
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int stx = game->floors[id]->rooms[i]->startx, enx = game->floors[id]->rooms[i]->h + stx;
        int sty =  game->floors[id]->rooms[i]->starty, eny = game->floors[id]->rooms[i]->w + sty;
        if(x >= stx && x < enx && y >= sty && y < eny){
            return i;
        }
    }
    return -1;
}
//Activating monsters entering the ith room
void activate_monster(int i, int id){
    for(int j = 0; j < game->floors[id]->monster_count; j++)
        if(get_monster_room(j, id) == i)
            game->floors[id]->monsters[j]->active = 1;
}
void activate_weapon(int i, int id){
    for(int j = 0; j < game->floors[id]->weapon_count; j++)
        if(get_weapon_room(j, id) == i)
            game->floors[id]->weapons_on_ground[j]->active = 1;
}
void activate_spell(int i, int id){
    for(int j = 0; j < game->floors[id]->spell_count; j++)
        if(get_spell_room(j, id) == i)
            game->floors[id]->spells_on_ground[j]->active = 1;
}
void activate_corridor(int x, int y, int id){
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            if(game->floors[id]->map[i][j]->type == Corridor && dist(x, y, i, j) <= HOW_FAR_SEE)
                game->floors[id]->map[i][j]->discovered = 1;
}
void show_message(char*msg, char*emj){
    clear_buttom_status_bar();
    attron(A_ITALIC);
    mvprintw(LINES - 1, (COLS - strlen(msg))/2, "%s %s", msg, emj);
    attroff(A_ITALIC);

}
void kill_monster(int id, int ind){
    int x = game->floors[id]->monsters[ind]->x, y = game->floors[id]->monsters[ind]->y;

    //redraw the position
    draw_item(x, y, game->floors[game->cf]->map[x][y]->type);

    // If a weapon was in (nx, ny) redraw it
    draw_weapons(game->cf);

    for(int i = ind + 1; i < game->floors[id]->monster_count; i++)
        game->floors[id]->monsters[i - 1] = game->floors[id]->monsters[i];
    game->floors[id]->monster_count--;

}
//Remove indth weapon on ground on the idth floor
void remove_weapon(int id, int ind){
    int x = game->floors[id]->weapons_on_ground[ind]->x, y = game->floors[id]->weapons_on_ground[ind]->y;
    for(int i = ind + 1; i < game->floors[id]->weapon_count; i++)
        game->floors[id]->weapons_on_ground[i - 1] = game->floors[id]->weapons_on_ground[i];
    game->floors[id]->weapon_count--;
}
//Remove indth spell on ground on the idth floor
void remove_spell(int id, int ind){
    int x = game->floors[id]->spells_on_ground[ind]->x, y = game->floors[id]->spells_on_ground[ind]->y;
    for(int i = ind + 1; i < game->floors[id]->spell_count; i++)
        game->floors[id]->spells_on_ground[i - 1] = game->floors[id]->spells_on_ground[i];
    game->floors[id]->spell_count--;
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