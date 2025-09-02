#ifndef UNSTABLE_STUDENTS_LOGGING_H
#define UNSTABLE_STUDENTS_LOGGING_H

#include "../model/structs.h"
#include <stdio.h>

FILE* init_log();
void log_init_game(const char* game_name);
void log_load_game(const char* game_name);
int log_prefix_round(bool next_round, bool reset);
void log_draw_card(const Player* player, const Card* card);
void log_play_card(const Player* player, const Card* card);
void log_discard_card(const Player* player, const Card* card);
void log_block_effect(const Player* player, const Card* card_used, const Card* card_blocked);
void log_scarta_effect(const Player* player, const Player* target_player, const Card* card);
void log_elimina_effect(const Player* player, const Player* target_player, const Card* card);
void log_ruba_effect(const Player* player, const Player* target_player, const Card* card);
void log_prendi_effect(const Player* player, const Player* target_player, const Card* card);
void log_scambia_effect(const Player* player, const Player* target_player);
void log_winner_game(const Player* player);

#endif