#ifndef UNSTABLE_STUDENTS_EFFECT_H
#define UNSTABLE_STUDENTS_EFFECT_H

#include "../model/structs.h"

bool has_effect(Effect* effects, int num_effects, Action action, Type_Player target_player, Type_card target_card);
void check_effect(Player* owner_card, Card* card, When when_activate, Card** draw_deck, Card** discard_deck);
void activate_effect(Card* card, Effect* effect, Player* player, Player* target_player, Card** draw_deck, Card** discard_deck);
bool can_block_effect(Player* player, Card* card);
bool ask_block_effect(Player* player, Card* card);
Card* block_effect(Player* player, Card* card, Card** discard_deck);

void gioca_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void scarta_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void elimina_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void ruba_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void pesca_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void prendi_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);
void scambia_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck);

#endif