#include<ncurses.h>
#include "game.h"

char *username;

void init_game(WINDOW* main, char*_username){
    username = _username;

    //init
    clear();
    noecho();
    curs_set(false);

    game = (Game*)malloc(sizeof(Game));
    game->player = (Player*)malloc(sizeof(Player));
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++){
            game->map[i][j] = (Point*)malloc(sizeof(Point));
            game->map[i][j]->discovered = 0;
            game->map[i][j]->activated = 0;
            game->map[i][j]->type = Nothing;
        }
    generate_rooms();

    //Player initial place
    game->player->x = game->rooms[0]->startx + game->rooms[0]->h/2;
    game->player->y = game->rooms[0]->starty + game->rooms[0]->w/2;
    activate_room(0);

    generate_corridors();    
    generate_pillar();
    generate_traps();

    draw_map();
    handle_input();
}

void draw_map(){
    clear();
    for(int i = 0; i < game->number_of_rooms; i++)
        if(game->rooms[i]->discovered)
            draw_room(game->rooms[i]);
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++){
            if(game->map[i][j]->discovered == 0)
                continue;
            draw_item(i, j, game->map[i][j]->type);
            refresh();
        }
    draw_player();
}
void handle_input(){
    while(true){
        int ch = getch();
        int x = game->player->x, y = game->player->y;
        int nx = x, ny = y;
        if(ch == 'w'){
            nx--;
        }else if(ch == 's'){
            nx++;
        }else if(ch == 'a'){
            ny--;
       }else if(ch == 'd'){
            ny++;
        }else if(ch == 'q'){
            nx--, ny--;
        }else if(ch == 'e'){
            nx--, ny++;
        }else if(ch == 'z'){
            nx++, ny--;
        }else if(ch == 'c'){
            nx++, ny++;
        }
        if(valid(nx, ny) && game->map[nx][ny]->type != Nothing && game->map[nx][ny]->type != Wall
            && game->map[nx][ny]->type != Pillar){
            draw_item(x, y, game->map[x][y]->type);
            game->player->y = ny;
            game->player->x = nx;
            draw_player();

            //Update visibility
            int current_room = get_room_from_point(nx, ny);
            if(current_room != EOF && game->rooms[current_room]->discovered == 0){
                activate_room(current_room);
                draw_map();
            }
            if(game->map[nx][ny]->type == Corridor && game->map[nx][ny]->discovered == 0){
                activate_corridor(nx, ny);
                draw_map();
            }
            if(game->map[nx][ny]->type == Trap){
                //Trap activated
                game->map[nx][ny]->activated = 1;
            }
        }
    }
}
void generate_rooms(){
    refresh();
    int room_count = random_num(number_of_room_min, number_of_rooms_max);
    game->number_of_rooms = 0;
    game->rooms = (Room**)malloc(sizeof(Room*) * room_count);
    int i = 0;
    while(i < room_count){
        int w = random_num(room_width_min, room_width_max);
        int h = random_num(room_height_min, room_height_max);
        int startx = random_num(Padding, LINES - Padding);
        int starty = random_num(Padding, COLS - Padding);
        if(!check_room_overlap(startx, starty, w, h)){
            game->rooms[i] = create_room(startx, starty, w, h);
            //edit game
            for(int j = 0; j < w; j++)
                for(int z = 0; z < h; z++){
                    if(j == 0 || j == w - 1 || z == 0 || z == h - 1)
                        game->map[startx + z][starty + j]->type = Wall;
                    else
                        game->map[startx + z][starty + j]->type = Floor;
                }
            game->number_of_rooms++;
            i++;
        }
    }
    
}
void generate_pillar(){
    for(int i = 0; i < game->number_of_rooms; i++){
        int j = random_num(number_of_pillar_min,number_of_pillar_max);
        while(j--){
            do{
                int x = rand() % (game->rooms[i]->h-4) + game->rooms[i]->startx + 2;
                int y = rand() % (game->rooms[i]->w-4) + game->rooms[i]->starty + 2;
                if(game->map[x][y]->type != Floor)
                    continue;
                game->map[x][y]->type = Pillar;
            }while(!is_game_connected());
        }
    }
}
void generate_traps(){
    for(int i = 0; i < game->number_of_rooms; i++){
        int j = random_num(number_of_traps_min,number_of_traps_max);
        while(j--){
            do{
                int x = rand() % (game->rooms[i]->h-4) + game->rooms[i]->startx + 2;
                int y = rand() % (game->rooms[i]->w-4) + game->rooms[i]->starty + 2;
                if(game->map[x][y]->type != Floor)
                    continue;
                game->map[x][y]->type = Trap;
            }while(!is_game_connected());
        }
    }
}
int check_room_overlap(int startx, int starty, int w, int h){
    if(startx + h - 1 >= LINES-2 || starty + w - 1 >= COLS-2)
        return 1;
    for(int i = 0; i < game->number_of_rooms; i++){
        int stx = game->rooms[i]->startx, wx = game->rooms[i]->w;
        int sty = game->rooms[i]->starty, hx = game->rooms[i]->h;
        int xcolid = 1, ycolid = 1;
        if(startx > stx + hx - 1 || startx + h - 1 < stx)
            xcolid = 0;
        if(starty > sty + wx - 1 || starty + w - 1 < sty)
            ycolid = 0; 

        // Room shouldn't be too close
        if(abs(startx - (stx + hx - 1)) <= rooms_min_distancex || abs(startx + h - 1 - stx) <= rooms_min_distancex)
            return 1;
        if(abs(starty - (sty + wx - 1)) <= rooms_min_distancey || abs(starty + w - 1 - sty) <= rooms_min_distancey)
            return 1;
        if(xcolid && ycolid)
            return 1;
    }
    return 0;
}
void generate_corridors(){
    int same[N][N];
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++) same[i][j] = 0;

    while(!is_game_connected()){
        int mmn = inf, mx = 0;
        for(int i = 0; i < game->number_of_rooms; i++){
            if(same[i][i] < mmn)
                mmn = same[i][i];
            if(same[i][i] > mx)
                mx = same[i][i];
        }
        int u = rand() % game->number_of_rooms;
        if(same[u][u] != mmn && mx > mmn)
            continue;

        int mn = inf, v = 0;
        for(int i = 0; i < game->number_of_rooms; i++)
            if(u != i && bfs(u, i) < mn && !same[u][i])
                v = i, mn = bfs(u, i);
        if(mn == inf)
            continue;
        same[u][u]++;

        same[u][v] = same[v][u] = 1;
        make_corridor(u, v);
        refresh();
    }
}
void make_corridor(int u, int v){
    int queue[N * N];
    int dist[N][N], par[N][N];
   int sx, sy; 
    for(int i = 0; i < game->rooms[u]->h; i++)
        for(int j = 0; j < game->rooms[u]->w; j++)
            if(game->map[i + game->rooms[u]->startx][j+game->rooms[u]->starty]->type == Floor)
                sx = i + game->rooms[u]->startx, sy = j+game->rooms[u]->starty;
    memset(dist, 63, sizeof(dist));
    dist[sx][sy] = 0, queue[0] = sx * N + sy;
    par[sx][sy] = -1;
    int f = 0, qsz = 1;

    while(f < qsz){
        int x = queue[f] / N, y = queue[f] % N;
        f++;
        for(int j = 0; j < CORRIDOR_RANDOMNESS; j++){
            int i = rand() % 4;
            int nx = dx[i] + x, ny = dy[i] + y;
            if(dist[nx][ny] > dist[x][y] + 1 && nx >= Padding && ny >= Padding && nx < LINES - Padding && ny < COLS - Padding){
                // Prevent moving through walls
                if(game->map[x][y]->type==Wall && game->map[nx][ny]->type==Wall)
                    continue;
                dist[nx][ny] = dist[x][y] + 1;
                par[nx][ny] = x * N + y;
                queue[qsz++] = nx * N + ny;
            }
        }
    }
    int distx = game->rooms[v]->startx + game->rooms[v]->h/2;
    int disty = game->rooms[v]->starty + game->rooms[v]->w/2;

    while(~par[distx][disty]){

        //Draw corridors
        if(game->map[distx][disty]->type != Floor && game->map[distx][disty]->type!=Pillar){
            if(game->map[distx][disty]->type == Wall || game->map[distx][disty]->type == Door){
               game->map[distx][disty]->type=Door;
            }else{                
                game->map[distx][disty]->type=Corridor;
            }
        }
        int p = par[distx][disty];
        distx = p / N, disty = p % N;
    }
}   
// get distance betweeen room u and v
int bfs(int u, int v){
    int sx = game->rooms[u]->startx + game->rooms[u]->h/2;
    int sy = game->rooms[u]->starty + game->rooms[u]->w/2;
    int dx = game->rooms[v]->startx + game->rooms[v]->h/2;
    int dy = game->rooms[v]->starty + game->rooms[v]->w/2;
    return abs(sx - dx) + abs(dy - sy);
}
int mark[N][N];
int is_game_connected(){
    memset(mark, 0, sizeof(mark));
    int sx, sy;
    for(int i = 0; i < game->rooms[0]->h; i++)
        for(int j = 0; j < game->rooms[0]->w; j++)
            if(game->map[i + game->rooms[0]->startx][j + game->rooms[0]->starty]->type == Floor)
                sx = i + game->rooms[0]->startx, sy = j + game->rooms[0]->starty;
    dfs(sx, sy);
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            if(game->map[i][j]->type != Nothing && game->map[i][j]->type != Pillar && !mark[i][j])
                return 0;
    return 1;
}
void dfs(int x, int y){
    mark[x][y] = 1;
    for(int i = 0; i < 4; i++){
        int nx = dx[i] + x, ny = y + dy[i];
        if(!mark[nx][ny] && nx >= 0 && ny >= 0 && nx < LINES && ny < COLS
             && game->map[nx][ny]->type != Nothing && game->map[nx][ny]->type != Pillar)
            dfs(nx, ny);
    }
}