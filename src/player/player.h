#ifndef UNSTABLE_STUDENTS_PLAYER_H
#define UNSTABLE_STUDENTS_PLAYER_H

#include "../model/structs.h"
#include "../utils/utils.h"
#include <stdio.h>

Player* add_player(Player* head, const char* name);
int count_players(Player* head);
void free_players(Player* head);
char** format_player(int* buffer_rows, Player* player, bool is_self, char* color);
void print_player(Player* player, bool is_self, char* color);
Player* choose_player(Player* players, bool with_me, const char* msg_player, const char* msg_index, const char* error_msg);
void init_hand(Player* player, Card** draw_deck, Card** study_room, Card** discard_deck);
void check_hand(Player* player, char* color, Card** discard_deck);
bool can_play_card(Player* player, Card* card);
int count_playable_cards(Player* player, Type_card force_card_type);
void play_hand(Player* player, Type_card force_card_type, Card** draw_deck, Card** discard_deck, bool show_card);

#endif