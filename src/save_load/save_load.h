#ifndef UNSTABLE_STUDENTS_SAVE_LOAD_H
#define UNSTABLE_STUDENTS_SAVE_LOAD_H

#include "../model/structs.h"
#include <stdio.h>

Card* load_saved_deck(FILE* file, int num_cards);
Player* load_saved_players(FILE* file, int num_players);
void load_saved_game(const char* path_file, const char* game_name);

void save_deck(FILE* file, Card* deck);
void save_players(FILE* file, Player* players);
void save_game(char* filename, Player* players, Card* draw_deck, Card* discard_deck, Card* study_room);

bool is_already_registered(const char* save_name, const char** registered_saves, int num_registered_saves);
char** register_new_save(const char* filename, char* save_name, char** registered_saves, int* num_registered_saves);
char** load_registered_saves(const char* filename, int* num_lines);
void free_registered_saves(char** lines, int num_lines);

#endif