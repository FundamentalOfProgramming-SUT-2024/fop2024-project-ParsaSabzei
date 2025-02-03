#include <ncurses.h>
#include "game.h"
#include <pthread.h>

char *username;

void update_status()
{
    for (int i = 0; i < COLS; i++)
        mvwprintw(status_win, 1, i, " ");
    box(status_win, 0, 0);
    mvwprintw(status_win, 1, 2, "Floor: %d", game->cf + 1);

    wattron(status_win, COLOR_PAIR(basic_colors[1]));
    mvwprintw(status_win, 1, 20, "%s: %d", GOLD_UNICODE, game->player->Golds);
    wattroff(status_win, COLOR_PAIR(basic_colors[1]));

    wattron(status_win, COLOR_PAIR(BLACK_GOLD_COLOR));
    mvwprintw(status_win, 1, 30, "%s: %d", BLACK_GOLD_UNICODE, game->player->black_golds);
    wattroff(status_win, COLOR_PAIR(BLACK_GOLD_COLOR));

    mvwprintw(status_win, 1, 38, "Weapon:%s", weapon_icon[game->player->weapon]);

    if (game->ignore_picking)
    {
        wattron(status_win, COLOR_PAIR(basic_colors[0]));
        mvwprintw(status_win, 1, 50, "X");
        wattroff(status_win, COLOR_PAIR(basic_colors[0]));
    }

    for (int i = 0; i < SPELL_TYPES; i++)
    {
        if (game->active_spells[i]) // if ith spell is active draw it
            mvwprintw(status_win, 1, 60 + 4 * i, "%s", spell_icon[i]);
    }

    mvwprintw(status_win, 1, 80, "Game Difficulty: %s", Difficulty_Names[setting.Difficulty]);

    for (int i = 0; i < game->player->hunger; i++)
        mvwprintw(status_win, 1, 120 + 4 * i, "🍖");

    // Printing player's health
    //  COLS - 25 + 4 * i
    int last = 0;
    for (int i = 0; i < game->player->health / 2; i++)
        mvwprintw(status_win, 1, COLS - 25 + 4 * i, "❤️"), last = i;
    if (game->player->health & 1)
        mvwprintw(status_win, 1, COLS - 25 + (last + 1) * 4, "💔");

    wrefresh(status_win);
    refresh();
}
void init_status_bar()
{
    status_win = newwin(3, COLS - 2, 0, 0);
    update_status();
}
void generate_floor(int id, int stair_room)
{
    game->floors[id]->weapon_count = 0;
    game->floors[id]->monster_count = 0;
    game->floors[id]->spell_count = 0;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            game->floors[id]->map[i][j] = (Point *)malloc(sizeof(Point));
            game->floors[id]->map[i][j]->discovered = 0;
            game->floors[id]->map[i][j]->activated = 0;
            game->floors[id]->map[i][j]->type = Nothing;
        }

    generate_rooms(id, stair_room);

    if(id == game->floor_count - 1)
        set_treasure_room();

    generate_corridors(id);
    generate_pillar(id);
    generate_traps(id);

    if (stair_room != -1)
    {
        Pair stair_pos = find_in_room(stair_room, id - 1, StairUp);
        game->floors[id]->map[stair_pos.x][stair_pos.y]->type = StairDown;
    }
    generate_golds(id);
    generate_black_golds(id);
    generate_deamon(id);
    generate_fire_breathing(id);
    generate_giant(id);
    generate_snake(id);
    generate_undeed(id);
    generate_dagger(id);
    generate_magic_wand(id);
    generate_sword(id);
    generate_arrow(id);
    generate_food(id);
    generate_spell(id, Health);
    generate_spell(id, Speed);
    generate_spell(id, Damage);
    if (id != game->floor_count - 1)
        generate_stair_up(id);
    
}
void set_treasure_room(){
    int dis = -inf, ind = -1;
    for(int i = 0; i < game->floors[game->floor_count - 1]->number_of_rooms; i++)
        if(bfs(0, i, game->floor_count - 1) > dis)
            dis = bfs(0, i, game->floor_count - 1), ind = i;
    treasure_room = ind;
    int id = game->floor_count - 1;
    int x = rand() % (game->floors[id]->rooms[ind]->h - 4) + game->floors[id]->rooms[ind]->startx + 2;
    int y = rand() % (game->floors[id]->rooms[ind]->w - 4) + game->floors[id]->rooms[ind]->starty + 2;
    game->floors[id]->map[x][y]->type = Endpoint;
}
void create_new_game()
{
    game = (Game *)malloc(sizeof(Game));
    game->player = (Player *)malloc(sizeof(Player));
    game->bag = (Bag *)malloc(sizeof(Bag));
    game->floor_count = random_num(MIN_NUMBER_OF_FLOORS, MAX_NUMBER_OF_FLOORS);
    game->floors = (Tabaghe **)malloc(sizeof(Tabaghe *) * game->floor_count);
    for (int i = 0; i < game->floor_count; i++)
        game->floors[i] = (Tabaghe *)malloc(sizeof(Tabaghe));

    for (int i = 0; i < SPELL_TYPES; i++)
    {
        game->active_spells[i] = 0;
        Spell *spell = malloc(sizeof(Spell));
        spell->type = i;
        spell->count = 0;
        game->bag->spells[i] = spell;
    }
    for (int i = 0; i < WEAPON_TYPES; i++)
    {
        Weapon *wep = malloc(sizeof(Weapon));
        wep->type = i;
        wep->count = 0;
        if (i == 0)
            wep->count = 1;
        game->bag->weapons[i] = wep;
    }

    game->cf = 0;
    game->ignore_hiding = 0;
    game->ignore_picking = 0;

    generate_floor(0, -1);
    for (int i = 1; i < game->floor_count; i++)
    {
        int room = -1;
        for (int j = 0; j < game->floors[i - 1]->number_of_rooms; j++)
            if (find_in_room(j, i - 1, StairUp).x != -1)
                room = j;
        generate_floor(i, room);
    }

    // initing player
    game->player->Golds = 0;
    game->player->black_golds = 0;
    game->player->health = 10;
    for(int i = game->floors[0]->rooms[0]->startx + 1; i < game->floors[0]->rooms[0]->startx + game->floors[0]->rooms[0]->h - 1; i++)
        for(int j = game->floors[0]->rooms[0]->starty + 1; j < game->floors[0]->rooms[0]->starty + game->floors[0]->rooms[0]->w - 1; j++)
            if(is_monster_on(0, i, j) == -1 && is_spell_on(0, i, j) == -1 && is_weapon_on(0, i, j) == -1 && game->floors[0]->map[i][j]->type == Floor){
                game->player->x = i;
                game->player->y = j;
                break;
            }
    game->player->weapon = 0;
    game->player->dir = 0;
    game->player->hunger = init_hunger;

    activate_room(0, 0);
    activate_monster(0, 0);
    enter_new_room(0, 0);
}
void *hunger()
{
    while (1)
    {
        sleep(20);
        game->player->hunger -= 1;
        update_status();
    }
}
void *health_increase()
{
    while (1)
    {
        sleep(1);
        time_t elapsed = time(NULL) - last_time_attacked;
        int t = secs_increase_heart;
        if (game->active_spells[Health])
            t /= 4;
        if (elapsed >= t)
        {
            if (game->player->hunger == init_hunger)
                game->player->health += 1;
            if (game->active_spells[Health])
                game->player->health += 2;
            last_time_attacked = time(NULL);
        }
    }
}
void *spell_management()
{
    while (1)
    {
        sleep(1);
        for (int i = 0; i < SPELL_TYPES; i++)
        {
            if (game->active_spells[i] == 0)
                continue;
            time_t elapsed = time(NULL) - last_time_spells[i];
            if (elapsed >= secs_to_stand_spell)
            {
                game->active_spells[i] = 0;
                update_status();
                show_message(spell_name[i], " Spell Ended!");
            }
        }
    }
}
void set_game_difficulty()
{
    if (setting.Difficulty == Easy)
    {
        min_deamons = 0;
        max_deamons = 2;
        min_fire_breathing = 0;
        max_fire_breathing = 1;
        min_giant = 0;
        max_giant = 1;
        min_snake = 0;
        max_snake = 1;
        min_undeed = 0;
        max_undeed = 1;
        min_food = 3;
        max_food = 5;
        secs_to_hunger = 40;
        secs_increase_heart = 1;
        secs_to_stand_spell = 20;
        init_hunger = 7;
    }
    else if (setting.Difficulty == Normal)
    {
        min_deamons = 1;
        max_deamons = 3;
        min_fire_breathing = 0;
        max_fire_breathing = 2;
        min_giant = 0;
        max_giant = 2;
        min_snake = 0;
        max_snake = 1;
        min_undeed = 0;
        max_undeed = 1;
        min_food = 1;
        max_food = 3;
        secs_to_hunger = 30;
        secs_increase_heart = 10;
        secs_to_stand_spell = 15;
        init_hunger = 6;
    }
    else if (setting.Difficulty == Hard)
    {
        min_deamons = 2;
        max_deamons = 6;
        min_fire_breathing = 2;
        max_fire_breathing = 4;
        min_giant = 1;
        max_giant = 3;
        min_snake = 1;
        max_snake = 2;
        min_undeed = 1;
        max_undeed = 3;
        min_food = 0;
        max_food = 4;
        secs_to_hunger = 20;
        secs_increase_heart = 12;
        secs_to_stand_spell = 10;
        init_hunger = 5;
    }
}
void init_game(WINDOW *main, char *_username, int scr)
{
    username = _username;
    setting = load_settings(username);
    basic_colors[0] = red(), basic_colors[1] = yellow(), basic_colors[2] = blue(), basic_colors[3] = green();

    set_game_difficulty();

    // init
    clear();
    noecho();
    curs_set(false);

    if (scr == 0)
        create_new_game();
    else if (scr == 1)
        game = load_game(username);

    // init bag and weapon page
    bag_win = newwin(LINES - 10, COLS - 20, 5, 10);
    weapon_win = newwin(weapon_page_size, weapon_page_size * 2, (LINES - weapon_page_size) / 2, COLS / 2 - weapon_page_size);
    spell_win = newwin(weapon_page_size, weapon_page_size * 2, (LINES - weapon_page_size) / 2, COLS / 2 - weapon_page_size);

    // init threads
    pthread_t hunger_thread, health_thread, spell_thread;
    pthread_create(&hunger_thread, NULL, hunger, NULL);
    pthread_create(&health_thread, NULL, health_increase, NULL);
    pthread_create(&spell_thread, NULL, spell_management, NULL);

    draw_map(game->cf);
    refresh();

    init_status_bar();
    handle_input();
}
void move_to_floor(int id)
{
    game->cf = id;
    update_status();
    activate_room(0, id);
    draw_map(id);
}
void draw_monsters(int id)
{
    for (int i = 0; i < game->floors[id]->monster_count; i++)
    {
        if (game->floors[id]->monsters[i]->active == 0 && !game->ignore_hiding)
            continue;
        switch (game->floors[id]->monsters[i]->type)
        {
        case Deamon:
            draw_deamon(game->floors[id]->monsters[i]->x, game->floors[id]->monsters[i]->y);
            break;
        case Fire_Breathing:
            draw_fire_breathing(game->floors[id]->monsters[i]->x, game->floors[id]->monsters[i]->y);
            break;
        case Giant:
            draw_giant(game->floors[id]->monsters[i]->x, game->floors[id]->monsters[i]->y);
            break;
        case Snake:
            draw_snake(game->floors[id]->monsters[i]->x, game->floors[id]->monsters[i]->y);
            break;
        case Undeed:
            draw_undeed(game->floors[id]->monsters[i]->x, game->floors[id]->monsters[i]->y);
            break;
        }
    }
}
void draw_weapons(int id)
{
    for (int i = 0; i < game->floors[id]->weapon_count; i++)
    {
        if (game->floors[id]->weapons_on_ground[i]->active == 0 && !game->ignore_hiding)
            continue;
        mvprintw(game->floors[id]->weapons_on_ground[i]->x, game->floors[id]->weapons_on_ground[i]->y,
                 "%s", weapon_icon[game->floors[id]->weapons_on_ground[i]->type]);
    }
}
void draw_spells(int id)
{
    for (int i = 0; i < game->floors[id]->spell_count; i++)
    {
        if (game->floors[id]->spells_on_ground[i]->active == 0 && !game->ignore_hiding)
            continue;
        mvprintw(game->floors[id]->spells_on_ground[i]->x, game->floors[id]->spells_on_ground[i]->y,
                 "%s", spell_icon[game->floors[id]->spells_on_ground[i]->type]);
    }
}
void draw_map(int id)
{
    clear();
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
        if (game->floors[id]->rooms[i]->discovered || game->ignore_hiding)
            draw_room(game->floors[id]->rooms[i]);
    for (int i = 0; i < LINES; i++)
        for (int j = 0; j < COLS; j++)
        {
            if (game->floors[id]->map[i][j]->discovered == 0 && game->ignore_hiding == 0)
                continue;
            draw_item(i, j, game->floors[id]->map[i][j]->type);
            refresh();
        }
    draw_player();
    draw_monsters(id);
    draw_weapons(id);
    draw_spells(id);
    update_status();
    refresh();
}
void attack()
{
    // if damage spell is active
    int coef = game->active_spells[Damage] ? 2 : 1;

    if (game->player->weapon == Mace)
    {
        int x = game->player->x, y = game->player->y;
        show_message("Attack with Mace!", weapon_icon[Mace]);
        for (int i = 0; i < 8; i++)
        {
            int nx = x + dxx[i], ny = y + dyy[i];
            int killed = -1;

            for (int i = 0; i < game->floors[game->cf]->monster_count; i++)
            {
                if (game->floors[game->cf]->monsters[i]->x != nx || game->floors[game->cf]->monsters[i]->y != ny)
                    continue;
                if (game->floors[game->cf]->monsters[i]->health <= weapon_damage[Mace] * coef)
                    killed = i;
                else
                    game->floors[game->cf]->monsters[i]->health -= weapon_damage[Mace] * coef;
            }
            if (killed != -1)
                kill_monster(game->cf, killed);

            if (game->floors[game->cf]->map[nx][ny]->type == Floor && is_weapon_on(game->cf, nx, ny) == -1)
            {
                attron(A_DIM);
                draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
                refresh();
                usleep(60000);
                attroff(A_DIM);

                draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
            }
        }
    }
    else if (game->player->weapon == Dagger)
    {
        if (!game->bag->weapons[Dagger]->count)
            return;
        game->bag->weapons[Dagger]->count--;

        int x = game->player->x, y = game->player->y;
        show_message("Attack with Dagger!", weapon_icon[Dagger]);
        int nx = x, ny = y;
        for (int i = 0; i < 5; i++)
        {
            nx += dx[game->player->dir], ny += dy[game->player->dir];
            int killed = is_monster_on(game->cf, nx, ny);
            if (killed != -1 && game->floors[game->cf]->monsters[killed]->health > weapon_damage[Dagger] * coef)
            {
                game->floors[game->cf]->monsters[killed]->health -= weapon_damage[Dagger] * coef;
                killed = -1;
            }

            if (killed != -1)
            {
                kill_monster(game->cf, killed);
                break;
            }

            // The dagger hit obstacle
            if (game->floors[game->cf]->map[nx][ny]->type == Pillar ||
                game->floors[game->cf]->map[nx][ny]->type == Wall ||
                game->floors[game->cf]->map[nx][ny]->type == Corridor ||
                game->floors[game->cf]->map[nx][ny]->type == Door)
                break;

            mvprintw(nx, ny, "%s", weapon_icon[Dagger]);
            refresh();
            usleep(60000);

            draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
        }
    }
    else if (game->player->weapon == Magic_Wand)
    {
        if (!game->bag->weapons[Magic_Wand]->count)
            return;
        game->bag->weapons[Magic_Wand]->count--;

        int x = game->player->x, y = game->player->y;
        show_message("Attack with Magic Wand!", weapon_icon[Magic_Wand]);
        int nx = x, ny = y;
        for (int i = 0; i < 10; i++)
        {
            nx += dx[game->player->dir], ny += dy[game->player->dir];
            int killed = is_monster_on(game->cf, nx, ny);
            if (killed != -1 && game->floors[game->cf]->monsters[killed]->health > weapon_damage[Magic_Wand] * coef)
            {
                game->floors[game->cf]->monsters[killed]->health -= weapon_damage[Magic_Wand] * coef;
                killed = -1;
            }

            if (killed != -1)
            {
                kill_monster(game->cf, killed);
                break;
            }

            // The Magic Wand hit obstacle
            if (game->floors[game->cf]->map[nx][ny]->type == Pillar ||
                game->floors[game->cf]->map[nx][ny]->type == Wall ||
                game->floors[game->cf]->map[nx][ny]->type == Corridor ||
                game->floors[game->cf]->map[nx][ny]->type == Door)
                break;

            mvprintw(nx, ny, "%s", MAGIC_WAND_BULLET);
            refresh();
            usleep(60000);

            draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
        }
    }
    else if (game->player->weapon == Sword)
    {
        int x = game->player->x, y = game->player->y;
        show_message("Attack with Sword!", weapon_icon[Sword]);
        for (int i = 0; i < 8; i++)
        {
            int nx = x + dxx[i], ny = y + dyy[i];
            int killed = -1;

            for (int i = 0; i < game->floors[game->cf]->monster_count; i++)
            {
                if (game->floors[game->cf]->monsters[i]->x != nx || game->floors[game->cf]->monsters[i]->y != ny)
                    continue;
                if (game->floors[game->cf]->monsters[i]->health <= weapon_damage[Sword] * coef)
                    killed = i;
                else
                    game->floors[game->cf]->monsters[i]->health -= weapon_damage[Sword] * coef;
            }
            if (killed != -1)
                kill_monster(game->cf, killed);

            if (game->floors[game->cf]->map[nx][ny]->type == Floor && is_weapon_on(game->cf, nx, ny) == -1)
            {
                attron(A_DIM);
                draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
                refresh();
                usleep(60000);
                attroff(A_DIM);

                draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
            }
        }
    }
    else if (game->player->weapon == Arrow)
    {
        if (!game->bag->weapons[Arrow]->count)
            return;
        game->bag->weapons[Arrow]->count--;

        int x = game->player->x, y = game->player->y;
        show_message("Attack with Arrow!", weapon_icon[Arrow]);
        int nx = x, ny = y;
        for (int i = 0; i < 5; i++)
        {
            nx += dx[game->player->dir], ny += dy[game->player->dir];
            int killed = is_monster_on(game->cf, nx, ny);
            if (killed != -1 && game->floors[game->cf]->monsters[killed]->health > weapon_damage[Arrow] * coef)
            {
                game->floors[game->cf]->monsters[killed]->health -= weapon_damage[Arrow] * coef;
                killed = -1;
            }

            if (killed != -1)
            {
                kill_monster(game->cf, killed);
                break;
            }

            // The dagger hit obstacle
            if (game->floors[game->cf]->map[nx][ny]->type == Pillar ||
                game->floors[game->cf]->map[nx][ny]->type == Wall ||
                game->floors[game->cf]->map[nx][ny]->type == Corridor ||
                game->floors[game->cf]->map[nx][ny]->type == Door)
                break;

            mvprintw(nx, ny, "%s", ARROW_UNICODE);
            refresh();
            usleep(60000);

            draw_item(nx, ny, game->floors[game->cf]->map[nx][ny]->type);
        }
    }
    draw_weapons(game->cf);
    draw_monsters(game->cf);
}
void move_monsters(int id, int nx, int ny, int room)
{
    for (int i = 0; i < game->floors[id]->monster_count; i++)
    {
        if (game->floors[id]->monsters[i]->type == Snake && game->floors[id]->monsters[i]->active)
        {
            // Bfs for finding best dir for snake
            int mnsx, mnsy;

            int queue[N * N];
            int dis[N][N], par[N][N];
            int sx = nx, sy = ny;
            memset(dis, 63, sizeof(dis));
            dis[sx][sy] = 0, queue[0] = sx * N + sy;
            par[sx][sy] = -1;
            int f = 0, qsz = 1;

            while (f < qsz)
            {
                int x = queue[f] / N, y = queue[f] % N;
                f++;
                for (int j = 0; j < SNAKE_RANDOMNESS_MOVE; j++)
                {
                    int nnx = dx[rand() % 4] + x, nny = dy[rand() % 4] + y;
                    if (valid(nnx, nny) && dis[nnx][nny] > dis[x][y] + 1)
                    {
                        // Prevent moving through walls
                        if (game->floors[id]->map[nnx][nny]->type == Wall || game->floors[id]->map[nnx][nny]->type == Pillar || game->floors[id]->map[nnx][nny]->discovered == 0 || game->floors[id]->map[nnx][nny] == Nothing)
                            continue;
                        dis[nnx][nny] = dis[x][y] + 1;
                        par[nnx][nny] = x * N + y;
                        queue[qsz++] = nnx * N + nny;
                    }
                }
            }
            int distx = game->floors[id]->monsters[i]->x;
            int disty = game->floors[id]->monsters[i]->y;
            mnsx = par[distx][disty] / N, mnsy = par[distx][disty] % N;

            if (dist(mnsx, mnsy, nx, ny) == 0)
                continue;
            if (dist(mnsx, mnsy, nx, ny) == 1)
            {
                game->player->health -= monsters_damage[game->floors[id]->monsters[i]->type];
                update_status();
                show_message("You've been attacked by a Snake🐍", "");
                last_time_attacked = time(NULL);
            }
            int x = game->floors[id]->monsters[i]->x, y = game->floors[id]->monsters[i]->y;
            draw_item(x, y, game->floors[id]->map[x][y]->type);
            game->floors[id]->monsters[i]->x = mnsx;
            game->floors[id]->monsters[i]->y = mnsy;
            draw_monsters(id);
            draw_weapons(id);
            return;
        }
        if (get_monster_room(i, id) == room)
        {
            int x = game->floors[id]->monsters[i]->x;
            int y = game->floors[id]->monsters[i]->y;

            int ans = dist(x, y, nx, ny), bestdir = 4;

            int cnt = 0, valid_dirs[N];
            for (int j = 0; j < 4; j++)
            {
                int mnsx = x + dx[j];
                int mnsy = y + dy[j];
                if (!valid(mnsx, mnsy) || game->floors[id]->map[mnsx][mnsy]->type == Wall ||
                    game->floors[id]->map[mnsx][mnsy]->type == Door || game->floors[id]->map[mnsx][mnsy]->type == Pillar ||
                    (is_monster_on(id, mnsx, mnsy) != -1) || (nx == mnsx && ny == mnsy))
                    continue;

                valid_dirs[cnt++] = j;
                if (dist(nx, ny, mnsx, mnsy) < ans)
                    ans = dist(nx, ny, mnsx, mnsy), bestdir = j;
            }

            // Following are ended like giant monster
            if (game->floors[id]->monsters[i]->following == 0)
            {
                continue;
            }
            game->floors[id]->monsters[i]->following--;

            int mnsx = x + dx[bestdir];
            int mnsy = y + dy[bestdir];
            if (dist(mnsx, mnsy, nx, ny) == 1)
            {
                game->player->health -= monsters_damage[game->floors[id]->monsters[i]->type];
                update_status();
                show_message("You've been attacked by a ", monsters_name[game->floors[id]->monsters[i]->type]);
                last_time_attacked = time(NULL);
            }
            draw_item(x, y, game->floors[id]->map[x][y]->type);
            game->floors[id]->monsters[i]->x = mnsx;
            game->floors[id]->monsters[i]->y = mnsy;
            draw_monsters(id);
            draw_weapons(id);
        }
    }
}
void show_bag()
{
    box(bag_win, 0, 0);
    mvwprintw(bag_win, 1, 3, "Weapons:");

    for (int i = 0; i < WEAPON_TYPES; i++)
    {
        mvwprintw(bag_win, 2 * i + 3, 3, "%s(%s ): %d", weapon_name[i], weapon_icon[i], game->bag->weapons[i]->count);
    }

    mvwprintw(bag_win, 1, 50, "Spells:");

    for (int i = 0; i < SPELL_TYPES; i++)
    {
        mvwprintw(bag_win, 2 * i + 3, 50, "%s(%s ): %d", spell_name[i], spell_icon[i], game->bag->spells[i]->count);
    }
    wrefresh(bag_win);
    refresh();
    while (getch() != 'r')
        ;
}
void show_weapons_page()
{
    box(weapon_win, 0, 0);
    for (int i = 0; i < WEAPON_TYPES; i++)
    {
        if (i == game->player->weapon)
        {
            wattron(weapon_win, COLOR_PAIR(basic_colors[3]));
            mvwprintw(weapon_win, 2 * i + 3, 3, "%s(%s ): %d", weapon_name[i], weapon_icon[i], game->bag->weapons[i]->count);
            wattroff(weapon_win, COLOR_PAIR(basic_colors[3]));
        }
        else
        {
            if (game->bag->weapons[i]->count)
                mvwprintw(weapon_win, 2 * i + 3, 3, "%s(%s ): %d    Press %d to select", weapon_name[i], weapon_icon[i], game->bag->weapons[i]->count, i + 1);
            else
                mvwprintw(weapon_win, 2 * i + 3, 3, "%s(%s ): %d", weapon_name[i], weapon_icon[i], game->bag->weapons[i]->count);
        }
    }

    mvwprintw(weapon_win, 2 * WEAPON_TYPES + 6, 3, "Press Y to deselect current weapon");
    wrefresh(weapon_win);
    refresh();
    while (1)
    {
        int c = getch();
        if (c == 'i')
            break;
        int num = c - '0';
        if (num > WEAPON_TYPES || num < 1)
            continue;
        game->player->weapon = num - 1;
        break;
    }
}
void show_spells_page()
{
    box(spell_win, 0, 0);
    for (int i = 0; i < SPELL_TYPES; i++)
    {

        if (game->bag->spells[i]->count)
            mvwprintw(spell_win, 2 * i + 3, 3, "%s(%s ): %d    Press %d to select", spell_name[i], spell_icon[i], game->bag->spells[i]->count, i + 1);
        else
            mvwprintw(spell_win, 2 * i + 3, 3, "%s(%s ): %d", spell_name[i], spell_icon[i], game->bag->spells[i]->count);
    }

    wrefresh(spell_win);
    refresh();
    while (1)
    {
        int c = getch();
        if (c == 'p')
            break;
        int num = c - '0';
        if (num > SPELL_TYPES || num < 1)
            continue;
        num--;
        // consume ith spell
        game->active_spells[num] = 1;
        last_time_spells[num] = time(NULL);
        update_status();
        break;
    }
}
void pick_weapon(int i)
{
    Weapon *weapon = game->floors[game->cf]->weapons_on_ground[i];
    draw_item(weapon->x, weapon->y, game->floors[game->cf]->map[weapon->x][weapon->y]->type);
    remove_weapon(game->cf, i);
    game->bag->weapons[weapon->type]->count += weapon_count_per_collect[weapon->type];
    show_message("You've picked up a ", weapon_icon[weapon->type]);
}
void pick_spell(int i)
{
    Spell *spell = game->floors[game->cf]->spells_on_ground[i];
    draw_item(spell->x, spell->y, game->floors[game->cf]->map[spell->x][spell->y]->type);
    remove_spell(game->cf, i);
    game->bag->spells[spell->type]->count += 1;
    show_message("You've picked up a ", spell_icon[spell->type]);
}
void enter_new_room(int id, int i)
{
    for (int j = 0; j < game->floors[id]->monster_count; j++)
        if (get_monster_room(j, id) == i)
        {
            game->floors[id]->monsters[j]->following = followings[game->floors[id]->monsters[j]->type];
        }
}
void handle_input()
{
    while (true)
    {
        static int number_of_moves = 0;

        int ch = getch();
        int x = game->player->x, y = game->player->y;
        int nx = x, ny = y;

        int id = game->cf;
        if (ch == 'w')
        {
            nx--;
            game->player->dir = 2;
            number_of_moves++;
        }
        else if (ch == 's')
        {
            nx++;
            game->player->dir = 0;
            number_of_moves++;
        }
        else if (ch == 'a')
        {
            ny--;
            game->player->dir = 1;
            number_of_moves++;
        }
        else if (ch == 'd')
        {
            ny++;
            game->player->dir = 3;
            number_of_moves++;
        }
        else if (ch == 'q')
        {
            nx--, ny--;
            number_of_moves++;
        }
        else if (ch == 'e')
        {
            nx--, ny++;
            number_of_moves++;
        }
        else if (ch == 'z')
        {
            nx++, ny--;
            number_of_moves++;
        }
        else if (ch == 'c')
        {
            nx++, ny++;
            number_of_moves++;
        }
        else if (ch == 'm')
        {
            game->ignore_hiding ^= 1;
            draw_map(game->cf);
            continue;
        }
        else if (ch == 32)
        { // Space Key
            attack();
            continue;
        }
        else if (ch == 'r')
        { // open bag
            show_bag();
            draw_map(id);
            continue;
        }
        else if (ch == 'x')
        {
            game->ignore_picking ^= 1;
            update_status();
            continue;
        }
        else if (ch == 'i')
        { // open weapons page
            show_weapons_page();
            draw_map(id);
            continue;
        }
        else if (ch == 'p')
        {
            // open spell page
            show_spells_page();
            draw_map(id);
            continue;
        }
        else if(ch == 27){ //Esc
            //saving the game than quit
            save_game(username, game);
            return;
        }
        else
            continue;

        // clear button Status bar
        clear_buttom_status_bar();

        if (valid(nx, ny) && game->floors[id]->map[nx][ny]->type != Nothing && game->floors[id]->map[nx][ny]->type != Wall && game->floors[id]->map[nx][ny]->type != Pillar)
        {

            // Preventing move through a monster
            if (is_monster_on(id, nx, ny) != -1)
                continue;
            if (is_weapon_on(id, nx, ny) != -1 && !game->ignore_picking)
            {
                pick_weapon(is_weapon_on(id, nx, ny));
            }

            if (is_spell_on(id, nx, ny) != -1 && !game->ignore_picking)
            {
                pick_spell(is_spell_on(id, nx, ny));
            }

            draw_item(x, y, game->floors[id]->map[x][y]->type);
            game->player->y = ny;
            game->player->x = nx;
            draw_player();

            // Update visibility
            int current_room = get_room_from_point(nx, ny, id);

            // if speed spell is active move the monster when number_of_moves is odd
            if (!game->active_spells[Speed] || (game->active_spells[Speed] && (number_of_moves & 1)))
                move_monsters(id, nx, ny, current_room);

            // Entering a room
            if (game->floors[id]->map[x][y]->type == Corridor && game->floors[id]->map[nx][ny]->type == Door)
                enter_new_room(id, current_room);

            if (current_room != EOF && game->floors[id]->rooms[current_room]->discovered == 0)
            {
                activate_room(current_room, id);
                draw_map(id);
            }
            if (game->floors[id]->map[nx][ny]->type == Corridor && game->floors[id]->map[nx][ny]->discovered == 0)
            {
                activate_corridor(nx, ny, id);
                draw_map(id);
            }
            if (game->floors[id]->map[nx][ny]->type == Trap)
            {
                // Trap activated
                game->floors[id]->map[nx][ny]->activated = 1;
                game->player->health -= 1;
                show_message("Ooch! That was a trap", "");
                update_status();
            }

            // move to upstair
            if (game->floors[id]->map[nx][ny]->type == StairUp)
                move_to_floor(game->cf + 1);
            if (game->floors[id]->map[nx][ny]->type == StairDown)
                move_to_floor(game->cf - 1);
            if (game->floors[id]->map[nx][ny]->type == Gold)
            {
                game->player->Golds += 1;
                // remove gold from ground
                game->floors[id]->map[nx][ny]->type = Floor;
                update_status();
                show_message("One Gold Reicieved", GOLD_UNICODE);
            }
            if (game->floors[id]->map[nx][ny]->type == Black_Gold)
            {
                game->player->black_golds += 1;
                // remove gold from ground
                game->floors[id]->map[nx][ny]->type = Floor;
                update_status();
                show_message("One Black Gold Reicieved", BLACK_GOLD_UNICODE);
            }
            if (game->floors[id]->map[nx][ny]->type == Food)
            {
                // remove meat from ground
                game->floors[id]->map[nx][ny]->type = Floor;

                if (game->player->hunger == init_hunger)
                    continue;
                game->player->hunger += 1;
                update_status();
                show_message("One Food Reicieved", FOOD_UNICODE);
            }
        }

        draw_monsters(id);
        draw_weapons(id);
        draw_spells(id);
        if (game->player->health <= 0)
        {
            clear();
        }
    }
}
void generate_spell(int id, enum SpellType type)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 100);
        if (j > SPELL_PROB)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            Spell *wep = malloc(sizeof(Spell));
            wep->count = 1;
            wep->type = type;
            wep->x = x, wep->y = y;
            wep->active = 0;
            game->floors[id]->spells_on_ground[game->floors[id]->spell_count++] = wep;
            break;
        }
    }
}
void generate_dagger(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 10);
        if (j > DAGGER_PROB)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            Weapon *wep = malloc(sizeof(Weapon));
            wep->count = weapon_count_per_collect[Dagger];
            wep->type = Dagger;
            wep->x = x, wep->y = y;
            wep->active = 0;
            game->floors[id]->weapons_on_ground[game->floors[id]->weapon_count++] = wep;
            break;
        }
    }
}
void generate_magic_wand(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 100);
        if (j > MAGIC_WAND_PROB)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            Weapon *wep = malloc(sizeof(Weapon));
            wep->count = weapon_count_per_collect[Magic_Wand];
            wep->type = Magic_Wand;
            wep->x = x, wep->y = y;
            wep->active = 0;
            game->floors[id]->weapons_on_ground[game->floors[id]->weapon_count++] = wep;
            break;
        }
    }
}
void generate_sword(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 100);
        if (j > SWORD_PROB)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            Weapon *wep = malloc(sizeof(Weapon));
            wep->count = 1;
            wep->type = Sword;
            wep->x = x, wep->y = y;
            wep->active = 0;
            game->floors[id]->weapons_on_ground[game->floors[id]->weapon_count++] = wep;
            break;
        }
    }
}
void generate_arrow(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 100);
        if (j > ARROW_PROB)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            Weapon *wep = malloc(sizeof(Weapon));
            wep->count = weapon_count_per_collect[Arrow];
            wep->type = Arrow;
            wep->x = x, wep->y = y;
            wep->active = 0;
            game->floors[id]->weapons_on_ground[game->floors[id]->weapon_count++] = wep;
            break;
        }
    }
}
void generate_rooms(int id, int stair_room)
{
    refresh();

    int room_count = random_num(number_of_room_min, number_of_rooms_max);
    game->floors[id]->number_of_rooms = 0;
    game->floors[id]->rooms = (Room **)malloc(sizeof(Room *) * room_count);

    int i = 0;

    while (i < room_count)
    {
        // We copied this room from previous level
        int w = random_num(room_width_min, room_width_max);
        int h = random_num(room_height_min, room_height_max);
        int startx = random_num(Padding, LINES - Padding);
        int starty = random_num(Padding, COLS - Padding);

        if (stair_room != -1 && i == 0)
        {
            w = game->floors[id - 1]->rooms[stair_room]->w;
            h = game->floors[id - 1]->rooms[stair_room]->h;
            startx = game->floors[id - 1]->rooms[stair_room]->startx;
            starty = game->floors[id - 1]->rooms[stair_room]->starty;
        }
        if (!check_room_overlap(startx, starty, w, h, id))
        {
            game->floors[id]->rooms[i] = create_room(startx, starty, w, h);
            // edit game
            for (int j = 0; j < w; j++)
                for (int z = 0; z < h; z++)
                {
                    if (j == 0 || j == w - 1 || z == 0 || z == h - 1)
                        game->floors[id]->map[startx + z][starty + j]->type = Wall;
                    else
                        game->floors[id]->map[startx + z][starty + j]->type = Floor;
                }
            game->floors[id]->number_of_rooms++;
            i++;
        }
    }
}
void generate_pillar(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(number_of_pillar_min, number_of_pillar_max);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Pillar;
                if (!is_game_connected(id))
                {
                    game->floors[id]->map[x][y]->type = Floor;
                    continue;
                }
                return;
            }
        }
    }
}
void generate_golds(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(number_of_gold_min, number_of_gold_max);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Gold;
                break;
            }
        }
    }
}
void generate_food(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_food, max_food);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Food;
                break;
            }
        }
    }
}
void generate_deamon(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_deamons, max_deamons);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                Monster *monster = malloc(sizeof(Monster));
                monster->active = 0;
                monster->following = 0;
                monster->health = monsters_health[Deamon];
                monster->type = Deamon;
                monster->x = x;
                monster->y = y;
                game->floors[id]->monsters[game->floors[id]->monster_count++] = monster;
                break;
            }
        }
    }
}
void generate_fire_breathing(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_fire_breathing, max_fire_breathing);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                Monster *monster = malloc(sizeof(Monster));
                monster->active = 0;
                monster->following = 0;
                monster->health = monsters_health[Fire_Breathing];
                monster->type = Fire_Breathing;
                monster->x = x;
                monster->y = y;
                game->floors[id]->monsters[game->floors[id]->monster_count++] = monster;
                break;
            }
        }
    }
}
void generate_giant(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_giant, max_giant);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                Monster *monster = malloc(sizeof(Monster));
                monster->active = 0;
                monster->following = 0;
                monster->health = monsters_health[Giant];
                monster->type = Giant;
                monster->x = x;
                monster->y = y;
                game->floors[id]->monsters[game->floors[id]->monster_count++] = monster;
                break;
            }
        }
    }
}
void generate_snake(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_snake, max_snake);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                Monster *monster = malloc(sizeof(Monster));
                monster->active = 0;
                monster->following = 0;
                monster->health = monsters_health[Snake];
                monster->type = Snake;
                monster->x = x;
                monster->y = y;
                game->floors[id]->monsters[game->floors[id]->monster_count++] = monster;
                break;
            }
        }
    }
}
void generate_undeed(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(min_undeed, max_undeed);
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                Monster *monster = malloc(sizeof(Monster));
                monster->active = 0;
                monster->following = 0;
                monster->health = monsters_health[Undeed];
                monster->type = Undeed;
                monster->x = x;
                monster->y = y;
                game->floors[id]->monsters[game->floors[id]->monster_count++] = monster;
                break;
            }
        }
    }
}
void generate_black_golds(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(1, 10);
        if (j > black_gold_prob)
            continue;
        while (1)
        {
            int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
            int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
            if (game->floors[id]->map[x][y]->type != Floor)
                continue;
            game->floors[id]->map[x][y]->type = Black_Gold;
            break;
        }
    }
}
void generate_traps(int id)
{
    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int j = random_num(number_of_traps_min, number_of_traps_max);
        if(id == game->floor_count - 1 && i == treasure_room)
            j += 10;
        while (j--)
        {
            while (1)
            {
                int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
                int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
                if (game->floors[id]->map[x][y]->type != Floor)
                    continue;
                game->floors[id]->map[x][y]->type = Trap;
                break;
            }
        }
    }
}
void generate_stair_up(int id)
{
    int i = random_num(0, game->floors[id]->number_of_rooms - 1);
    while (1)
    {
        int x = rand() % (game->floors[id]->rooms[i]->h - 4) + game->floors[id]->rooms[i]->startx + 2;
        int y = rand() % (game->floors[id]->rooms[i]->w - 4) + game->floors[id]->rooms[i]->starty + 2;
        if (game->floors[id]->map[x][y]->type != Floor)
            continue;
        game->floors[id]->map[x][y]->type = StairUp;
        break;
    }
}
int check_room_overlap(int startx, int starty, int w, int h, int id)
{
    if (startx + h - 1 >= LINES - 2 || starty + w - 1 >= COLS - 2)
        return 1;

    for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
    {
        int stx = game->floors[id]->rooms[i]->startx, wx = game->floors[id]->rooms[i]->w;
        int sty = game->floors[id]->rooms[i]->starty, hx = game->floors[id]->rooms[i]->h;
        int xcolid = 1, ycolid = 1;
        if (startx > stx + hx - 1 || startx + h - 1 < stx)
            xcolid = 0;
        if (starty > sty + wx - 1 || starty + w - 1 < sty)
            ycolid = 0;

        // Room shouldn't be too close
        if (abs(startx - (stx + hx - 1)) <= rooms_min_distancex || abs(startx + h - 1 - stx) <= rooms_min_distancex)
            return 1;
        if (abs(starty - (sty + wx - 1)) <= rooms_min_distancey || abs(starty + w - 1 - sty) <= rooms_min_distancey)
            return 1;
        if (xcolid && ycolid)
            return 1;
    }
    return 0;
}
void generate_corridors(int id)
{
    int same[N][N];
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            same[i][j] = 0;
    while (!is_game_connected(id))
    {
        int mmn = inf, mx = 0;
        for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
        {
            if (same[i][i] < mmn)
                mmn = same[i][i];
            if (same[i][i] > mx)
                mx = same[i][i];
        }
        int u = rand() % game->floors[id]->number_of_rooms;
        refresh();
        if (same[u][u] != mmn && mx > mmn)
            continue;

        int mn = inf, v = -1;
        for (int i = 0; i < game->floors[id]->number_of_rooms; i++)
            if (u != i && bfs(u, i, id) < mn && !same[u][i])
                v = i, mn = bfs(u, i, id);

        if (mn == inf)
            continue;
        same[u][u]++;

        same[u][v] = same[v][u] = 1;
        make_corridor(u, v, id);
        refresh();
    }
}
void make_corridor(int u, int v, int id)
{
    int queue[N * N];
    int dist[N][N], par[N][N];
    int sx, sy;
    for (int i = 0; i < game->floors[id]->rooms[u]->h; i++)
        for (int j = 0; j < game->floors[id]->rooms[u]->w; j++)
            if (game->floors[id]->map[i + game->floors[id]->rooms[u]->startx][j + game->floors[id]->rooms[u]->starty]->type == Floor)
                sx = i + game->floors[id]->rooms[u]->startx, sy = j + game->floors[id]->rooms[u]->starty;
    memset(dist, 63, sizeof(dist));
    dist[sx][sy] = 0, queue[0] = sx * N + sy;
    par[sx][sy] = -1;
    int f = 0, qsz = 1;

    while (f < qsz)
    {
        int x = queue[f] / N, y = queue[f] % N;
        f++;
        for (int j = 0; j < CORRIDOR_RANDOMNESS; j++)
        {
            int i = rand() % 4;
            int nx = dx[i] + x, ny = dy[i] + y;
            if (dist[nx][ny] > dist[x][y] + 1 && nx >= Padding && ny >= Padding && nx < LINES - Padding && ny < COLS - Padding)
            {
                // Prevent moving through walls
                if (game->floors[id]->map[x][y]->type == Wall && game->floors[id]->map[nx][ny]->type == Wall)
                    continue;
                dist[nx][ny] = dist[x][y] + 1;
                par[nx][ny] = x * N + y;
                queue[qsz++] = nx * N + ny;
            }
        }
    }
    int distx = game->floors[id]->rooms[v]->startx + game->floors[id]->rooms[v]->h / 2;
    int disty = game->floors[id]->rooms[v]->starty + game->floors[id]->rooms[v]->w / 2;

    while (~par[distx][disty])
    {

        // Draw corridors
        if (game->floors[id]->map[distx][disty]->type != Floor && game->floors[id]->map[distx][disty]->type != Pillar)
        {
            if (game->floors[id]->map[distx][disty]->type == Wall || game->floors[id]->map[distx][disty]->type == Door)
            {
                game->floors[id]->map[distx][disty]->type = Door;
            }
            else
            {
                game->floors[id]->map[distx][disty]->type = Corridor;
            }
        }
        int p = par[distx][disty];
        distx = p / N, disty = p % N;
    }
}
// get distance betweeen room u and v
int bfs(int u, int v, int id)
{
    int sx = game->floors[id]->rooms[u]->startx + game->floors[id]->rooms[u]->h / 2;
    int sy = game->floors[id]->rooms[u]->starty + game->floors[id]->rooms[u]->w / 2;
    int dx = game->floors[id]->rooms[v]->startx + game->floors[id]->rooms[v]->h / 2;
    int dy = game->floors[id]->rooms[v]->starty + game->floors[id]->rooms[v]->w / 2;
    return abs(sx - dx) + abs(dy - sy);
}
int mark[N][N];
int is_game_connected(int id)
{
    memset(mark, 0, sizeof(mark));
    int sx, sy;
    for (int i = 0; i < game->floors[id]->rooms[0]->h; i++)
        for (int j = 0; j < game->floors[id]->rooms[0]->w; j++)
            if (game->floors[id]->map[i + game->floors[id]->rooms[0]->startx][j + game->floors[id]->rooms[0]->starty]->type == Floor)
                sx = i + game->floors[id]->rooms[0]->startx, sy = j + game->floors[id]->rooms[0]->starty;
    dfs(sx, sy, id);
    for (int i = 0; i < LINES; i++)
        for (int j = 0; j < COLS; j++)
            if (game->floors[id]->map[i][j]->type != Nothing && game->floors[id]->map[i][j]->type != Pillar && !mark[i][j])
                return 0;
    return 1;
}
void dfs(int x, int y, int id)
{
    mark[x][y] = 1;
    for (int i = 0; i < 4; i++)
    {
        int nx = dx[i] + x, ny = y + dy[i];
        if (!mark[nx][ny] && nx >= 0 && ny >= 0 && nx < LINES && ny < COLS && game->floors[id]->map[nx][ny]->type != Nothing && game->floors[id]->map[nx][ny]->type != Pillar)
            dfs(nx, ny, id);
    }
}