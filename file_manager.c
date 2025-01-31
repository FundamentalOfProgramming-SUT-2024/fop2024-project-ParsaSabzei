#include "settings.h"

#define maxbuf (int)1e4
const char* users_file_path = "storage/user.csv";

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
void save_user(char* username, char*password, char*email, char*hint){
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