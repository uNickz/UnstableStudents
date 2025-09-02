#ifndef UNSTABLE_STUDENTS_CARD_H
#define UNSTABLE_STUDENTS_CARD_H

#include "../model/structs.h"
#include "../utils/utils.h"
#include <stdbool.h>
#include <stdio.h>

Card* load_cards(const char* filename, int* num_cards, int* num_unique_cards);
Card* read_card(FILE* file);
void free_card(Card* card);
void free_deck(Card* deck);
void copy_card(Card* src, Card* dst);
Card* add_card(Card* head, Card* card);
char* get_type_card(Type_card type);
char* get_parsed_type_card(Type_card type);
char* get_color_by_type(Type_card type);
int calculate_max_row_length_for_description(const Card* deck);
char** format_card(int* buffer_rows, const Card* card, int max_rows_description, int card_index, int total_cards);
char** format_hidden_card(int* buffer_rows, int card_index, int total_cards);
void print_card(const Card* card);
char** format_deck(int* buffer_rows, const Card* deck, const char* deck_name, int offset_card_index, bool hidden, char* color);
void print_deck(const Card* deck, const char* deck_name, int offset_card_index, bool hidden, char* color);
int count_cards(Card* deck);
Card* shuffle_deck(Card* deck, int num_cards);
void separate_matricola_cards(Card** deck, Card** study_room);
void draw_card(Player** player, Card** draw_deck, Card** discard_deck, bool show_card);
Card* draw_matricola_card(Card* player_classroom, Card** study_room);
int choice_card(Card* deck, char* msg, char* error_msg);
Card* select_card(int card_index, Card** deck, bool unlink_card);
void discard_card(Card* card, Card** discard_deck);
void select_and_discard_card(int card_index, Card** player_hand, Card** discard_deck);
bool deck_contains_card(const Card* deck, const Card* card);
bool deck_contains_type(const Card* deck, Type_card required_type);
bool deck_contains_effect(const Card* deck, Action action, Type_Player target_player, Type_card target_card);

#endif