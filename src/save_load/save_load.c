#include "save_load.h"
#include "../card/card.h"
#include "../utils/utils.h"
#include "../core/game.h"
#include "../logging/logging.h"

/**
 * @brief Carica un mazzo di carte da un file binario di salvataggio.
 * 
 * @param file Puntatore al file da cui caricare le carte.
 * @param num_cards Numero di carte da caricare.
 * @return Card* Puntatore alla testa della lista di carte.
 */
Card* load_saved_deck(FILE* file, int num_cards) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Card* deck = NULL; // Inizializzazione del mazzo

    for (int i = 0; i < num_cards; i++) {
        // Alloca la memoria per la carta
        Card* card = (Card*) safe_calloc(1, sizeof(Card));
        // Legge la carta dal file
        safe_fread(card, sizeof(Card), 1, file);

        // Sanitizza e rimuove gli spazi dal nome
        sanitize_string(card->name);
        strip_string(card->name);

        // Sanitizza e rimuove gli spazi dalla descrizione
        sanitize_string(card->description);
        strip_string(card->description);

        // SE la carta ha effetti
        if (card->num_effects > 0) {
            // Alloca la memoria per gli effetti della carta
            card->effects = (Effect*) safe_calloc(card->num_effects, sizeof(Effect));
            // Legge gli effetti della carta
            safe_fread(card->effects, sizeof(Effect), card->num_effects, file);
        } else {
            card->effects = NULL; // Per evitare problemi imposto a NULL se non ci sono effetti
        }

        card->next_card = NULL; // Imposta il puntatore alla prossima carta a NULL

        deck = add_card(deck, card); // Aggiunge la carta al mazzo
    }

    return deck;
}

/**
 * @brief Carica i giocatori da un file binario di salvataggio.
 * 
 * @param file Puntatore al file da cui caricare i giocatori.
 * @param num_players Numero di giocatori da caricare.
 * @return Player* Puntatore alla testa della lista circolare dei giocatori.
 */
Player* load_saved_players(FILE* file, int num_players) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* head = NULL; // Inizializzazione della testa della lista circolare

    for (int i = 0; i < num_players; i++) {
        // Alloca la memoria per il giocatore
        Player* player = (Player*) safe_calloc(1, sizeof(Player));
        // Legge il giocatore dal file
        safe_fread(player, sizeof(Player), 1, file);

        // Sanitizza e rimuove gli spazi dal nome
        sanitize_string(player->name);
        strip_string(player->name);

        // Carica il mazzo delle carte in mano
        int num_hand_cards = 0;
        safe_fread(&num_hand_cards, sizeof(int), 1, file);
        player->hand = load_saved_deck(file, num_hand_cards);

        // Carica il mazzo delle carte dell'aula studio
        int num_classroom_cards = 0;
        safe_fread(&num_classroom_cards, sizeof(int), 1, file);
        player->classroom = load_saved_deck(file, num_classroom_cards);

        // Carica il mazzo delle carte bonus/malus
        int num_magic_cards = 0;
        safe_fread(&num_magic_cards, sizeof(int), 1, file);
        player->magic_cards = load_saved_deck(file, num_magic_cards);

        // SE la testa è NULL => Inizializza la lista circolare
        if (head == NULL) {
            head = player; // Imposta il giocatore come testa
            head->next_player = head; // Collega il giocatore a se stesso
        } else {
            // Trova l'ultimo giocatore della lista circolare
            Player* current = head;
            while (current->next_player != head) {
                current = current->next_player;
            }

            current->next_player = player; // Collega l'ultimo giocatore al nuovo giocatore
            player->next_player = head; // Collega il nuovo giocatore alla testa
        }
    }

    return head;
}

/**
 * @brief Carica una partita da un file binario di salvataggio.
 * 
 * @param path_file Percorso del file di salvataggio.
 * @param game_name Nome della partita.
 */
void load_saved_game(const char* path_file, const char* game_name) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE *file = safe_fopen(path_file, "rb"); // Apertura del file in lettura binaria
    printf("[" YEL "S" RESET "] Caricamento del salvataggio \"" GRN "%s" RESET "\" in corso...\n", path_file);

    // Carica i giocatori della partita
    int num_players = 0;
    safe_fread(&num_players, sizeof(int), 1, file);
    Player* players = load_saved_players(file, num_players);

    // Carica il mazzo di pesca
    int num_draw_deck_cards = 0;
    safe_fread(&num_draw_deck_cards, sizeof(int), 1, file);
    Card* draw_deck = load_saved_deck(file, num_draw_deck_cards);

    // Carica il mazzo degli scarti
    int num_discard_deck_cards = 0;
    safe_fread(&num_discard_deck_cards, sizeof(int), 1, file);
    Card* discard_deck = load_saved_deck(file, num_discard_deck_cards);

    // Carica il mazzo dell'aula studio
    int num_study_room_cards = 0;
    safe_fread(&num_study_room_cards, sizeof(int), 1, file);
    Card* study_room = load_saved_deck(file, num_study_room_cards);

    fclose(file); // Chiusura del file

    log_load_game(game_name); // Registra l'azione di caricamento della partita nel file di log

    // Avvia la partita con i dati caricati
    play_game(game_name, players, draw_deck, discard_deck, study_room);
    return;
}

/**
 * @brief Salva un mazzo di carte in un file binario di salvataggio.
 * 
 * @param file Puntatore al file in cui salvare le carte.
 * @param deck Puntatore al mazzo di carte da salvare.
 */
void save_deck(FILE* file, Card* deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Card* current_card = deck; // Inizializzazione della carta corrente

    // Scorre tutte le carte del mazzo
    while (current_card != NULL) {
        // Scrive la carta nel file
        safe_fwrite(current_card, sizeof(Card), 1, file);

        // SE la carta ha effetti
        if (current_card->num_effects > 0) {
            // Scrive gli effetti della carta nel file
            safe_fwrite(current_card->effects, sizeof(Effect), current_card->num_effects, file);
        }

        current_card = current_card->next_card; // Passa alla prossima carta
    }

    return;
}

/**
 * @brief Salva i giocatori in un file binario di salvataggio.
 * 
 * @param file Puntatore al file in cui salvare i giocatori.
 * @param players Puntatore alla testa della lista circolare dei giocatori.
 */
void save_players(FILE* file, Player* players) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current = players; // Inizializzazione del giocatore corrente
    do {
        // Scrive il giocatore nel file
        safe_fwrite(current, sizeof(Player), 1, file);

        // Scrive il mazzo delle carte in mano del giocatore nel file
        int num_hand_cards = count_cards(current->hand);
        safe_fwrite(&num_hand_cards, sizeof(int), 1, file);
        save_deck(file, current->hand);

        // Scrive il mazzo delle carte dell'aula studio del giocatore nel file
        int num_classroom_cards = count_cards(current->classroom);
        safe_fwrite(&num_classroom_cards, sizeof(int), 1, file);
        save_deck(file, current->classroom);

        // Scrive il mazzo delle carte bonus/malus del giocatore nel file
        int num_magic_cards = count_cards(current->magic_cards);
        safe_fwrite(&num_magic_cards, sizeof(int), 1, file);
        save_deck(file, current->magic_cards);

        current = current->next_player; // Passa al prossimo giocatore
    } while (current != players); // Continua finché non torna alla testa

    return;
}

/**
 * @brief Salva una partita in un file binario di salvataggio.
 * 
 * @param filename Percorso del file di salvataggio.
 * @param players Puntatore alla testa della lista circolare dei giocatori.
 * @param draw_deck Puntatore al mazzo di carte da cui pescare.
 * @param discard_deck Puntatore al mazzo degli scarti.
 * @param study_room Puntatore al mazzo dell'aula studio.
 */
void save_game(char* filename, Player* players, Card* draw_deck, Card* discard_deck, Card* study_room) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    printf("\n[" YEL "S" RESET "] Salvataggio della partita in \"" GRN "%s" RESET "\" in corso...\n", filename);

    FILE* file = safe_fopen(filename, "wb"); // Apertura del file in scrittura binaria

    // Scrive i giocatori nel file
    int num_players = count_players(players);
    safe_fwrite(&num_players, sizeof(int), 1, file);
    save_players(file, players);

    // Scrive il mazzo di pesca nel file
    int num_draw_deck_cards = count_cards(draw_deck);
    safe_fwrite(&num_draw_deck_cards, sizeof(int), 1, file);
    save_deck(file, draw_deck);

    // Scrive il mazzo degli scarti nel file
    int num_discard_deck_cards = count_cards(discard_deck);
    safe_fwrite(&num_discard_deck_cards, sizeof(int), 1, file);
    save_deck(file, discard_deck);

    // Scrive il mazzo dell'aula studio nel file
    int num_study_room_cards = count_cards(study_room);
    safe_fwrite(&num_study_room_cards, sizeof(int), 1, file);
    save_deck(file, study_room);

    fclose(file); // Chiusura del file
    return;
}

/**
 * @brief Controlla se un salvataggio è già stato registrato.
 * 
 * @param save_name Nome del salvataggio da controllare.
 * @param registered_saves Array di stringhe dei salvataggi registrati.
 * @param num_registered_saves Numero di salvataggi registrati.
 * @return true Se il salvataggio è già stato registrato, false altrimenti.
 */
bool is_already_registered(const char* save_name, const char** registered_saves, int num_registered_saves) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    bool is_registered = false; // Inizializzazione del flag di controllo

    // Scorre tutti i salvataggi registrati
    for (int i = 0; i < num_registered_saves && !is_registered; i++) {
        // SE il nome del salvataggio è uguale (case insensitive) a un salvataggio registrato
        if (strcmp_insensitive(save_name, registered_saves[i]) == true) {
            is_registered = true; // Imposta il flag a true
        }
    }

    return is_registered;
}

/**
 * @brief Registra un nuovo salvataggio.
 * 
 * @param filename Percorso del file di salvataggio.
 * @param save_name Nome del salvataggio da registrare.
 * @param registered_saves Array di stringhe dei salvataggi registrati.
 * @param num_registered_saves Puntaore al numero di salvataggi registrati.
 * @return char** Array di stringhe dei salvataggi registrati aggiornato.
 */
char** register_new_save(const char* filename, char* save_name, char** registered_saves, int* num_registered_saves) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Verifica SE il salvataggio non è già stato registrato
    if (!is_already_registered(save_name, registered_saves, *num_registered_saves)) {
        // Alloca la memoria per il nuovo salvataggio
        registered_saves = (char**) safe_realloc(registered_saves, (*num_registered_saves + 1) * sizeof(char*));
        // Alloca la memoria per la nuova riga
        registered_saves[*num_registered_saves] = (char*) safe_calloc(strlen(save_name) + 1, sizeof(char));

        // Copia il nome del salvataggio nella nuova riga
        strncpy(registered_saves[*num_registered_saves], save_name, strlen(save_name));
        (*num_registered_saves)++; // Incrementa il numero di righe

        FILE* file = safe_fopen(filename, "a"); // Apre il file in append
        fprintf(file, "%s\n", save_name); // Scrive il nome del salvataggio nel file
        fclose(file); // Chiude il file
    }

    return registered_saves; // Ritorna l'array di salvataggi registrati
}

/**
 * @brief Carica i salvataggi registrati da un file.
 * 
 * @param filename Percorso del file dei salvataggi registrati.
 * @param num_registered_saves Puntaore al numero di salvataggi registrati.
 * @return char** Array di stringhe dei salvataggi registrati.
 */
char** load_registered_saves(const char* filename, int* num_registered_saves) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    *num_registered_saves = 0; // Inizializza il numero di righe
    char** registered_saves = NULL; // Inizializza l'array di puntatori

    // Verifica SE il file dei salvataggi esiste
    if (!file_exists(filename)) {
        return registered_saves; // Ritorna l'array vuoto
    }

    FILE* file = safe_fopen(filename, "r"); // Apre il file in lettura

    char buffer[MAX_FILENAME_LENGTH + 1]; // Buffer temporaneo per la lettura del file
    char full_path_buffer[MAX_FILENAME_LENGTH + SAVES_FOLDER_LEN + SAVES_EXTENSION_LEN + 1]; // Buffer temporaneo per il percorso completo del file

    while (fscanf(file, " %" STR_MAX_FILENAME_LENGTH "[^\n]s", buffer) == 1) {
        // Sanitizza e rimuove gli spazi dal buffer
        sanitize_string(buffer);
        strip_string(buffer);

        // Costruzione del percorso del file di salvataggio
        sprintf(full_path_buffer, "%s%s%s", SAVES_FOLDER, buffer, SAVES_EXTENSION);

        // Verifica SE il nome del file è valido, SE il file esiste e SE il file non è già stato registrato
        if (is_valid_filename(buffer) && file_exists(full_path_buffer) && !is_already_registered(buffer, registered_saves, *num_registered_saves)) {
            // Alloca la memoria per la nuova riga
            registered_saves = (char**) safe_realloc(registered_saves, (*num_registered_saves + 1) * sizeof(char*));
            registered_saves[*num_registered_saves] = (char*) safe_calloc(strlen(buffer) + 1, sizeof(char));

            // Copia il contenuto del buffer nella nuova riga
            strncpy(registered_saves[*num_registered_saves], buffer, strlen(buffer));
            (*num_registered_saves)++; // Incrementa il numero di righe
        }
    }

    fclose(file); // Chiude il file
    return registered_saves;
}

/**
 * @brief Liberazione della memoria allocata per gli array di salvataggi registrati.
 * 
 * @param registered_saves Array di stringhe dei salvataggi registrati.
 * @param num_registered_saves Numero di salvataggi registrati.
 */
void free_registered_saves(char** registered_saves, int num_registered_saves) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // SE l'array di puntatori non è vuoto
    if (registered_saves != NULL) {        
        // Scorre tutte le righe dell'array
        for (int i = 0; i < num_registered_saves; i++) {
            free(registered_saves[i]); // Libera la memoria allocata per ogni riga
        }

        free(registered_saves); // Libera l'array di puntatori
    }

    return;
}