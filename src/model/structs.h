#ifndef UNSTABLE_UNICORN_STRUCTS_H
#define UNSTABLE_UNICORN_STRUCTS_H

#include "colors.h"
#include "constants.h"
#include "enums.h"
#include <stdbool.h>

typedef struct {
    Action action_effect;
    Type_Player target_player;
    Type_card target_card;
} Effect;

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    char description[MAX_DESCRIPTION_LENGTH + 1];
    Type_card type;
    int num_effects;
    Effect* effects;
    When when_activate;
    bool optional;
    struct Card* next_card;
} Card;

typedef struct {
    char name[MAX_NAME_LENGTH + 1];
    Card* hand;
    Card* classroom;
    Card* magic_cards;
    struct Player* next_player;
} Player;

#endif