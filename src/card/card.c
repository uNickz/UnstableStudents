#include "card.h"
#include "../utils/utils.h"
#include "../effect/effect.h"
#include "../logging/logging.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Carica un mazzo di carte da un file.
 * 
 * @param filename Nome del file da cui caricare le carte.
 * @param num_cards Puntatore al numero di carte totali.
 * @param num_unique_cards Puntatore al numero di carte univoche.
 * @return Card* Puntatore alla testa della lista di carte.
 */
Card* load_cards(const char* filename, int* num_cards, int* num_unique_cards) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    puts("\n[" HBLU "i" RESET "] Caricamento delle carte dal mazzo in corso...");

    FILE* file = safe_fopen(filename, "r"); // Apertura del file in lettura
    *num_cards = 0; // Inizializzazione del numero di carte totali
    *num_unique_cards = 0; // Inizializzazione del numero di carte univoche
    Card* deck = NULL; // Inizializzazione del mazzo

    int quantity, check; // Variabili di controllo + quantità di carte
    do {
        // Legge la quantità di carte e verifica se la lettura è andata a buon fine
        check = fscanf(file, " %d", &quantity);
        if (check == 1) {
            Card* card = read_card(file); // Legge la carta dal file
            *num_cards += quantity; // Incrementa il numero di carte totali
            (*num_unique_cards)++; // Incrementa il numero di carte univoche

            // Aggiunge la carta al mazzo per la quantità specificata
            for (int i = 0; i < quantity; i++) {
                Card* copy_of_card = (Card*) safe_calloc(1, sizeof(Card)); // Alloca la memoria per la copia della carta
                copy_card(card, copy_of_card); // Copia la carta
                deck = add_card(deck, copy_of_card); // Aggiunge la carta al mazzo
            }

            free_card(card); // Libera la memoria allocata per la carta iniziale
        }
    } while (check == 1); // Continua finché ci sono carte da leggere

    fclose(file); // Chiusura del file
    return deck;
}

/**
 * @brief Legge una carta da un file.
 * 
 * @param file Puntatore al file da cui leggere la carta.
 * @return Card* Puntatore alla carta letta.
 */
Card* read_card(FILE* file) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Alloca la memoria per la carta
    Card* card = (Card*) safe_calloc(1, sizeof(Card));

    // Legge il nome della carta e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %" STR_MAX_NAME_LENGTH "[^\n]s", card->name) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura del nome della carta!\n");
        exit(EXIT_FAILURE);
    }

    // Sanitizza e rimuove gli spazi dal nome
    sanitize_string(card->name);
    strip_string(card->name);

    // Legge la descrizione della carta e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %" STR_MAX_DESCRIPTION_LENGTH "[^\n]s", card->description) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura della descrizione della carta \"%s\"!\n", card->name);
        exit(EXIT_FAILURE);
    }

    // Sanitizza e rimuove gli spazi dalla descrizione
    sanitize_string(card->description);
    strip_string(card->name);

    // Legge il tipo della carta e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %d", &card->type) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura del tipo della carta \"%s\"!\n", card->name);
        exit(EXIT_FAILURE);
    }

    // Legge il numero di effetti della carta e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %d", &card->num_effects) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura del numero di effetti della carta \"%s\"!\n", card->name);
        exit(EXIT_FAILURE);
    }

    // Se la carta ha effetti, li legge e verifica se la lettura è andata a buon fine
    if (card->num_effects > 0) {
        // Alloca la memoria per gli effetti della carta
        card->effects = (Effect*) safe_calloc(card->num_effects, sizeof(Effect));

        for (int i = 0; i < card->num_effects; i++) {
            if (fscanf(file, " %d %d %d", &card->effects[i].action_effect, &card->effects[i].target_player, &card->effects[i].target_card) != INPUT_EFFECT_UNIT) {
                printf("\n[" RED "!" RESET "] Errore nella lettura  del %d° effetto della carta \"%s\"!\n", i+1, card->name);
                exit(EXIT_FAILURE);
            }
        }
    } else {
        card->effects = NULL; // Per evitare problemi imposto a NULL
    }

    // Legge il momento di attivazione della carta e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %d", &card->when_activate) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura del momento di attivazione della carta \"%s\"!\n", card->name);
        exit(EXIT_FAILURE);
    }

    // Legge se l'effetto della carta è opzionale e verifica se la lettura è andata a buon fine
    if (fscanf(file, " %d", &card->optional) != INPUT_UNIT) {
        printf("\n[" RED "!" RESET "] Errore nella lettura della flag opzionale della carta \"%s\"!\n", card->name);
        exit(EXIT_FAILURE);
    }

    card->next_card = NULL;
    return card;
}

/**
 * @brief Libera la memoria allocata per una carta.
 * 
 * @param card Puntatore alla carta da liberare.
 */
void free_card(Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    if (card == NULL) {
        return;
    }

    if (card->effects != NULL) {
        free(card->effects);
        card->effects = NULL;
    }

    free(card);
    return;
}

/**
 * @brief Libera la memoria allocata per un mazzo di carte.
 * 
 * @param deck Puntatore al mazzo di carte da liberare.
 */
void free_deck(Card* deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Card* current_card = deck;
    Card* next_card = NULL;

    while (current_card != NULL) {
        next_card = current_card->next_card;
        free_card(current_card);
        current_card = next_card;
    }

    return;
}

/**
 * @brief Copia i dati di una carta in un'altra.
 * 
 * @param src Puntatore alla carta sorgente.
 * @param dst Puntatore alla carta destinazione.
 */
void copy_card(Card* src, Card* dst) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    assert (src != NULL && "Source card is NULL!");

    strncpy(dst->name, src->name, MAX_NAME_LENGTH); // Copia il nome della carta
    strncpy(dst->description, src->description, MAX_DESCRIPTION_LENGTH); // Copia la descrizione della carta
    dst->type = src->type; // Copia il tipo della carta
    dst->num_effects = src->num_effects; // Copia il numero di effetti della carta

    // Se la carta ha effetti, li copia
    if (src->num_effects > 0) {
        dst->effects = (Effect*) safe_calloc(dst->num_effects, sizeof(Effect)); // Alloca la memoria per gli effetti

        // Copia tutta la lista degli effetti
        for (int i = 0; i < dst->num_effects; i++) {
            dst->effects[i].action_effect = src->effects[i].action_effect;
            dst->effects[i].target_player = src->effects[i].target_player;
            dst->effects[i].target_card = src->effects[i].target_card;
        }
    } else {
        dst->effects = NULL; // Per evitare problemi imposto a NULL se non ci sono effetti
    }

    dst->when_activate = src->when_activate; // Copia il momento di attivazione della carta
    dst->optional = src->optional;           // Copia la flag opzionale della carta
    dst->next_card = NULL;                   // Imposta il puntatore alla prossima carta a NULL

    return;
}

/**
 * @brief Aggiunge una carta in coda ad un mazzo.
 * 
 * @param head Puntatore alla testa del mazzo.
 * @param card Puntatore alla carta da aggiungere.
 * @return Card* Puntatore alla testa del mazzo.
 */
Card* add_card(Card* head, Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    assert (card != NULL && "Card is NULL!");

    // Se la carta è già collegata ad un'altra carta, la scollega
    if (card->next_card != NULL) {
        card->next_card = NULL;
    }

    // Se il mazzo è vuoto, la carta diventa la testa
    if (head == NULL) {
        head = card;
    } else {
        // Altrimenti scorre il mazzo fino all'ultima carta
        Card* current = head;
        while (current->next_card != NULL) {
            current = current->next_card;
        }

        current->next_card = card; // Collega la carta alla fine del mazzo
    }

    return head;
}

/**
 * @brief Restituisce il tipo di carta in formato stringa in base al valore dell'enumerazione.
 * 
 * @param type Valore dell'enumerazione del tipo di carta.
 * @return char* Stringa del tipo di carta.
 */
char* get_type_card(Type_card type) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    char* type_card; // Inizializa la stringa del tipo di carta

    // Assegna il tipo di carta in base al valore dell'enumerazione
    switch (type) {
        case ALL:
            type_card = "ALL";
            break;
        case STUDENTE:
            type_card = "STUDENTE";
            break;
        case MATRICOLA:
            type_card = "MATRICOLA";
            break;
        case STUDENTE_SEMPLICE:
            type_card = "STUDENTE SEMPLICE";
            break;
        case LAUREANDO:
            type_card = "LAUREANDO";
            break;
        case BONUS:
            type_card = "BONUS";
            break;
        case MALUS:
            type_card = "MALUS";
            break;
        case MAGIA:
            type_card = "MAGIA";
            break;
        case ISTANTANEA:
            type_card = "ISTANTANEA";
            break;
        default:
            type_card = "UNKNOWN";
            break;
    }

    return type_card;
}

/**
 * @brief Restituisce il tipo di carta in formato stringa formattato.
 * 
 * @param type Valore dell'enumerazione del tipo di carta.
 * @return char* Stringa del tipo di carta formattata.
 */
char* get_parsed_type_card(Type_card type) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Stringhe di inizio e fine per il tipo di carta
    const static char start_str[] = "[";
    const static char end_str[] = "]";

    char* type_card = get_type_card(type); // Ottiene il tipo di carta in formato stringa
    int parsed_type_len = strlen(type_card) + strlen(start_str) + strlen(end_str); // Calcola la lunghezza della stringa formattata

    // Alloca la memoria per la stringa formattata
    char* parsed_type_card = (char*) safe_malloc((parsed_type_len + 1) * sizeof(char));
    parsed_type_card[0] = '\0'; // Inizializa la stringa

    // Costruisce la stringa formattata
    strcat(parsed_type_card, start_str);
    strcat(parsed_type_card, type_card);
    strcat(parsed_type_card, end_str);

    return parsed_type_card;
}

/**
 * @brief Restituisce il colore per la formattazione in base al tipo di carta.
 * 
 * @param type Valore dell'enumerazione del tipo di carta.
 * @return char* Stringa del colore per la formattazione.
 */
char* get_color_by_type(Type_card type) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    char* color; // Inizializza il colore per la formattazione

    // Assegna il colore in base al tipo di carta
    switch (type) {
        case STUDENTE:
        case MATRICOLA:
        case STUDENTE_SEMPLICE:
        case LAUREANDO:
            color = BHYEL;
            break;
        case BONUS:
            color = BHBLU;
            break;
        case MALUS:
            color = BHRED;
            break;
        case MAGIA:
            color = BHMAG;
            break;
        case ISTANTANEA:
            color = BHGRN;
            break;
        default:
            color = WHT;
            break;
    }

    return color;
}

/**
 * @brief Calcola il numero massimo di righe per la descrizione di un mazzo di carte.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @return int Il numero massimo di righe per la descrizione delle carte.
 */
int calculate_max_row_length_for_description(const Card* deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int max_row_length = 0; // Inizializza il numero massimo di righe per la descrizione a 0

    // Scorre tutte le carte del mazzo
    Card* current_card = deck;
    while (current_card != NULL) {
        // Calcola il numero di righe per la descrizione della carta
        int card_description_rows = 0;
        char** wrapped_description = wrap_text(current_card->description, REAL_CARD_WIDTH, &card_description_rows);

        // Aggiorna il numero massimo di righe per la descrizione
        max_row_length = max(max_row_length, card_description_rows);

        // Libera la memoria allocata per le righe della descrizione
        for (int j = 0; j < card_description_rows; j++) {
            free(wrapped_description[j]);
        }

        free(wrapped_description); // Libera la memoria allocata per l'array di righe
        current_card = current_card->next_card; // Passa alla carta successiva
    }

    return max_row_length;
}

/**
 * @brief Formatta graficamente una carta in un buffer di righe.
 * 
 * @param buffer_rows Puntatore per restituire il numero di righe del buffer.
 * @param card Puntatore alla carta da formattare.
 * @param max_rows_description Numero massimo di righe per la descrizione.
 * @param card_index Indice della carta.
 * @param total_cards Numero totale di carte.
 * @return char** Buffer di righe contenente la carta formattata.
 */
char** format_card(int* buffer_rows, const Card* card, int max_rows_description, int card_index, int total_cards) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Costruzione dei separatori e calcolo del padding
    char* card_border = repeat_string(UNICODE_BORDER_HORIZONTAL, CARD_WIDTH);
    char* card_line_separator = repeat_char(' ', REAL_CARD_WIDTH);

    char* card_name = padding_string(card->name, REAL_CARD_WIDTH);
    char* parsed_type = get_parsed_type_card(card->type);
    char* card_type = padding_string(parsed_type, REAL_CARD_WIDTH);

    // Calcola il numero di righe per la descrizione della carta
    int card_description_lines = 0;
    char** wrapped_description = wrap_text(card->description, REAL_CARD_WIDTH, &card_description_lines);

    // Calcola il colore della carta in base al tipo e calcola la dimensione del colore in caratteri
    char* card_color = get_color_by_type(card->type);
    int card_color_size = strlen(card_color) + strlen(RESET);

    bool need_index = card_index != -1; // Flag per la necessità dell'indice

    if (!max_rows_description) {
        // SE non è stato specificato il numero massimo di righe per la descrizione della carta
        // utilizza il numero di righe effettivo della descrizione della carta
        max_rows_description = card_description_lines;
    }

    // Calcola il numero totale di righe del buffer
    int total_rows = CARD_BASE_HEIGHT + max_rows_description - !need_index;
    *buffer_rows = total_rows;

    // Calcola la lunghezza totale di ogni riga del buffer
    int total_row_length = strlen(UNICODE_BORDER_UP_LEFT) + strlen(card_border) + strlen(UNICODE_BORDER_UP_RIGHT) + card_color_size;

    char** buffer = (char**) safe_malloc(total_rows * sizeof(char*)); // Alloca la memoria per il buffer

    for (int i = 0; i < total_rows; i++) {
        // Alloca la memoria per ogni riga del buffer
        buffer[i] = (char*) safe_malloc((total_row_length + 1) * sizeof(char));
        buffer[i][0] = '\0'; // Inizializza la stringa
    }

    // Costruzione del buffer
    int buffer_index = 0;

    sprintf(buffer[buffer_index++], "%s%s%s%s%s", card_color, UNICODE_BORDER_UP_LEFT, card_border, UNICODE_BORDER_UP_RIGHT, RESET);
    sprintf(buffer[buffer_index++], "%s%s%s %s%s%s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, BOLD, card_name, RESET, card_color, UNICODE_BORDER_VERTICAL, RESET);
    sprintf(buffer[buffer_index++], "%s%s %s %s%s", card_color, UNICODE_BORDER_VERTICAL, card_type, UNICODE_BORDER_VERTICAL, RESET);
    sprintf(buffer[buffer_index++], "%s%s%s %s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, card_line_separator, card_color, UNICODE_BORDER_VERTICAL, RESET);

    for (int i = 0; i < card_description_lines; i++) {
        sprintf(buffer[buffer_index++], "%s%s%s %s%s%s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, ITALIC, wrapped_description[i], RESET, card_color, UNICODE_BORDER_VERTICAL, RESET);
        free(wrapped_description[i]);
    }

    for (int i = card_description_lines; i < max_rows_description; i++) {
        sprintf(buffer[buffer_index++], "%s%s%s %s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, card_line_separator, card_color, UNICODE_BORDER_VERTICAL, RESET);
    }

    sprintf(buffer[buffer_index++], "%s%s%s %s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, card_line_separator, card_color, UNICODE_BORDER_VERTICAL, RESET);

    if (need_index) {
        char* parsed_index = (char*) safe_malloc((MAX_CARD_INDEX_LENGTH + 1) * sizeof(char));
        sprintf(parsed_index, "%d/%d", card_index, total_cards);
        char* padded_card_index = padding_string(parsed_index, REAL_CARD_WIDTH);

        sprintf(buffer[buffer_index++], "%s%s%s %s %s%s%s", card_color, UNICODE_BORDER_VERTICAL, RESET, padded_card_index, card_color, UNICODE_BORDER_VERTICAL, RESET);

        free(parsed_index);
        free(padded_card_index);
    }

    sprintf(buffer[buffer_index++], "%s%s%s%s%s", card_color, UNICODE_BORDER_DOWN_LEFT, card_border, UNICODE_BORDER_DOWN_RIGHT, RESET);

    // Liberazione della memoria allocata dinamicamente
    free(card_border);
    free(card_line_separator);
    free(card_name);
    free(parsed_type);
    free(card_type);
    free(wrapped_description);

    return buffer;
}

/**
 * @brief Formatta graficamente una carta nascosta in un buffer di righe.
 * 
 * @param buffer_rows Puntatore per restituire il numero di righe del buffer.
 * @param card_index Indice della carta.
 * @param total_cards Numero totale di carte.
 * @return char** Buffer di righe contenente la carta nascosta formattata.
 */
char** format_hidden_card(int* buffer_rows, int card_index, int total_cards) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Costruzione dei separatori e calcolo del padding
    char* card_border = repeat_string(UNICODE_BORDER_HORIZONTAL, CARD_WIDTH);
    char* card_line_separator = repeat_char(' ', REAL_CARD_WIDTH);

    char* card_name = padding_string("Unstable Students", REAL_CARD_WIDTH);

    // Calcola il colore della carta in base al tipo e calcola la dimensione del colore in caratteri
    char* card_color = WHT;
    int card_color_size = strlen(card_color) + strlen(RESET);

    bool need_index = card_index != -1; // Flag per la necessità dell'indice

    // Calcola il numero totale di righe del buffer
    int total_rows = CARD_BASE_HEIGHT + CARD_HIDDEN_HEIGHT - !need_index;
    *buffer_rows = total_rows;

    // Calcola la lunghezza totale di ogni riga del buffer
    int total_row_length = strlen(UNICODE_BORDER_UP_LEFT) + strlen(card_border) + strlen(UNICODE_BORDER_UP_RIGHT) + card_color_size;

    // Alloca la memoria per il buffer
    char** buffer = (char**) safe_malloc(total_rows * sizeof(char*));

    for (int i = 0; i < total_rows; i++) {
        // Alloca la memoria per ogni riga del buffer
        buffer[i] = (char*) safe_malloc((total_row_length + 1) * sizeof(char));
        buffer[i][0] = '\0'; // Inizializza la stringa
    }

    // Costruzione del buffer
    int buffer_index = 0;

    sprintf(buffer[buffer_index++], "%s%s%s", UNICODE_BORDER_UP_LEFT, card_border, UNICODE_BORDER_UP_RIGHT);
    sprintf(buffer[buffer_index++], "%s %s%s%s %s", UNICODE_BORDER_VERTICAL, BOLD, card_name, RESET, UNICODE_BORDER_VERTICAL);
    sprintf(buffer[buffer_index++], "%s %s %s", UNICODE_BORDER_VERTICAL, card_line_separator, UNICODE_BORDER_VERTICAL);

    sprintf(buffer[buffer_index++], "%s   %s   %s", UNICODE_BORDER_VERTICAL, HIDDEN_CARD_FIRST_PATTERN, UNICODE_BORDER_VERTICAL);
    sprintf(buffer[buffer_index++], "%s   %s   %s", UNICODE_BORDER_VERTICAL, HIDDEN_CARD_SECOND_PATTERN, UNICODE_BORDER_VERTICAL);
    sprintf(buffer[buffer_index++], "%s   %s   %s", UNICODE_BORDER_VERTICAL, HIDDEN_CARD_FIRST_PATTERN, UNICODE_BORDER_VERTICAL);
    sprintf(buffer[buffer_index++], "%s   %s   %s", UNICODE_BORDER_VERTICAL, HIDDEN_CARD_SECOND_PATTERN, UNICODE_BORDER_VERTICAL);

    sprintf(buffer[buffer_index++], "%s %s %s", UNICODE_BORDER_VERTICAL, card_line_separator, UNICODE_BORDER_VERTICAL);

    if (need_index) {
        char* parsed_index = (char*) safe_malloc((MAX_CARD_INDEX_LENGTH + 1) * sizeof(char));
        sprintf(parsed_index, "%d/%d", card_index, total_cards);
        char* card_index = padding_string(parsed_index, REAL_CARD_WIDTH);

        sprintf(buffer[buffer_index++], "%s %s %s", UNICODE_BORDER_VERTICAL, card_index, UNICODE_BORDER_VERTICAL);

        free(parsed_index);
        free(card_index);
    }

    sprintf(buffer[buffer_index++], "%s%s%s", UNICODE_BORDER_DOWN_LEFT, card_border, UNICODE_BORDER_DOWN_RIGHT);

    // Liberazione della memoria allocata dinamicamente
    free(card_border);
    free(card_line_separator);
    free(card_name);

    return buffer;
}

/**
 * @brief Stampa una carta.
 * 
 * @param card Puntatore alla carta da stampare.
 */
void print_card(const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Formatta la carta in un buffer di righe
    int buffer_rows = 0;
    char** buffer = format_card(&buffer_rows, card, 0, -1, -1);

    // Stampa la carta
    for (int i = 0; i < buffer_rows; i++) {
        printf("%s\n", buffer[i]);
        free(buffer[i]); // Liberazione della memoria allocata dinamicamente per la riga
        buffer[i] = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione
    }

    free(buffer); // Liberazione della memoria allocata dinamicamente per il buffer
    return;
}

/**
 * @brief Formatta graficamente un mazzo di carte in un buffer di righe.
 * 
 * @param buffer_rows Puntatore per restituire il numero di righe del buffer.
 * @param deck Puntatore al mazzo di carte da formattare.
 * @param deck_name Nome del mazzo di carte.
 * @param offset_card_index Indice di offset per la numerazione delle carte.
 * @param hidden Flag per nascondere le informazioni delle carte.
 * @param color Colore del mazzo di carte.
 * @return char** Buffer di righe contenente il mazzo di carte formattato.
 */
char** format_deck(int* buffer_rows, const Card* deck, const char* deck_name, int offset_card_index, bool hidden, char* color) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // SE non è stato specificato un colore, utilizza il colore bianco
    if (color == NULL) {
        color = WHT;
    }

    char** buffer = NULL; // Inizializzazione del buffer
    int extra_space_for_color = (strlen(color) + strlen(RESET)) * (DECK_PADDING + COLOR_PADDING); // Calcolo dello spazio extra per il colore

    // SE il deck è vuoto, formatta un deck vuoto
    if (deck == NULL) {
        // Alloca la memoria per il buffer
        buffer = (char**) safe_malloc(EMPTY_DECK_HEIGHT * sizeof(char*));
        *buffer_rows = EMPTY_DECK_HEIGHT; // Imposta il numero di righe del buffer

        // Alloca la memoria per ogni riga del buffer
        for (int i = 0; i < EMPTY_DECK_HEIGHT; i++) {
            buffer[i] = (char*) safe_malloc(((strlen(UNICODE_BORDER_HORIZONTAL) * EMPTY_DECK_WIDTH) + extra_space_for_color + 1) * sizeof(char));
            buffer[i][0] = '\0'; // Inizializza la stringa
        }

        // Costruzione dei separatori e calcolo del padding
        char* empty_deck_border = repeat_string(UNICODE_BORDER_HORIZONTAL, EMPTY_DECK_WIDTH-DECK_PADDING);
        char* empy_line = repeat_char(' ', EMPTY_DECK_WIDTH-DECK_PADDING);

        char* parsed_deck_name = padding_string(deck_name, DECK_NAME_WIDTH);

        int available_space = (EMPTY_DECK_WIDTH-DECK_PADDING-DECK_PADDING);
        int deck_name_padding = calculate_padding(available_space, DECK_NAME_WIDTH);

        char* deck_name_left_border = repeat_string(UNICODE_BORDER_HORIZONTAL, deck_name_padding);
        char* deck_name_right_border = repeat_string(UNICODE_BORDER_HORIZONTAL, available_space-DECK_NAME_WIDTH-deck_name_padding);

        char* empy_text = padding_string("///", EMPTY_DECK_WIDTH-DECK_PADDING);

        // Costruzione del buffer
        int buffer_index = 0;

        sprintf(buffer[buffer_index++], "%s%s%s[%s%s%s%s]%s%s%s", color, UNICODE_BORDER_UP_LEFT, deck_name_left_border, BOLD, parsed_deck_name, RESET, color, deck_name_right_border, UNICODE_BORDER_UP_RIGHT, RESET);
        sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, empy_line, color, UNICODE_BORDER_VERTICAL, RESET);
        sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, BOLD, empy_text, RESET, color, UNICODE_BORDER_VERTICAL, RESET);
        sprintf(buffer[buffer_index++], "%s%s%s%s%s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, empy_line, color, UNICODE_BORDER_VERTICAL, RESET);
        sprintf(buffer[buffer_index++], "%s%s%s%s%s", color, UNICODE_BORDER_DOWN_LEFT, empty_deck_border, UNICODE_BORDER_DOWN_RIGHT, RESET);

        // Liberazione della memoria allocata dinamicamente
        free(empty_deck_border);
        free(empy_line);
        free(parsed_deck_name);
        free(deck_name_left_border);
        free(deck_name_right_border);
        free(empy_text);
    } else {
        // SE il deck non è vuoto, formatta il deck con le carte al suo interno
        int num_cards = count_cards(deck); // Calcola il numero di carte nel mazzo
        // Calcola il numero massimo di righe da utilizzare per la descrizione di ogni carta
        int max_rows_description = hidden == true ? CARD_HIDDEN_HEIGHT : calculate_max_row_length_for_description(deck);
        int card_rows = CARD_BASE_HEIGHT + max_rows_description; // Calcola il numero di righe totali per ogni carta
        int line_cards = (num_cards / MAX_CARD_PER_ROW) + (num_cards % MAX_CARD_PER_ROW != 0); // Calcola il numero di righe di carte
        int deck_rows = DECK_BASE_HEIGHT + (card_rows * line_cards); // Calcola il numero di righe complessive da utilizzare per la formattazione del mazzo

        // Alloca la memoria per il buffer
        buffer = (char**) safe_calloc(deck_rows, sizeof(char*));
        *buffer_rows = deck_rows; // Imposta il numero di righe del buffer

        Card* current_card = deck; // Inizializza la carta corrente al primo elemento del mazzo
        int card_index = 1; // Inizializza l'indice delle carte da 1

        // Scorre tutte le carte del mazzo
        while (current_card != NULL) {
            // Per ogni carta, formatta la carta in un buffer di righe
            char** card_buffer = NULL;
            int card_buffer_rows = 0;

            if (hidden == true) {
                // SE la carta è nascosta, formatta la carta nascosta con il template apposito
                card_buffer = format_hidden_card(&card_buffer_rows, offset_card_index+card_index, offset_card_index+num_cards);
            } else {
                // Altrimenti, formatta la carta con il suoi dati effettivi
                card_buffer = format_card(&card_buffer_rows, current_card, max_rows_description, offset_card_index+card_index, offset_card_index+num_cards);
            }

            // Calcola l'offset per la posizione della carta nel buffer (in base alla riga)
            int offset = (card_index-1) / MAX_CARD_PER_ROW;
            offset *= card_rows;

            // Aggiunge la carta formattata al buffer del mazzo
            for (int i = 0, j = offset+i+1; i < card_buffer_rows; i++, j++) {
                // Inizializa la riga del buffer se non è stata inizializzata
                if (buffer[j] == NULL) {
                    buffer[j] = (char*) safe_malloc(sizeof(char));
                    buffer[j][0] = '\0';
                }

                // Rialloca la memoria per farci stare la riga del buffer della carta formattata
                buffer[j] = (char*) safe_realloc(buffer[j], (strlen(buffer[j]) + strlen(card_buffer[i]) + 1) * sizeof(char));

                strcat(buffer[j], card_buffer[i]); // Concatena la riga del buffer della carta formattata al buffer del mazzo
                free(card_buffer[i]); // Libera la memoria allocata per la riga del buffer della carta formattata
                card_buffer[i] = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione
            }

            free(card_buffer); // Libera la memoria allocata per il buffer di righe della carta
            current_card = current_card->next_card; // Passa alla carta successiva
            card_index++; // Incrementa l'indice delle carte
        }

        // Costruzione dei separatori e calcolo del padding
        int real_cards_width = min(MAX_CARD_PER_ROW, num_cards) * MAX_CARD_WIDTH;
        int edges_width = strlen(UNICODE_BORDER_UP_LEFT) + strlen(UNICODE_BORDER_UP_RIGHT);
        int deck_width = (real_cards_width * strlen(UNICODE_BORDER_HORIZONTAL)) + (edges_width * DECK_PADDING);
        int colored_deck_width = strlen(buffer[1]) + (edges_width * DECK_PADDING);

        int last_row_cards = num_cards % MAX_CARD_PER_ROW;
        last_row_cards = last_row_cards == 0 ? MAX_CARD_PER_ROW : last_row_cards; // Se l'ultima riga è piena (resto == 0), allora last_row_cards = MAX_CARD_PER_ROW
        int real_last_row_cards_width = last_row_cards * MAX_CARD_WIDTH;

        int max_space_available = max(DECK_PADDING+real_cards_width+DECK_PADDING, EMPTY_DECK_WIDTH);
        // SE l'ultima riga è più corta della larghezza massima disponibile, allora allinea le carte al centro
        if (real_last_row_cards_width < max_space_available) {
            deck_width = max_space_available * strlen(UNICODE_BORDER_HORIZONTAL);

            int offset = (line_cards-1) * card_rows + 1; // Calcola l'offset per la posizione delle carte nell'ultima riga

            // Per ogni carta dell'ultima riga
            for (int i = offset; i < offset+card_rows; i++) {
                // Calcola lo spazio disponibile per allineare le carte al centro
                int space_available = max_space_available-DECK_PADDING-DECK_PADDING;
                int line_padding = calculate_padding(space_available, real_last_row_cards_width);

                char* left_padding = repeat_char(' ', line_padding);
                char* right_padding = repeat_char(' ', space_available-real_last_row_cards_width-line_padding);

                // Allocare la memoria per il buffer temporaneo
                char* tmp = (char*) safe_malloc((strlen(buffer[i]) + strlen(left_padding) + strlen(right_padding) + 1) * sizeof(char));
                tmp[0] = '\0';
                sprintf(tmp, "%s%s%s", left_padding, buffer[i], right_padding); // Formatta il buffer temporaneo con il padding per allineare le carte al centro

                free(buffer[i]); // Libera la memoria allocata per la riga del buffer
                buffer[i] = tmp;

                free(left_padding); // Libera la memoria allocata per il padding sinistro
                free(right_padding); // Libera la memoria allocata per il padding destro
            }
        }

        // Per ogni riga del buffer del mazzo
        for (int i = 0; i < deck_rows; i++) {
            // SE la riga del buffer non è stata inizializzata
            if (buffer[i] == NULL) {
                // Alloca la memoria per la riga del buffer
                buffer[i] = (char*) safe_calloc(colored_deck_width + extra_space_for_color + 1, sizeof(char));
                buffer[i][0] = '\0'; // Inizializza la stringa
            } else {
                // Rialloca la memoria per la riga del buffer
                buffer[i] = (char*) safe_realloc(buffer[i], (colored_deck_width + extra_space_for_color + 1) * sizeof(char));
            }
        }

        // Costruzione dei separatori e calcolo del padding
        int card_area_width = max(real_cards_width, EMPTY_DECK_WIDTH-DECK_PADDING-DECK_PADDING);
        int deck_name_padding = calculate_padding(card_area_width, DECK_NAME_WIDTH);

        char* deck_border = repeat_string(UNICODE_BORDER_HORIZONTAL, (deck_width-edges_width)/strlen(UNICODE_BORDER_HORIZONTAL));
        char* parsed_deck_name = padding_string(deck_name, DECK_NAME_WIDTH);

        char* deck_name_left_border = repeat_string(UNICODE_BORDER_HORIZONTAL, deck_name_padding);
        char* deck_name_right_border = repeat_string(UNICODE_BORDER_HORIZONTAL, card_area_width-DECK_NAME_WIDTH-deck_name_padding);

        // Costruzione del buffer
        int buffer_index = 0;

        sprintf(buffer[buffer_index++], "%s%s%s[%s%s%s%s]%s%s%s", color, UNICODE_BORDER_UP_LEFT, deck_name_left_border, BOLD, parsed_deck_name, RESET, color, deck_name_right_border, UNICODE_BORDER_UP_RIGHT, RESET);

        for (int i = 1; i < deck_rows-1; i++) {
            // Alloco un buffer temporaneo
            char* tmp = (char*) safe_calloc((colored_deck_width + extra_space_for_color + 1), sizeof(char));
            tmp[0] = '\0'; // Inizializzo il buffer temporaneo con un terminatore di stringa

            sprintf(tmp, "%s%s%s %s %s%s%s", color, UNICODE_BORDER_VERTICAL, RESET, buffer[buffer_index], color, UNICODE_BORDER_VERTICAL, RESET);

            free(buffer[buffer_index]); // Libero la memoria precedentemente allocata
            buffer[buffer_index++] = tmp; // Assegno il nuovo buffer temporaneo al buffer principale e incremento l'indice del buffer principale
        }

        sprintf(buffer[buffer_index++], "%s%s%s%s%s", color, UNICODE_BORDER_DOWN_LEFT, deck_border, UNICODE_BORDER_DOWN_RIGHT, RESET);

        // Liberazione della memoria allocata dinamicamente
        free(deck_border);
        free(parsed_deck_name);
        free(deck_name_left_border);
        free(deck_name_right_border);
    }

    return buffer;
}

/**
 * @brief Stampa un mazzo di carte.
 * 
 * @param deck Puntatore al mazzo di carte da stampare.
 * @param deck_name Nome del mazzo di carte.
 * @param offset_card_index Indice di offset per la numerazione delle carte.
 * @param hidden Flag per nascondere le informazioni delle carte.
 * @param color Colore del mazzo di carte.
 */
void print_deck(const Card* deck, const char* deck_name, int offset_card_index, bool hidden, char* color) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Formatta il mazzo di carte in un buffer di righe
    int buffer_rows = 0;
    char** buffer = format_deck(&buffer_rows, deck, deck_name, offset_card_index, hidden, color);

    for (int i = 0; i < buffer_rows; i++) {
        printf("%s\n", buffer[i]); // Stampa ogni riga del buffer
        free(buffer[i]); // Libera la memoria allocata per la riga del buffer
        buffer[i] = NULL; // Imposta il puntatore a NULL per evitare problemi dopo la deallocazione
    }

    free(buffer); // Libera la memoria allocata per il buffer
    return;
}

/**
 * @brief Conta il numero di carte presenti in un mazzo.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @return int Numero di carte presenti nel mazzo.
 */
int count_cards(Card* deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int count = 0; // Inizializza il conteggio delle carte a 0
    Card* current_card = deck; // Inizializza la carta corrente al primo elemento del maazzo

    // Scorre tutte le carte del mazzo
    while (current_card != NULL) {
        count++; // Incrementa il conteggio delle carte
        current_card = current_card->next_card; // Passa alla carta successiva
    }

    return count;
}

/**
 * @brief Mescola un mazzo di carte.
 * 
 * @param deck Puntatore al mazzo di carte da mescolare.
 * @param num_cards Numero di carte presenti nel mazzo.
 * @return Card* Puntatore alla testa del nuovo mazzo di carte mescolato.
 */
Card* shuffle_deck(Card* deck, int num_cards) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    assert (deck != NULL && "Deck is NULL!");
    assert (num_cards > 0 && "Number of cards is invalid!");

    // SE il mazzo contiene una sola carta, restituisce il mazzo invariato
    if (num_cards == 1) {
        return deck;
    }

    // Alloca la memoria per un array di puntatori alle carte
    Card** arr_card = (Card**) safe_malloc(num_cards * sizeof(Card*));

    // Riempie l'array di puntatori alle carte con le carte del mazzo
    int i = 0; // Inizializza l'indice dell'array di puntatori
    Card* current_card = deck; // Inizializza la carta corrente al primo elemento del mazzo
    while (current_card != NULL) {
        arr_card[i++] = current_card; // Aggiunge la carta corrente all'array di puntatori
        current_card = current_card->next_card; // Passa alla carta successiva
    }

    // Mescola l'array di puntatori alle carte
    int idx, idj; // Inizializza gli indici per lo scambio delle carte
    for (int i = 0; i < SHUFFLE_ROUNDS; i++) {
        idx = rand() % num_cards; // Genera un indice casuale per la carta da scambiare
        idj = rand() % num_cards; // Genera un indice casuale per la carta con cui scambiare

        // Scambia le carte
        Card* temp = arr_card[idx];
        arr_card[idx] = arr_card[idj];
        arr_card[idj] = temp;
    }

    // Collega le carte dell'array di puntatori
    for (int i = 0; i < num_cards - 1; i++) {
        arr_card[i]->next_card = arr_card[i + 1];
    }
    arr_card[num_cards - 1]->next_card = NULL; // Imposta il puntatore alla prossima carta dell'ultima carta a NULL

    // La nuova testa del mazzo è la prima carta dell'array di puntatori
    Card* new_deck = arr_card[0];

    // Libera la memoria allocata per l'array di puntatori
    free(arr_card);

    return new_deck;
}

/**
 * @brief Separa le carte MATRICOLA da un mazzo di carte (il mazzo di pesca) e le aggiunge ad un altro mazzo di carte (l'aula studio).
 * 
 * @param deck Puntatore al mazzo di carte da cui separare le carte MATRICOLA.
 * @param study_room Puntatore al mazzo di carte in cui aggiungere le carte MATRICOLA.
 */
void separate_matricola_cards(Card** deck, Card** study_room) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Card* current_card = *deck; // Inizializza la carta corrente al primo elemento del mazzo
    Card* prev_card = NULL; // Inizializza la carta precedente a NULL
    Card* next_card = NULL; // Inizializza la prossima carta a NULL

    // Scorre tutte le carte del mazzo
    while (current_card != NULL) {
        next_card = current_card->next_card; // Salva la prossima carta

        // SE la carta corrente è una carta MATRICOLA
        if (current_card->type == MATRICOLA) {
            // SE la carta precedente è NULL, allora la carta corrente è la testa del mazzo
            if (prev_card == NULL) {
                *deck = next_card; // Imposta la testa del mazzo alla prossima carta
            } else {
                prev_card->next_card = next_card; // Collega la carta precedente alla prossima carta
            }

            current_card->next_card = NULL; // Imposta il puntatore alla prossima carta della carta corrente a NULL
            *study_room = add_card(*study_room, current_card); // Aggiunge la carta Matricola al mazzo dell'aula studio

        } else {
            prev_card = current_card; // Imposta la carta precedente alla carta corrente
        }

        current_card = next_card; // Passa alla prossima carta
    }

    return;
}

/**
 * @brief Pesca una carta dal mazzo di pesca e la aggiunge alla mano del giocatore.
 * 
 * @param player Puntatore al giocatore.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 * @param show_card Flag per mostrare la carta pescata.
 */
void draw_card(Player** player, Card** draw_deck, Card** discard_deck, bool show_card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_cards = count_cards(*draw_deck); // Conteggio delle carte nel mazzo di pesca

    // SE il mazzo di pesca è vuoto
    if (num_cards == 0) {
        assert (*discard_deck != NULL && "Il mazzo di pesca e il mazzo degli scarti sono vuoti!");

        num_cards = count_cards(*discard_deck); // Conteggio delle carte nel mazzo degli scarti
        *draw_deck = shuffle_deck(*discard_deck, num_cards); // Mescola il mazzo degli scarti e lo trasforma nel mazzo di pesca
        *discard_deck = NULL; // Imposta il mazzo degli scarti a NULL
    }

    Card* current_card = *draw_deck; // Inizializza la carta corrente al primo elemento del mazzo
    *draw_deck = current_card->next_card; // Sposta la testa del mazzo alla prossima carta
    current_card->next_card = NULL; // Scollega la carta corrente dal mazzo di pesca
    (*player)->hand = add_card((*player)->hand, current_card); // Aggiunge la carta pescata alla mano del giocatore

    log_draw_card(*player, current_card); // Registra l'azione di pesca della carta nel file di log

    // SE è richiesto di mostrare la carta pescata
    if (show_card == true) {
        printf("\n[" GRN "+" RESET "] Hai pescato una carta!\n");
        print_card(current_card); // Stampa la carta pescata
    }

    return;
}

/**
 * @brief Pesca una carta MATRICOLA dal mazzo dell'aula studio e la aggiunge alla mano del giocatore.
 * 
 * @param player_classroom Puntatore all'aula del giocatore.
 * @param study_room Puntatore al mazzo dell'aula studio.
 * @return Card* Puntatore all'aula del giocatore aggiornata.
 */
Card* draw_matricola_card(Card* player_classroom, Card** study_room) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_study_room = count_cards(*study_room); // Conteggio delle carte nell'aula studio
    assert (num_study_room > 0 && "Carte Matricola terminate!");

    Card* current_card = *study_room; // Inizializza la carta corrente al primo elemento dell'aula studio
    *study_room = current_card->next_card; // Sposta la testa dell'aula studio alla prossima carta
    current_card->next_card = NULL; // Scollega la carta corrente dall'aula studio
    player_classroom = add_card(player_classroom, current_card); // Aggiunge la carta MATRICOLA all'aula del giocatore

    return player_classroom;
}

/**
 * @brief Fa scegliere al giocatore una carta da un mazzo di carte.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @param msg Messaggio per la scelta della carta.
 * @param error_msg Messaggio di errore per la scelta non valida della carta.
 * @return int Indice della carta selezionata.
 */
int choice_card(Card* deck, char* msg, char* error_msg) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_cards = count_cards(deck); // Conteggio delle carte nel mazzo
    int card_index; // Inizializzazione dell'indice della carta selezionata

    do {
        printf(msg); // Stampa il messaggio per la scelta della carta
        read_int(&card_index); // Legge l'indice della carta selezionata

        // SE l'indice della carta selezionata non è valido
        if (card_index < 1 || card_index > num_cards) {
            printf(error_msg); // Stampa il messaggio di errore
        }
    } while (card_index < 1 || card_index > num_cards);

    // Ritorna l'indice della carta selezionata (partendo da 0)
    return card_index-1;
}

/**
 * @brief Seleziona una carta da un mazzo di carte tramite l'indice.
 * 
 * @param card_index Indice della carta da selezionare.
 * @param deck Puntatore al mazzo di carte.
 * @param unlink_card Flag per scollegare la carta dal mazzo.
 * @return Card* Puntatore alla carta selezionata.
 */
Card* select_card(int card_index, Card** deck, bool unlink_card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    assert (*deck != NULL && "Mazzo vuoto!");
    assert ((card_index >= 0 && card_index < count_cards(*deck)) && "Indice della carta non valido!");

    Card* current_card = *deck; // Inizializza la carta corrente al primo elemento del mazzo
    Card* prev_card = NULL; // Inizializza la carta precedente a NULL

    // Scorre la lista fino all'indice della carta da scartare
    for (int i = 0; i < card_index; i++) {
        prev_card = current_card; // Imposta la carta precedente alla carta corrente
        current_card = current_card->next_card; // Passa alla prossima carta
    }

    // SE la carta deve essere scollata dal mazzo
    if (unlink_card == true) {
        // SE la carta precedente è NULL, allora la carta corrente è la testa del mazzo
        if (prev_card == NULL) {
            *deck = current_card->next_card; // Sposta la testa del mazzo alla prossima carta
        } else {
            prev_card->next_card = current_card->next_card; // Collega la carta precedente alla prossima carta
        }
    }

    return current_card;
}

/**
 * @brief Scarta una carta.
 * 
 * @param card Puntatore alla carta da scartare.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void discard_card(Card* card, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    assert (card != NULL && "Carta da scartare non valida!");

    // Aggiunge la carta scartata in cima al mazzo degli scarti
    card->next_card = *discard_deck; // Imposta la prossima carta della carta scartata alla testa del mazzo degli scarti
    *discard_deck = card; // Imposta la carta scartata come testa del mazzo degli scarti

    return;
}

/**
 * @brief Seleziona e scarta una carta tramite indice dalla mano del giocatore.
 * 
 * @param card_index Indice della carta da scartare.
 * @param player_hand Puntatore alla mano del giocatore.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void select_and_discard_card(int card_index, Card** player_hand, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Selezione della carta da scartare dalla mano del giocatore
    Card* selected_card = select_card(card_index, player_hand, true);
    discard_card(selected_card, discard_deck); // Scarta la carta selezionata

    return;
}

/**
 * @brief Controlla se una determinata carta è presente in un mazzo.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @param card Puntatore alla carta da cercare.
 * @return true se la carta è presente nel mazzo, false altrimenti.
 */
bool deck_contains_card(const Card* deck, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    bool found = false; // Inizializza il flag di ricerca a false

    // Scorre tutte le carte del mazzo
    for (Card* current_card = deck; current_card != NULL && !found; current_card = current_card->next_card) {
        found = strcmp(current_card->name, card->name) == 0; // Confronta il nome della carta corrente con il nome della carta da cercare
    }

    return found;
}

/**
 * @brief Controlla se un determinato tipo di carta è presente in un mazzo.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @param required_type Tipo di carta richiesto.
 * @return true se il tipo di carta è presente nel mazzo, false altrimenti.
 */
bool deck_contains_type(const Card* deck, Type_card required_type) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    bool found = false; // Inizializza il flag di ricerca a false

    // Scorre tutte le carte del mazzo
    for (Card* current_card = deck; current_card != NULL && !found; current_card = current_card->next_card) {
        found = (
            required_type == ALL // Se il tipo richiesto è ALL, allora la carta è valida
            || current_card->type == required_type // Se il tipo della carta corrente è uguale al tipo richiesto, allora la carta è valida
            || (required_type == STUDENTE && (
                current_card->type == MATRICOLA
                || current_card->type == STUDENTE_SEMPLICE
                || current_card->type == LAUREANDO
            )) // Se il tipo richiesto è STUDENTE, allora la carta è valida se è una carta MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
        ); // Confronta il tipo della carta corrente con il tipo richiesto
    }

    return found;
}

/**
 * @brief Controlla se un determinato effetto è presente in una delle carte del mazzo.
 * 
 * @param deck Puntatore al mazzo di carte.
 * @param action Azione dell'effetto richiesto.
 * @param target_player Giocatore bersaglio dell'effetto richiesto.
 * @param target_card Carta bersaglio dell'effetto richiesto.
 * @return true se l'effetto è presente in una delle carte del mazzo, false altrimenti.
 */
bool deck_contains_effect(const Card* deck, Action action, Type_Player target_player, Type_card target_card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    bool found = false; // Inizializza il flag di ricerca a false

    // Scorre tutte le carte del mazzo
    for (Card* current_card = deck; current_card != NULL && !found; current_card = current_card->next_card) {
        // Controlla se l'effetto è presente nella carta corrente
        found = has_effect(current_card->effects, current_card->num_effects, action, target_player, target_card);
    }

    return found;
}