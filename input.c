typedef struct Input{
    WINDOW* win;
    char content[100];
} Input;

Input* create_input(int, int, int, int, int, char*);

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