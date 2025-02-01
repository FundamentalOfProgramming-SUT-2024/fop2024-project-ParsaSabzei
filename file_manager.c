#include "settings.h"
#include "player_info.h"
#include "game.h"

#define maxbuf (int)1e4
const char* users_file_path = "storage/user.csv";
const char* player_info_path = "storage/player_info/";

int user_exists(char*);
void save_user(char*, char*, char*, char*);
int match_password(char*, char*);
void save_settings(Settings, char*);
Settings load_settings(char*);
char* get_settings_file_path(char* username){
    char* path = "storage/settings";
    char* ans = (char*)malloc(strlen(path) + strlen(username) + 5);
    strcpy(ans, path);
    strcat(ans, username);
    strcat(ans, ".bin");
    return ans;
}
char* get_game_file_path(char* username){
    char* path = "storage/game";
    char* ans = (char*)malloc(strlen(path) + strlen(username) + 5);
    strcpy(ans, path);
    strcat(ans, username);
    strcat(ans, ".bin");
    return ans;
}
char* get_player_info_file_path(char* username){
    char* path = player_info_path;
    char* ans = (char*)malloc(strlen(path) + strlen(username) + 5);
    strcpy(ans, path);
    strcat(ans, username);
    strcat(ans, ".bin");
    return ans;
}
int user_count(){
    FILE* fp = fopen(users_file_path, "r");
    char buffer[maxbuf];
    int cnt = 0;
    while (fgets(buffer, maxbuf, fp)) 
        cnt++;
    fclose(fp);
    return cnt;
}
int user_exists(char*username){
    FILE* fp = fopen(users_file_path, "r");
    char buffer[maxbuf];
    while (fgets(buffer, maxbuf, fp)) {
        int column = 0;
        char* value = strtok(buffer, ", ");
        while (value) {
            if (column == 0 && !strcmp(value, username)) {
                return 1;
            }
            value = strtok(NULL, ", ");
            column++;
        }
    }
    fclose(fp);
    return 0;
}
char**get_all_users(){
    FILE* fp = fopen(users_file_path, "r");
    char buffer[maxbuf];
    int tot = user_count();
    char **ans = (char**)malloc(tot * sizeof(char*));
    while (fgets(buffer, maxbuf, fp)) {
        int column = 0;
        char* value = strtok(buffer, ", ");
        while (value) {
            if (column == 0) {
                ans[--tot] = malloc(sizeof(char) * strlen(value));
                strcpy(ans[tot], value);
            }
            value = strtok(NULL, ", ");
            column++;
        }
    }
    fclose(fp);
    return ans;
}
int match_password(char*username, char*password){
    FILE* fp = fopen(users_file_path, "r");
    char buffer[maxbuf];
    while (fgets(buffer, maxbuf, fp)) {
        int column = 0;
        char* value = strtok(buffer, ", ");
        int here = 0;
        while (value) {
            if (column == 0 && !strcmp(value, username)) 
                here = 1;
            if(column == 1 && here){
                if(!strcmp(value, password))
                    return 1;
                return 0;
            }
            value = strtok(NULL, ", ");
            column++;
        }
    }
    fclose(fp);
    return 0;
}
void save_info(char*username, PlayerInfo info){
    FILE* file = fopen(get_player_info_file_path(username), "wb");
    fwrite(&info, sizeof(info), 1, file);
    fclose(file);
}
PlayerInfo load_info(char*username){
    PlayerInfo set;
    FILE* file = fopen(get_player_info_file_path(username), "rb");
    fread(&set, sizeof(set), 1, file);
    fclose(file);
    return set;
}
void save_user(char* username, char*password, char*email, char*hint){
    PlayerInfo info;
    info.registration_time = time(NULL);
    info.total_golds = 0;
    info.total_scores = 0;
    info.total_game_played = 0;
    save_info(username, info);
    FILE* fp = fopen(users_file_path, "a");
    fprintf(fp, "%s, %s, %s, %s\n", username, password, email, hint);
    fclose(fp);
}
void save_settings(Settings set, char* username){
    FILE* file = fopen(get_settings_file_path(username), "wb");
    fwrite(&set, sizeof(set), 1, file);
    fclose(file);
}
Settings load_settings(char* username){
    Settings set;
    FILE* file = fopen(get_settings_file_path(username), "rb");
    if(file == NULL){
        set.Difficulty = 0, set.Player_Color = 0;
        return set;
    }
    fread(&set, sizeof(set), 1, file);
    fclose(file);
    return set;
}

void save_player(FILE* file, Player* player){
    fwrite(game->player, sizeof(Player), 1, file);
}
Player* load_player(FILE* file){
    Player *player = malloc(sizeof(Player));
    fread(player, sizeof(Player), 1, file);
    return player;
}
void save_room(FILE* file, Room* room){
    fwrite(&room->startx, sizeof(int), 1, file);
    fwrite(&room->starty, sizeof(int), 1, file);
    fwrite(&room->w, sizeof(int), 1, file);
    fwrite(&room->h, sizeof(int), 1, file);
    fwrite(&room->discovered, sizeof(int), 1, file);
}
Room* load_room(FILE *file) {
    Room *room = malloc(sizeof(Room));
    fread(&room->startx, sizeof(int), 1, file);
    fread(&room->starty, sizeof(int), 1, file);
    fread(&room->w, sizeof(int), 1, file);
    fread(&room->h, sizeof(int), 1, file);
    fread(&room->discovered, sizeof(int), 1, file);
    room->win = newwin(room->h, room->w, room->startx, room->starty);
    return room;
}
void save_tabaghe(FILE* file, Tabaghe* tabaghe){
    fwrite(&tabaghe->number_of_rooms, sizeof(int), 1, file);

    //Saving room
    for (int i = 0; i < tabaghe->number_of_rooms; i++) {
        save_room(file, tabaghe->rooms[i]);
    }

    //Saving tabaghe
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            fwrite(tabaghe->map[i][j], sizeof(Point), 1, file);
        }
    }
}
Tabaghe* load_tabaghe(FILE *file) {
    Tabaghe *tabaghe = malloc(sizeof(Tabaghe));
    fread(&tabaghe->number_of_rooms, sizeof(int), 1, file);

    tabaghe->rooms = malloc(tabaghe->number_of_rooms * sizeof(Room*));
    for (int i = 0; i < tabaghe->number_of_rooms; i++) {
        tabaghe->rooms[i] = load_room(file);
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            tabaghe->map[i][j] = malloc(sizeof(Point));
            fread(tabaghe->map[i][j], sizeof(Point), 1, file);
        }
    }
    return tabaghe;
}
void save_game(char* username, Game* game){
    FILE* file = fopen(get_game_file_path(username), "wb");
    fwrite(&game->floor_count, sizeof(int), 1, file);
    fwrite(&game->cf, sizeof(int), 1, file);
    fwrite(&game->ignore_hiding, sizeof(bool), 1, file);

    //Saving player
    save_player(file, game->player);
    
    //Saving tabaghe
    for(int i = 0; i < game->floor_count; i++){
        save_tabaghe(file, game->floors[i]);
    }
    fclose(file);
}
Game* load_game(char*username){
    FILE* file = fopen(get_game_file_path(username), "rb");
    Game *game = malloc(sizeof(Game));
    fread(&game->floor_count, sizeof(int), 1, file);
    fread(&game->cf, sizeof(int), 1, file);
    fread(&game->ignore_hiding, sizeof(bool), 1, file);

    game->floors = malloc(sizeof(Tabaghe*) * game->floor_count);
    //Loading player
    game->player = load_player(file);
    for(int i = 0; i < game->floor_count; i++){
        game->floors[i] = load_tabaghe(file);
    }
    fclose(file);
    return game;
}
