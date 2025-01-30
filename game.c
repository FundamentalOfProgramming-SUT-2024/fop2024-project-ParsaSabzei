#include<ncurses.h>
#include "game.h"

char *username;

void generate_floor(int id, int stair_room){
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++){
            game->floors[id]->map[i][j] = (Point*)malloc(sizeof(Point));
            game->floors[id]->map[i][j]->discovered = 0;
            game->floors[id]->map[i][j]->activated = 0;
            game->floors[id]->map[i][j]->type = Nothing;
        }

    generate_rooms(id, stair_room);

    generate_corridors(id);  
    generate_pillar(id);
    generate_traps(id);

    if(stair_room != -1){
        Pair stair_pos = find_in_room(stair_room, id - 1, StairUp);
        game->floors[id]->map[stair_pos.x][stair_pos.y]->type = StairDown;
    }

    if(id != NUMBER_OF_FLOORS - 1)
       generate_stair_up(id); 
}

void init_game(WINDOW* main, char*_username){
    username = _username;

    //init
    clear();
    noecho();
    curs_set(false);

    game = (Game*)malloc(sizeof(Game));
    game->player = (Player*)malloc(sizeof(Player));
    game->floors = (Tabaghe**)malloc(sizeof(Tabaghe*) * NUMBER_OF_FLOORS);
    for(int i = 0; i < NUMBER_OF_FLOORS; i++)
        game->floors[i] = (Tabaghe*)malloc(sizeof(Tabaghe));

    game->cf = 0;
    game->ignore_hiding = 0;

    generate_floor(0, -1);
    for(int i = 1; i < NUMBER_OF_FLOORS; i++){
        int room = -1;
        for(int j = 0; j < game->floors[i - 1]->number_of_rooms; j++)
            if(find_in_room(j, i - 1, StairUp).x != -1)
                room = j;
        generate_floor(i, room);
    }

    //Player initial place
    game->player->x = game->floors[0]->rooms[0]->startx + game->floors[0]->rooms[0]->h/2;
    game->player->y = game->floors[0]->rooms[0]->starty + game->floors[0]->rooms[0]->w/2;

    activate_room(0, 0);

    draw_map(0);
    refresh();
    handle_input();
}

void move_to_floor(int id){
    game->cf = id;
    activate_room(0, id);
    draw_map(id);
}

void draw_map(int id){
    clear();
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++)
        if(game->floors[id]->rooms[i]->discovered || game->ignore_hiding)
            draw_room(game->floors[id]->rooms[i]);
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++){
            if(game->floors[id]->map[i][j]->discovered == 0 && game->ignore_hiding == 0)
                continue;
            draw_item(i, j, game->floors[id]->map[i][j]->type);
            refresh();
        }
    draw_player();
    refresh();
}
void handle_input(){
    while(true){
        int ch = getch();
        int x = game->player->x, y = game->player->y;
        int nx = x, ny = y;

        int id = game->cf;

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
        }else if(ch == 'm'){
            game->ignore_hiding ^= 1;
            draw_map(game->cf);
            continue;
        }
        if(valid(nx, ny) && game->floors[id]->map[nx][ny]->type != Nothing && game->floors[id]->map[nx][ny]->type != Wall
            && game->floors[id]->map[nx][ny]->type != Pillar){
            draw_item(x, y, game->floors[id]->map[x][y]->type);
            game->player->y = ny;
            game->player->x = nx;
            draw_player();

            //Update visibility
            int current_room = get_room_from_point(nx, ny, id);
            if(current_room != EOF && game->floors[id]->rooms[current_room]->discovered == 0){
                activate_room(current_room, id);
                draw_map(id);
            }
            if(game->floors[id]->map[nx][ny]->type == Corridor && game->floors[id]->map[nx][ny]->discovered == 0){
                activate_corridor(nx, ny, id);
                draw_map(id);
            }
            if(game->floors[id]->map[nx][ny]->type == Trap){
                //Trap activated
                game->floors[id]->map[nx][ny]->activated = 1;
            }

            // move to upstair
            if(game->floors[id]->map[nx][ny]->type == StairUp){
                move_to_floor(game->cf+1);
            }

            if(game->floors[id]->map[nx][ny]->type == StairDown){
                move_to_floor(game->cf-1);
            }
        }
    }
}
void generate_rooms(int id, int stair_room){
    refresh();

    int room_count =random_num(number_of_room_min, number_of_rooms_max);
    game->floors[id]->number_of_rooms = 0;
    game->floors[id]->rooms = (Room**)malloc(sizeof(Room*) * room_count);

    int i = 0;

    while(i < room_count){
        // We copied this room from previous level
        int w = random_num(room_width_min, room_width_max);
        int h = random_num(room_height_min, room_height_max);
        int startx = random_num(Padding, LINES - Padding);
        int starty = random_num(Padding, COLS - Padding);

        if(stair_room != -1 && i == 0){
            w = game->floors[id - 1]->rooms[stair_room]->w;
            h = game->floors[id - 1]->rooms[stair_room]->h;
            startx = game->floors[id - 1]->rooms[stair_room]->startx;
            starty = game->floors[id - 1]->rooms[stair_room]->starty;
        }
        if(!check_room_overlap(startx, starty, w, h, id)){
            game->floors[id]->rooms[i] = create_room(startx, starty, w, h);
            //edit game
            for(int j = 0; j < w; j++)
                for(int z = 0; z < h; z++){
                    if(j == 0 || j == w - 1 || z == 0 || z == h - 1)
                        game->floors[id]->map[startx + z][starty + j]->type = Wall;
                    else
                        game->floors[id]->map[startx + z][starty + j]->type = Floor;
                }
            game->floors[id]->number_of_rooms++;
            i++;
        }
    }
}
void generate_pillar(int id){
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int j = random_num(number_of_pillar_min,number_of_pillar_max);
        while(j--){
            while(1){
                int x = rand() % (game->floors[id]->rooms[i]->h-4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w-4) + game->floors[id]->rooms[i]->starty + 2;
                if(game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Pillar;
                if(!is_game_connected(id)){
                    game->floors[id]->map[x][y]->type = Floor;
                    continue;
                }
                return;
            }
        }
    }
}
void generate_traps(int id){
    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int j = random_num(number_of_traps_min,number_of_traps_max);
        while(j--){
            while(1){
                int x = rand() % (game->floors[id]->rooms[i]->h-4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w-4) + game->floors[id]->rooms[i]->starty + 2;
                if(game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Trap;
                break;
            }
        }
    }
}
void generate_stair_up(int id){
    int i = random_num(0, game->floors[id]->number_of_rooms - 1);
    while(1){
        int x = rand() % (game->floors[id]->rooms[i]->h-4) + game->floors[id]->rooms[i]->startx + 2;
        int y = rand() % (game->floors[id]->rooms[i]->w-4) + game->floors[id]->rooms[i]->starty + 2;
        if(game->floors[id]->map[x][y]->type != Floor)
            continue;
        game->floors[id]->map[x][y]->type = StairUp;
        break;
    }
}
int check_room_overlap(int startx, int starty, int w, int h, int id){
    if(startx + h - 1 >= LINES-2 || starty + w - 1 >= COLS-2)
        return 1;

    for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
        int stx = game->floors[id]->rooms[i]->startx, wx = game->floors[id]->rooms[i]->w;
        int sty = game->floors[id]->rooms[i]->starty, hx = game->floors[id]->rooms[i]->h;
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
void generate_corridors(int id){
    int same[N][N];
    for(int i = 0; i < N; i++)
        for(int j = 0; j < N; j++) same[i][j] = 0;
    while(!is_game_connected(id)){
        int mmn = inf, mx = 0;
        for(int i = 0; i < game->floors[id]->number_of_rooms; i++){
            if(same[i][i] < mmn)
                mmn = same[i][i];
            if(same[i][i] > mx)
                mx = same[i][i];
        }
        int u = rand() % game->floors[id]->number_of_rooms;
        refresh();
        if(same[u][u] != mmn && mx > mmn)
            continue;

        int mn = inf, v = -1;
        for(int i = 0; i < game->floors[id]->number_of_rooms; i++)
            if(u != i && bfs(u, i, id) < mn && !same[u][i])
                v = i, mn = bfs(u, i, id);
     
        if(mn == inf)
            continue;
        same[u][u]++;

        same[u][v] = same[v][u] = 1;
        make_corridor(u, v, id);
        refresh();
    }
}
void make_corridor(int u, int v, int id){
    int queue[N * N];
    int dist[N][N], par[N][N];
   int sx, sy; 
    for(int i = 0; i < game->floors[id]->rooms[u]->h; i++)
        for(int j = 0; j < game->floors[id]->rooms[u]->w; j++)
            if(game->floors[id]->map[i + game->floors[id]->rooms[u]->startx][j+game->floors[id]->rooms[u]->starty]->type == Floor)
                sx = i + game->floors[id]->rooms[u]->startx, sy = j+game->floors[id]->rooms[u]->starty;
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
                if(game->floors[id]->map[x][y]->type==Wall && game->floors[id]->map[nx][ny]->type==Wall)
                    continue;
                dist[nx][ny] = dist[x][y] + 1;
                par[nx][ny] = x * N + y;
                queue[qsz++] = nx * N + ny;
            }
        }
    }
    int distx = game->floors[id]->rooms[v]->startx + game->floors[id]->rooms[v]->h/2;
    int disty = game->floors[id]->rooms[v]->starty + game->floors[id]->rooms[v]->w/2;

    while(~par[distx][disty]){

        //Draw corridors
        if(game->floors[id]->map[distx][disty]->type != Floor && game->floors[id]->map[distx][disty]->type!=Pillar){
            if(game->floors[id]->map[distx][disty]->type == Wall || game->floors[id]->map[distx][disty]->type == Door){
               game->floors[id]->map[distx][disty]->type=Door;
            }else{                
                game->floors[id]->map[distx][disty]->type=Corridor;
            }
        }
        int p = par[distx][disty];
        distx = p / N, disty = p % N;
    }
}   
// get distance betweeen room u and v
int bfs(int u, int v, int id){
    int sx = game->floors[id]->rooms[u]->startx + game->floors[id]->rooms[u]->h/2;
    int sy = game->floors[id]->rooms[u]->starty + game->floors[id]->rooms[u]->w/2;
    int dx = game->floors[id]->rooms[v]->startx + game->floors[id]->rooms[v]->h/2;
    int dy = game->floors[id]->rooms[v]->starty + game->floors[id]->rooms[v]->w/2;
    return abs(sx - dx) + abs(dy - sy);
}
int mark[N][N];
int is_game_connected(int id){
    memset(mark, 0, sizeof(mark));
    int sx, sy;
    for(int i = 0; i < game->floors[id]->rooms[0]->h; i++)
        for(int j = 0; j < game->floors[id]->rooms[0]->w; j++)
            if(game->floors[id]->map[i + game->floors[id]->rooms[0]->startx][j + game->floors[id]->rooms[0]->starty]->type == Floor)
                sx = i + game->floors[id]->rooms[0]->startx, sy = j + game->floors[id]->rooms[0]->starty;
    dfs(sx, sy, id);
    for(int i = 0; i < LINES; i++)
        for(int j = 0; j < COLS; j++)
            if(game->floors[id]->map[i][j]->type != Nothing && game->floors[id]->map[i][j]->type != Pillar && !mark[i][j])
                return 0;
    return 1;
}
void dfs(int x, int y, int id){
    mark[x][y] = 1;
    for(int i = 0; i < 4; i++){
        int nx = dx[i] + x, ny = y + dy[i];
        if(!mark[nx][ny] && nx >= 0 && ny >= 0 && nx < LINES && ny < COLS
             && game->floors[id]->map[nx][ny]->type != Nothing && game->floors[id]->map[nx][ny]->type != Pillar)
            dfs(nx, ny, id);
    }
}