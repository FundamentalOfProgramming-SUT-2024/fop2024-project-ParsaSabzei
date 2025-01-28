#define maxbuf (int)1e4
const char* users_file_path = "storage/user.csv";

int user_exists(char*);
void save_user(char*, char*, char*, char*);
int match_password(char*, char*);

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