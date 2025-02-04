void newuser_draw_main_screen();
void newuser_handle_input();
char* validation();

#define inp_count 5
#define inp_padding 5
#define distance_between_inps 6 

#define confirm_btn_height 8
#define confirm_btn_width (COLS/2 - 10)
#define confirm_btn_posx (LINES - 10)
#define confirm_btn_posy (COLS/2 - 35)
#define confirm_btn_color 5

Input* inps[inp_count];
WINDOW* confirm_btn;
int confirm_status = 1;
char inps_content[inp_count][100];

void newuser_draw_main_screen(WINDOW* main){
    char* msg[] = {
        "Username: ",
        "Password: ",
        "Email: ",
        "What is your first pets's name? "
    };

    //init
    curs_set(true);
    box(main, 0, 0);

    //Info
    move(2, COLS/2 - 14);
    printw("Press 1 to generate password randomly");

    refresh();
    // Inputs
    for(int i = 0; i < inp_count - 1; i++)
        inps[i] = create_input(5, COLS-10, distance_between_inps * i + 5, 3, inp_padding, msg[i]);

    // Confirm button
    confirm_btn = newwin(confirm_btn_height, confirm_btn_width, confirm_btn_posx, confirm_btn_posy);
    wattron(confirm_btn, COLOR_PAIR(confirm_btn_color));
    box(confirm_btn, 0, 0);
    for(int i = 0; i < confirm_len; i++){
        wmove(confirm_btn, i+1, confirm_btn_width/2 - 20);
        wprintw(confirm_btn, "%s", confirm[i]);
    }

    refresh();
    wrefresh(confirm_btn);
}

int cur_input = 0;
void newuser_handle_input(){
    while(true){
        int ch = getch();

        if(ch == '1'){
            char*pass = random_password();
            for(int i = 0; i < strlen(pass); i++)
                write_inp(inps[1], pass[i], inp_padding);
            continue;
        }
        //Ignore ENTER when btn is not selected
        if(ch == 10 && inp_count != cur_input + 1)
            continue;

        if(inp_count == cur_input + 1){
            if(ch == 10 || ch == 'c'){
                char*res = validation();
                if(!strcmp(res, "ok")){
                    //Saving user's data
                    save_user(inps[0]->content, inps[1]->content, inps[2]->content, inps[3]->content);
                    return;
                }else {  
                    //Erase Previous Error
                    move(1, 0);
                    for(int i = 0; i < COLS; i++)
                        printw(" ");

                    //Show the error
                    int i = red();
                    attron(COLOR_PAIR(i));
                    mvprintw(1, COLS/2-strlen(res)/2, "%s", res);
                    attroff(COLOR_PAIR(i));
                    continue;
                }
            }
            if(ch != KEY_UP && ch != KEY_DOWN)
                continue;
            trig(confirm_btn, confirm_btn_color, &confirm_status);
            curs_set(true);
            echo();
        }
  
        if(ch == KEY_UP){
            cur_input = (cur_input + inp_count - 1) % inp_count;
        }else if(ch == KEY_DOWN){
            cur_input = (cur_input + 1) % inp_count;
        }else{
            write_inp(inps[cur_input], ch, inp_padding);
        }
        if(cur_input == inp_count - 1){
            trig(confirm_btn, confirm_btn_color, &confirm_status);
            noecho();
            curs_set(false);
            continue;
        }
        int x, y;
        getyx(inps[cur_input]->win, x, y);
        move(distance_between_inps * cur_input + 7, y+3);
    }
}

char* validation(){
    char* username = inps[0]->content;
    char* password = inps[1]->content;
    char* email = inps[2]->content;
    char* hint = inps[3]->content;
    if(strlen(username) * strlen(password) * strlen(email) * strlen(hint) == 0)
        return "Please fill out the inputs...";
    if(strlen(password) < 7)
        return "Password must has atleast 7 characters";
    int up = 0, lw = 0, dig = 0;
    for(int i = 0; i < strlen(password); i++){
        if(is_dig(password[i]))
            dig = 1;
        if(is_upper(password[i]))
            up = 1;
        if(is_lower(password[i]))
            lw = 1;
    }
    if(!dig)
        return "Password must has atleast one digit";
    if(!up)
        return "Password must has atleast one upper letter";
    if(!lw)
        return "Password must has atleast one lower letter";
    if(!regex_match(email, email_regex))
        return "Email form must be xxx@yyy.z";
    if(user_exists(username))
        return "A user already exists with this username";
    return "ok";
}