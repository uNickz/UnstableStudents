#ifndef UNSTABLE_STUDENTS_GAME_H
#define UNSTABLE_STUDENTS_GAME_H

#include "../model/structs.h"

void init_new_game(char*** registered_saves, int* num_registered_saves);
void play_game(const char* game_name, Player* players, Card* draw_deck, Card* discard_deck, Card* study_room);
bool check_win_condition(Player* player);

#endif