typedef struct Input{
    WINDOW* win;
    char content[100];
} Input;

Input* create_input(int, int, int, int, int, char*);
WINDOW* create_button(int, int, int, int, int, char*);
void trig(WINDOW*, int, int*);

Input* create_input(int height, int width, int startx, int starty, int write_padding, char*caption){
    Input* inp = (Input*)malloc(sizeof(Input));
    WINDOW* win = newwin(height, width, startx, starty);
    box(win, 0, 0);
    mvwprintw(win, 0, 2, "%s", caption);
    wrefresh(win);
    wmove(win, 2, write_padding);
    refresh();
    inp->win = win;
    return inp;
}

// Backspace
void write_inp(Input* input, int ch, int write_padding){
    WINDOW* win = input->win;
    if(ch == KEY_BACKSPACE){
        int x, y;
        getyx(win, x, y);
        if(y == write_padding)
            return;
        wmove(win, x, y - 1);
        waddch(win, ' ');
        wmove(win, x, y - 1);
        wrefresh(win);
        input->content[strlen(input->content) - 1] = '\0';
        refresh();
    }else{
        waddch(win, ch);
        wrefresh(win);
        input->content[strlen(input->content)] = (char)ch;
        refresh();
    }
}

WINDOW* create_button(int button_height, int button_width, int button_start_posx,
                    int button_start_posy, int button_color_id, char* button_text){
    WINDOW* btn = newwin(button_height, button_width, button_start_posx, button_start_posy);
    wattron(btn, COLOR_PAIR(button_color_id));
    box(btn, 0, 0);
    mvwprintw(btn, button_height/2, (button_width-strlen(button_text))/2, "%s", button_text);
    refresh();
    wrefresh(btn);
    return btn;
}

void trig(WINDOW* btn, int color, int *active){
    if(*active == 1)
        init_pair(color + 5, 0, color);
    else    
        init_pair(color + 5, color, 0);
    wbkgd(btn, COLOR_PAIR(color + 5));
    *active ^= 1;
    wrefresh(btn);
    refresh();
}