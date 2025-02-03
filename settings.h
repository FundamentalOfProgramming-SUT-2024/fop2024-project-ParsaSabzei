#ifndef SETTINGS_H
#define SETTINGS_H

enum Difficulity_Types{
    Easy,
    Normal,
    Hard
};

char* Difficulty_Names[] = {"Easy", "Normal", "Hard"};

typedef struct Settings{
    int Player_Color;
    enum Difficulity_Types Difficulty;
} Settings;
#endif