void loginuser_draw_main_screen();
char* loginuser_handle_uinput();
char* uvalidation();
void throw_error(char*);

#define uinp_count 4
#define uinp_padding 5
#define distance_between_uuinps 10

#define uconfirm_btn_height 8
#define uconfirm_btn_width (COLS/2 - 10)
#define uconfirm_btn_posx (LINES - 20)
#define uconfirm_btn_posy (COLS/2 - 35)
#define uconfirm_btn_color 5

#define guest_btn_height 7
#define guest_btn_width (20)
#define guest_btn_color 6
#define guest_btn_posx (LINES - 20)
#define guest_btn_posy (COLS/2 - 10)

Input* uuinps[uinp_count];
WINDOW* uconfirm_btn, *guest_btn;
int uconfirm_status = 1, guest_btn_status = 1;
char uuinps_content[uinp_count][100];

void loginuser_draw_main_screen(WINDOW* main){
    char* msg[] = {
        "Username: ",
        "Password: "
    };

    //init
    curs_set(true);

    box(main, 0, 0);

    //Info
    move(2, COLS/2 - 18);
    printw("Please enter your information");

    refresh();
    // uinputs
    for(int i = 0; i < uinp_count - 2; i++)
        uuinps[i] = create_input(5, COLS-10, distance_between_uuinps * i + 5, 3, uinp_padding, msg[i]);

    // uconfirm button
    uconfirm_btn = newwin(uconfirm_btn_height, uconfirm_btn_width, uconfirm_btn_posx, uconfirm_btn_posy);
    wattron(uconfirm_btn, COLOR_PAIR(uconfirm_btn_color));
    box(uconfirm_btn, 0, 0);
    for(int i = 0; i < confirm_len; i++){
        wmove(uconfirm_btn, i+1, uconfirm_btn_width/2 - 20);
        wprintw(uconfirm_btn, "%s", confirm[i]);
    }
    refresh();
    wrefresh(uconfirm_btn);

    // guest button
    guest_btn = newwin(guest_btn_height, guest_btn_width, guest_btn_posx+10, guest_btn_posy);
    wattron(guest_btn, COLOR_PAIR(guest_btn_color));
    box(guest_btn, 0, 0);
    mvwprintw(guest_btn, 3, 3, "Login as guest");
    refresh();
    wrefresh(guest_btn);

    //move cursor to first input
    int x, y;
    getyx(uuinps[0]->win, x, y);
    move(7, y+3);
}

void throw_error(char* err){
    //Erase Previous Error
    move(1, 0);
    for(int i = 0; i < COLS; i++)
        printw(" ");
    int i = red();
    attron(COLOR_PAIR(i));
    mvprintw(1, COLS/2-strlen(err)/2, "%s", err);
    attroff(COLOR_PAIR(i));
}
char* loginuser_handle_uinput(){
    int cur_input = 0;
    while(true){
        int ch = getch();
        
        //Ignore ENTER when btn is not selected
        if(ch == 10 && cur_input < 2)
            continue;

        if(uinp_count - 2 == cur_input){
            if(ch == 10 || ch == 'c'){
                char*res = uvalidation();
                if(!strcmp(res, "ok")){
                    //Pass username
                    return uuinps[0]->content;
                }else {  
                    throw_error(res);
                    continue;
                }
            }
            if(ch != KEY_UP && ch != KEY_DOWN)
                continue;
            trig(uconfirm_btn, uconfirm_btn_color, &uconfirm_status);
            curs_set(true);
            echo();
        }else if(cur_input == uinp_count - 1){
            if(ch == 10 || ch == 'c')
                 return "Guest";
            if(ch != KEY_UP && ch != KEY_DOWN)
                continue;
            trig(guest_btn, guest_btn_color, &guest_btn_status);
            curs_set(true);
            echo();
        }
  
        if(ch == KEY_UP){
            cur_input = (cur_input + uinp_count - 1) % uinp_count;
        }else if(ch == KEY_DOWN){
            cur_input = (cur_input + 1) % uinp_count;
        }else{
            write_inp(uuinps[cur_input], ch, uinp_padding);
        }

        if(cur_input == uinp_count - 2){
            trig(uconfirm_btn, uconfirm_btn_color, &uconfirm_status);
            noecho();
            curs_set(false);
            continue;
        }else if(cur_input == uinp_count - 1){
            trig(guest_btn, guest_btn_color, &guest_btn_status);
            noecho();
            curs_set(false);
            continue;
        }
        int x, y;
        getyx(uuinps[cur_input]->win, x, y);
        move(distance_between_uuinps * cur_input + 7, y+3);
    }
}

char* uvalidation(){
    char* username = uuinps[0]->content;
    char* password = uuinps[1]->content;
    if(strlen(username) * strlen(password) == 0)
        return "Please fill out the uinputs...";
    if(!user_exists(username))
        return "Username Notfound";
    if(!match_password(username, password))
        return "Password is not correct";
    return "ok";
}