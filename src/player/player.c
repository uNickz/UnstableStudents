#include "player.h"

#include "../card/card.h"
#include "../effect/effect.h"
#include "../logging/logging.h"

/**
 * @brief Aggiunge un nuovo giocatore alla lista dei giocatori.
 * 
 * @param head Puntaore alla testa della lista dei giocatori.
 * @param name Nome del nuovo giocatore.
 * @return Player* Puntatore alla testa della lista dei giocatori.
 */
Player* add_player(Player* head, const char* name) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Alloca un nuovo giocatore
    Player* new_player = (Player*) safe_malloc(sizeof(Player));

    // Inizializza i campi del nuovo giocatore
    strncpy(new_player->name, name, MAX_NAME_LENGTH);
    new_player->hand = NULL;
    new_player->classroom = NULL;
    new_player->magic_cards = NULL;

    // SE è il primo giocatore: la lista è vuota
    if (head == NULL) {
        new_player->next_player = new_player; // Puntatore circolare
        return new_player; // Ritorna il nuovo giocatore come testa
    }

    // Trova l'ultimo giocatore
    Player* current = head;
    while (current->next_player != head) {
        current = current->next_player;
    }

    // Collega il nuovo giocatore alla fine della lista
    current->next_player = new_player;
    new_player->next_player = head; // Puntatore circolare

    return head; // La testa rimane invariata
}

/**
 * @brief Conta il numero di giocatori presenti nella lista circolare.
 * 
 * @param head Puntatore alla testa della lista dei giocatori.
 * @return int Numero di giocatori presenti nella lista.
 */
int count_players(Player* head) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // SE la lista è vuota
    if (head == NULL) {
        return 0;
    }

    int count = 0; // Inizializza il conteggio dei giocatori
    Player* current = head; // Inizializza il giocatore corrente
    do {
        count++; // Incrementa il conteggio dei giocatori
        current = current->next_player; // Passa al prossimo giocatore
    } while (current != head); // Continua finché non torna alla testa

    return count;
}

/**
 * @brief Libera la memoria allocata per la lista circolare dei giocatori.
 * 
 * @param head Puntatore alla testa della lista dei giocatori.
 */
void free_players(Player* head) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // SE la lista è vuota
    if (head == NULL) {
        return;
    }

    Player* current = head; // Inizializza il giocatore corrente
    Player* next_player = NULL; // Inizializza il prossimo giocatore

    do {
        next_player = current->next_player; // Salva il prossimo giocatore

        free_deck(current->hand); // Libera il mazzo delle carte in mano
        current->hand = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione

        free_deck(current->classroom); // Libera il mazzo delle carte dell'aula studio
        current->classroom = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione

        free_deck(current->magic_cards); // Libera il mazzo delle carte bonus/malus
        current->magic_cards = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione

        free(current); // Libera la memoria allocata per il giocatore

        current = next_player; // Passa al prossimo giocatore
    } while (current != head); // Continua finché non torna alla testa

    return;
}

/**
 * @brief Formatta graficamente un giocatore in un buffer di righe.
 * 
 * @param buffer_rows Puntaore per restituire il numero di righe del buffer.
 * @param player Puntatore al giocatore da formattare.
 * @param is_self Flag per indicare se il giocatore è il giocatore corrente.
 * @param color Colore del giocatore.
 * @return char** Buffer di righe contenente il giocatore formattato.
 */
char** format_player(int* buffer_rows, Player* player, bool is_self, char* color) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // SE il colore non è stato specificato
    if (color == NULL) {
        color = WHT; // Imposta il colore bianco
    }

    char** buffer = NULL; // Inizializza il buffer di righe
    int player_rows = BASE_PLAYER_HEIGHT; // Inizializza il numero di righe del buffer

    // Formattazione del mazzo delle carte bonus/malus
    int deck_magic_rows = 0;
    int dack_magic_color_size = strlen(MAGIC_DECK_COLOR) + strlen(RESET);
    char** deck_magic_buffer = format_deck(&deck_magic_rows, player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR);
    int deck_magic_max_width = strlen(deck_magic_buffer[deck_magic_rows-1]) - dack_magic_color_size;
    player_rows += deck_magic_rows;

    // Formattazione del mazzo delle carte dell'aula studio
    int deck_classroom_rows = 0;
    int deck_classroom_color_size = strlen(CLASSROOM_DECK_COLOR) + strlen(RESET);
    char** deck_classroom_buffer = format_deck(&deck_classroom_rows, player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR);
    int deck_classroom_max_width = strlen(deck_classroom_buffer[deck_classroom_rows-1]) - deck_classroom_color_size;
    player_rows += deck_classroom_rows;

    // Formattazione del mazzo delle carte in mano
    int deck_hand_rows = 0;
    int deck_hand_color_size = strlen(HAND_DECK_COLOR) + strlen(RESET);
    char** deck_hand_buffer = format_deck(&deck_hand_rows, player->hand, "Carte in Mano", 0, !is_self, HAND_DECK_COLOR);
    int deck_hand_max_width = strlen(deck_hand_buffer[deck_hand_rows-1]) - deck_hand_color_size;
    player_rows += deck_hand_rows;

    // Allocazione del buffer di righe per la formatazione grafica del giocatore
    buffer = (char**) safe_calloc(player_rows, sizeof(char*));
    *buffer_rows = player_rows;

    // Costruzione dei separatori e calcolo del padding
    int edges_width = strlen(UNICODE_BORDER_UP_LEFT) + strlen(UNICODE_BORDER_UP_RIGHT);
    int max_row_width = max(deck_magic_max_width, max(deck_classroom_max_width, deck_hand_max_width));
    int max_colored_row_width = (max(strlen(MAGIC_DECK_COLOR), max(strlen(CLASSROOM_DECK_COLOR), strlen(HAND_DECK_COLOR))) + strlen(RESET)) * (DECK_PADDING + COLOR_PADDING + 10);
    int player_width = max_row_width + edges_width + ((PLAYER_PADDING+PLAYER_PADDING) * strlen(UNICODE_BORDER_HORIZONTAL));
    int colored_player_width = player_width + max_colored_row_width;

    int real_player_width = (player_width-edges_width)/strlen(UNICODE_BORDER_HORIZONTAL);

    char* player_border = repeat_string(UNICODE_BORDER_HORIZONTAL, real_player_width);
    char* player_line_separator = repeat_char(' ', real_player_width);

    // Per ogni riga del buffer
    for (int i = 0; i < player_rows; i++) {
        // Alloca la memoria per ogni riga del buffer
        buffer[i] = (char*) safe_calloc(colored_player_width + 1, sizeof(char));
        buffer[i][0] = '\0'; // Inizializza la stringa
    }

    // Costruzione del buffer
    int buffer_index = 0;

    char* title_suffix = "'s POV";
    char* player_name = (char*) safe_malloc((strlen(player->name) + strlen(title_suffix) + 1) * sizeof(char));
    sprintf(player_name, "%s%s", player->name, title_suffix);
    char* padded_player_name = padding_string(player_name, real_player_width);

    sprintf(buffer[buffer_index++], "%s%s%s%s", color, BOLD, padded_player_name, RESET);
    sprintf(buffer[buffer_index++], "%s%s%s%s%s", color, UNICODE_BORDER_UP_LEFT, player_border, UNICODE_BORDER_UP_RIGHT, RESET);
    sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, player_line_separator, color, UNICODE_BORDER_VERTICAL, RESET);

    // Calcolo del padding per allineare le carte al centro del box delle carte bonus/malus 
    bool need_padding = deck_magic_max_width < max_row_width;
    for (int i = 0; i < deck_magic_rows; i++) {
        if (need_padding) {
            int padding = calculate_padding(max_row_width/3, deck_magic_max_width/3);

            char* padding_left = repeat_char(' ', padding);
            char* padded_right = repeat_char(' ', (max_row_width/3)-(deck_magic_max_width/3)-padding);

            sprintf(buffer[buffer_index++], "%s%s%s   %s%s%s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, padding_left, deck_magic_buffer[i], padded_right, color, UNICODE_BORDER_VERTICAL, RESET);
            free(padding_left); // Libera la memoria allocata per il padding sinistro
            free(padded_right); // Libera la memoria allocata per il padding destro
        } else {
            sprintf(buffer[buffer_index++], "%s%s%s   %s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, deck_magic_buffer[i], color, UNICODE_BORDER_VERTICAL, RESET);
        }
        free(deck_magic_buffer[i]); // Libera la memoria allocata per la riga del buffer del mazzo delle carte bonus/malus
    }
    free(deck_magic_buffer); // Libera la memoria allocata per il buffer del mazzo delle carte bonus/malus

    sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, player_line_separator, color, UNICODE_BORDER_VERTICAL, RESET);

    // Calcolo del padding per allineare le carte al centro del box delle carte dell'aula studio 
    need_padding = deck_classroom_max_width < max_row_width;
    for (int i = 0; i < deck_classroom_rows; i++) {
        if (need_padding) {
            int padding = calculate_padding(max_row_width/3, deck_classroom_max_width/3);

            char* padding_left = repeat_char(' ', padding);
            char* padded_right = repeat_char(' ', (max_row_width/3)-(deck_classroom_max_width/3)-padding);

            sprintf(buffer[buffer_index++], "%s%s%s   %s%s%s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, padding_left, deck_classroom_buffer[i], padded_right, color, UNICODE_BORDER_VERTICAL, RESET);
            free(padding_left); // Libera la memoria allocata per il padding sinistro
            free(padded_right); // Libera la memoria allocata per il padding destro
        } else {
            sprintf(buffer[buffer_index++], "%s%s%s   %s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, deck_classroom_buffer[i], color, UNICODE_BORDER_VERTICAL, RESET);
        }
        free(deck_classroom_buffer[i]); // Libera la memoria allocata per la riga del buffer del mazzo delle carte dell'aula studio
    }
    free(deck_classroom_buffer); // Libera la memoria allocata per il buffer del mazzo delle carte dell'aula studio

    sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, player_line_separator, color, UNICODE_BORDER_VERTICAL, RESET);

    // Calcolo del padding per allineare le carte al centro del box delle carte della mano
    need_padding = deck_hand_max_width < max_row_width;
    for (int i = 0; i < deck_hand_rows; i++) {
        if (need_padding) {
            int padding = calculate_padding(max_row_width/3, deck_hand_max_width/3);

            char* padding_left = repeat_char(' ', padding);
            char* padded_right = repeat_char(' ', (max_row_width/3)-(deck_hand_max_width/3)-padding);

            sprintf(buffer[buffer_index++], "%s%s%s   %s%s%s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, padding_left, deck_hand_buffer[i], padded_right, color, UNICODE_BORDER_VERTICAL, RESET);
            free(padding_left); // Libera la memoria allocata per il padding sinistro
            free(padded_right); // Libera la memoria allocata per il padding destro
        } else {
            sprintf(buffer[buffer_index++], "%s%s%s   %s   %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, deck_hand_buffer[i], color, UNICODE_BORDER_VERTICAL, RESET);
        }
        free(deck_hand_buffer[i]); // Libera la memoria allocata per la riga del buffer del mazzo delle carte in mano
    }
    free(deck_hand_buffer); // Libera la memoria allocata per il buffer del mazzo delle carte in mano

    sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, player_line_separator, color, UNICODE_BORDER_VERTICAL, RESET);
    sprintf(buffer[buffer_index++], "%s%s%s%s%s", color, UNICODE_BORDER_DOWN_LEFT, player_border, UNICODE_BORDER_DOWN_RIGHT, RESET);

    // Liberazione della memoria allocata dinamicamente
    free(player_name);
    free(padded_player_name);
    free(player_border);
    free(player_line_separator);

    return buffer;
}

/**
 * @brief Stampa un giocatore.
 * 
 * @param player Puntaore al giocatore da stampare.
 * @param is_self Flag per indicare se il giocatore è il giocatore corrente.
 * @param color Colore del giocatore.
 */
void print_player(Player* player, bool is_self, char* color) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Formatta il giocatore in un buffer di righe
    int buffer_rows = 0;
    char** buffer = format_player(&buffer_rows, player, is_self, color);

    // Stampa il giocatore
    for (int i = 0; i < buffer_rows; i++) {
        printf("%s\n", buffer[i]);
        free(buffer[i]); // Libera la memoria allocata per la riga
        buffer[i] = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione
    }

    free(buffer); // Libera la memoria allocata per il buffer
    return;
}

/**
 * @brief Fa scegliere all'utente un giocatore dalla lista circolare dei giocatori.
 * 
 * @param players Puntatore alla testa della lista dei giocatori.
 * @param with_me Flag per indicare se includere il giocatore corrente.
 * @param msg_player Messaggio per chiedere di scegliere un giocatore.
 * @param msg_index Messaggio per chiedere l'indice del giocatore.
 * @param error_msg Messaggio di errore per indice non valido.
 * @return Player* Puntatore al giocatore scelto.
 */
Player* choose_player(Player* players, bool with_me, const char* msg_player, const char* msg_index, const char* error_msg) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = players; // Inizializza il giocatore corrente
    int num_players = 1; // Inizializza il numero di giocatori

    printf(msg_player); // Stampa il messaggio per la scelta del giocatore

    // SE il giocatore corrente deve essere incluso
    if (with_me) {
        // Stampa il giocatore corrente come opzione e incrementa il numero di giocatori
        printf("  %d. %s (IO)\n", num_players++, current_player->name);
        current_player = current_player->next_player; // Passa al prossimo giocatore
    } else {
        current_player = current_player->next_player; // Passa al prossimo giocatore saltando il giocatore corrente
    }

    // Per ogni giocatore nella lista circolare
    while (current_player != players) {
        printf("  %d. %s\n", num_players++, current_player->name); // Stampa il giocatore come opzione e incrementa il numero di giocatori
        current_player = current_player->next_player; // Passa al prossimo giocatore
    }

    int player_index; // Inizializza l'indice del giocatore scelto
    do {
        printf(msg_index); // Stampa il messaggio per l'indice del giocatore
        read_int(&player_index); // Legge l'indice del giocatore selezionato

        // SE l'indice del giocatore selezionato non è valido
        if (player_index < 1 || player_index > num_players) {
            printf(error_msg); // Stampa il messaggio di errore
        }
    } while (player_index < 1 || player_index > num_players);

    // Inizializza il contatore dei giocatori (tenendo della presenza del giocatore corrente o meno)
    int ctr = 1 - !with_me;

    // Scorro la lista circolare fino al giocatore selezionato
    while (ctr < player_index) {
        current_player = current_player->next_player; // Passa al prossimo giocatore
        ctr++; // Incrementa il contatore dei giocatori
    }

    return current_player;
}

/**
 * @brief Inizializza le carte in mano e dell'aula studio per ogni giocatore.
 * 
 * @param player Puntatore alla testa della lista dei giocatori.
 * @param draw_deck Puntatore al mazzo delle carte da cui pescare.
 * @param study_room Puntatore al mazzo delle carte dell'aula studio.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void init_hand(Player* player, Card** draw_deck, Card** study_room, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player; // Puntatore al giocatore corrente

    // Pesca N carte per ogni giocatore
    for (int i = 0; i < NUM_STARTING_CARDS; i++) {
        current_player = player; // Inizializza il giocatore corrente
        do {
            draw_card(&current_player, draw_deck, discard_deck, false); // Pesca una carta
            current_player = current_player->next_player; // Passa al prossimo giocatore
        } while (current_player != player); // Continua finché non torna al giocatore iniziale
    }

    // Ogni giocatore pesca M carte Studente MATRICOLA  dall'aula studio
    for (int i = 0; i < NUM_STARTING_MATRICOLA_CARDS; i++) {
        current_player = player; // Inizializza il giocatore corrente
        do {
            current_player->classroom = draw_matricola_card(current_player->classroom, study_room); // Pesca una carta MATRICOLA 
            current_player = current_player->next_player; // Passa al prossimo giocatore
        } while (current_player != player); // Continua finché non torna al giocatore iniziale
    }

    return;
}

/**
 * @brief Controlla se il numero di carte in mano di un giocatore è maggiore di MAX_HAND_SIZE, in tal caso scarta le carte in eccesso.
 * 
 * @param player Puntatore al giocatore.
 * @param player_color Colore del giocatore.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void check_hand(Player* player, char* player_color, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_cards = count_cards(player->hand); // Numero di carte in mano
    Card* selected_card = NULL; // Inizializza la carta selezionata

    // SE il giocatore ha un numero di carte in mano inferiore o uguale a MAX_HAND_SIZE => non fa nulla
    if (num_cards <= MAX_HAND_SIZE) {
        return;
    }

    // SE il colore non è stato specificato
    if (player_color == NULL) {
        player_color = WHT; // Imposta il colore bianco
    }

    // Se il giocatore ha più di MAX_HAND_SIZE carte in mano => scarta le carte in eccesso
    int cards_to_discard = num_cards - MAX_HAND_SIZE; // Numero di carte da scartare
    printf("\n[" RED "!" RESET "] %s%s%s hai troppe carte in mano! Devi scartare %d carte.\n", player_color, player->name, RESET, cards_to_discard);

    for (int i = 0; i < cards_to_discard; i++) {
        printf("\n[" HBLU "i" RESET "] %s%s%s scegli una carta da scartare:\n\n", player_color, player->name, RESET);
        print_deck(player->hand, "Carte in Mano", 0, false, HAND_DECK_COLOR); // Stampa le carte in mano

        // Chiede all'utente di scegliere una carta da scartare
        int card_index = choice_card(player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da scartare: ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
        selected_card = select_card(card_index, &player->hand, false); // Seleziona la carta scelta

        printf("\n[" RED "-" RESET "] %s hai scartato una carta dalla tua mano!\n", player->name);
        print_card(selected_card); // Stampa la carta scartata

        log_discard_card(player, selected_card); // Registra l'azione di scarto della carta nel file di log

        select_and_discard_card(card_index, &player->hand, discard_deck); // Seleziona e scarta la carta selezionata
    }

    return;
}

/**
 * @brief Controlla se una carta può essere giocata da un giocatore.
 * 
 * @param player Puntatore al giocatore.
 * @param card Puntatore alla carta da giocare.
 * @return true Se la carta può essere giocata, false altrimenti.
 */
bool can_play_card(Player* player, Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Controlla se nella aula studio o bonus/malus è presenta qualche carta
    // che ha come effetto l'impedimento di giocare la carta selezionata
    bool can_play = !(
        deck_contains_effect(player->magic_cards, IMPEDIRE, IO, card->type) || deck_contains_effect(player->magic_cards, IMPEDIRE, IO, ALL)
        || deck_contains_effect(player->classroom, IMPEDIRE, IO, card->type) || deck_contains_effect(player->classroom, IMPEDIRE, IO, ALL)
    );

    return can_play;
}

/**
 * @brief Conta il numero di carte giocabili dalla mano di un giocatore.
 * 
 * @param player Puntatore al giocatore.
 * @param force_card_type Tipo di carta obbligatorio da giocare.
 * @return int Numero di carte giocabili.
 */
int count_playable_cards(Player* player, Type_card force_card_type) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_playable_cards = 0; // Inizializza il numero di carte giocabili

    // Scorre tutte le carte in mano del giocatore
    for (Card* current_card = player->hand; current_card != NULL; current_card = current_card->next_card) {
        if (
            can_play_card(player, current_card) && ( // SE la carta può essere giocata
                force_card_type == ALL || current_card->type == force_card_type || ( // SE possono essere giocate tutte le carte o la carta è del tipo richiesto
                    force_card_type == STUDENTE && ( // SE la carta richiesta è di tipo STUDENTE e la carta è di tipo MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                        current_card->type == MATRICOLA || current_card->type == STUDENTE_SEMPLICE || current_card->type == LAUREANDO
                    )
                )
            )
        ) {
            num_playable_cards++; // Incrementa il numero di carte giocabili
        }
    }

    return num_playable_cards;
}

/**
 * @brief Fa giocare una carta dalla mano di un giocatore.
 * 
 * @param player Puntatore al giocatore.
 * @param force_card_type Tipo di carta obbligatorio da giocare.
 * @param draw_deck Puntatore al mazzo delle carte da cui pescare.
 * @param discard_deck Puntatore al mazzo degli scarti.
 * @param show_card Flag per indicare se mostrare la carta giocata.
 */
void play_hand(Player* player, Type_card force_card_type, Card** draw_deck, Card** discard_deck, bool show_card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    printf("\n[" HBLU "i" RESET "] %s scegli una carta da giocare:\n\n", player->name);
    print_deck(player->hand, "Carte in Mano", 0, false, HAND_DECK_COLOR); // Stampa le carte in mano

    // Inizializza le variabili per la scelta della carta da giocare
    int card_index = 0;
    Card* played_card = NULL;
    bool is_valid_choice = false;

    do {
        // Chiede all'utente di scegliere una carta da giocare
        card_index = choice_card(player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da giocare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
        played_card = select_card(card_index, &player->hand, false); // Seleziona la carta scelta

        // SE la carta non può essere giocata
        if (!can_play_card(player, played_card)) {
            printf("\n[" RED "!" RESET "] %s non puoi giocare la carta \"%s%s%s\" perchè è presente un MALUS che te lo impedisce! Riprova!\n", player->name, get_color_by_type(played_card->type), played_card->name, RESET);
        } else if (!( // SE la carta non è del tipo richiesto
            force_card_type == ALL || played_card->type == force_card_type || ( // SE possono essere giocate tutte le carte o la carta è del tipo richiesto
                force_card_type == STUDENTE && ( // SE la carta richiesta è di tipo STUDENTE e la carta è di tipo MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                    played_card->type == MATRICOLA || played_card->type == STUDENTE_SEMPLICE || played_card->type == LAUREANDO
                )
            )
        )) {
            printf("\n[" RED "!" RESET "] %s non puoi giocare la carta \"%s%s%s\" perchè sei obbligato a giocare una carta di tipo \"%s\"! Riprova!\n", player->name, get_color_by_type(played_card->type), played_card->name, RESET, get_type_card(force_card_type));
        } else {
            is_valid_choice = true;
            // Scollega effettivamente la carta dalla mano del giocatore
            played_card = select_card(card_index, &player->hand, true);
        }

    } while (!is_valid_choice); // Continua finché non viene scelta una carta valida

    log_play_card(player, played_card); // Registra la carta giocata nel file di log
    
    // SE la carta deve essere mostrata
    if (show_card == true) {
        printf("\n[" GRN "+" RESET "] Hai giocato una carta!\n");
        print_card(played_card); // Stampa la carta giocata
    }

    switch (played_card->type) {
        case MATRICOLA:
        case STUDENTE_SEMPLICE:
        case STUDENTE:
        case LAUREANDO:
            // Non è possibile inserire due carte identiche nella propria aula (o bonusmalus) o quella di un altro giocatore.
            // SE la carta giocata è già presente nell'aula studio
            // => non è possibile giocare la carta => la carta viene scartata
            if (deck_contains_card(player->classroom, played_card)) {
                printf("\n[" RED "!" RESET "] È già presente una carta identica nella tua aula studio! La carta verrà scartata!\n");
                discard_card(played_card, discard_deck);
            } else {
                // Aggiunge la carta studente giocata alla propria aula
                player->classroom = add_card(player->classroom, played_card);
                check_effect(player, played_card, SUBITO, draw_deck, discard_deck); // Controlla e attiva eventuali effetti della carta giocata
            }
            break;

        case BONUS:
        case MALUS:
            // Carte BONUS e MALUS possono essere applicate a qualsiasi giocatore, mentre tutte le altre solo su se stessi.

            // Chiedere all'utente a quale giocatore applicare la carta bonus/malus
            Player* target_player = choose_player(player, true, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui assegnare la carta:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

            // Non è possibile inserire due carte identiche nella propria aula (o bonusmalus) o quella di un altro giocatore.
            // SE la carta giocata è già presente nell'aula bonus/malus del giocatore scelto
            // => non è possibile giocare la carta => la carta viene scartata
            if (deck_contains_card(target_player->magic_cards, played_card)) {
                printf("\n[" RED "!" RESET "] È già presente una carta identica nell'aula bonus/malus del giocatore scelto! La carta verrà scartata!\n");
                discard_card(played_card, discard_deck); // Scarta la carta giocata
            } else {
                // SE l'utente target non sono io
                // e SE l'utente target può bloccare l'effetto
                // chiede all'utente target SE vuole bloccare l'effetto
                if (player != target_player && can_block_effect(target_player, played_card) && ask_block_effect(target_player, played_card)) {
                    block_effect(target_player, played_card, discard_deck);
                    discard_card(played_card, discard_deck); // Scarta la carta giocata
                } else {
                    // Aggiunge la carta all'aula bonus/malus del giocatore scelto
                    target_player->magic_cards = add_card(target_player->magic_cards, played_card);
                    check_effect(player, played_card, SUBITO, draw_deck, discard_deck); // Controlla e attiva eventuali effetti della carta giocata
                }
            }
            break;

        case MAGIA:
            // Applica l'effetto della carta magia
            check_effect(player, played_card, SUBITO, draw_deck, discard_deck); // Controlla e attiva eventuali effetti della carta giocata
            // Una volta applicato l'effetto, la carta viene scartata
            discard_card(played_card, discard_deck); // Scarta la carta giocata
            break;

        case ISTANTANEA:
            // Applica l'effetto della carta istantanea
            // Una volta applicato l'effetto (effetto sprecato), la carta viene scartata
            discard_card(played_card, discard_deck); // Scarta la carta giocata
            break;
    }

    return;
}