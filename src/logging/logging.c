#include "logging.h"
#include "../utils/utils.h"
#include <string.h>

/**
 * @brief Inizializza il file di log.
 * 
 * @return FILE* Puntatore al file di log.
 */
FILE* init_log() {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = NULL; // Inizializzazione del file di log

    // SE il file di log non esiste
    if (!file_exists(DEFAULT_LOG_FILE_PATH)) {
        log_file = safe_fopen(DEFAULT_LOG_FILE_PATH, "w"); // Crea il file di log in modalità scrittura
    } else {
        log_file = safe_fopen(DEFAULT_LOG_FILE_PATH, "a"); // Apre il file di log in modalità append
    }

    // SE il file di log non è stato aperto correttamente
    if (log_file == NULL) {
        printf(RED "[!] Errore nell'apertura del file di log \"%s\"!" RESET, DEFAULT_LOG_FILE_PATH);
        exit(EXIT_FAILURE);
    }

    return log_file;
}

/**
 * @brief Registra l'inizio di una nuova partita nel file di log.
 * 
 * @param game_name Nome della partita.
 */
void log_init_game(const char* game_name) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    log_prefix_round(false, true); // Reset del contatore dei round

    FILE* log_file = init_log(); // Inizializzazione del file di log

    char* game_separator = repeat_string(UNICODE_BORDER_HORIZONTAL, LOG_GAME_SEPARATOR_SIZE); // Separatore della partita

    fprintf(log_file, "\n%s\n", game_separator); // Scrive il separatore della partita
    fprintf(log_file, "\n[+] Inizio di una nuova partita \"%s\" salvata in \"%s%s%s\"\n\n", game_name, SAVES_FOLDER, game_name, SAVES_EXTENSION);

    free(game_separator); // Libera la memoria allocata per il separatore della partita
    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra il caricamento di una partita salvata nel file di log.
 * 
 * @param game_name Nome del salvataggio.
 */
void log_load_game(const char* game_name) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    log_prefix_round(false, true); // Reset del contatore dei round

    FILE* log_file = init_log(); // Inizializzazione del file di log

    char* game_separator = repeat_string(UNICODE_BORDER_HORIZONTAL, LOG_GAME_SEPARATOR_SIZE); // Separatore della partita

    fprintf(log_file, "\n%s\n", game_separator); // Scrive il separatore della partita
    fprintf(log_file, "\n[+] Caricamento del salvataggio \"%s%s%s\"\n\n", SAVES_FOLDER, game_name, SAVES_EXTENSION);

    free(game_separator); // Libera la memoria allocata per il separatore della partita
    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Scrive il prefisso del turno nel file di log.
 * 
 * @param next_round Flag per il prossimo turno.
 * @param reset Flag per il reset del numero dei turni.
 * @return int Numero del turno.
 */
int log_prefix_round(bool next_round, bool reset) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    static int round = 0; // Inizializzazione del contatore dei round

    // SE il reset è true
    if (reset == true) {
        round = 0; // Reset del contatore dei round
        return round;
    }

    FILE* log_file = init_log(); // Inizializzazione del file di log

    // SE il next_round è true
    if (next_round == true) {
        round++; // Incrementa il contatore dei round
    } else if (round > 0) {
        // SE il round è maggiore di 0 (ovvero, la partita è iniziata)
        fprintf(log_file, "[Turno %d]: ", round); // Scrive il prefisso del round
    }

    fclose(log_file); // Chiude il file di log
    return round;
}

/**
 * @brief Registra l'azione di pesca di una carta nel file di log.
 * 
 * @param player Puntatore al giocatore che ha pescato la carta.
 * @param card Puntatore alla carta pescata.
 */
void log_draw_card(const Player* player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di pesca della carta

    fprintf(log_file, "\"%s\" ha pescato la carta \"%s\".\n", player->name, card->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione di giocata di una carta nel file di log.
 * 
 * @param player Puntatore al giocatore che ha giocato la carta.
 * @param card Puntatore alla carta giocata.
 */
void log_play_card(const Player* player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di giocata della carta

    fprintf(log_file, "\"%s\" ha giocato la carta \"%s\".\n", player->name, card->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione di scarto di una carta nel file di log.
 * 
 * @param player Puntatore al giocatore che ha scartato la carta.
 * @param card Puntatore alla carta scartata.
 */
void log_discard_card(const Player* player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    fprintf(log_file, "\"%s\" ha scartato la carta \"%s\".\n", player->name, card->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione di blocco dell'effetto di una carta nel file di log.
 * 
 * @param player Puntatore al giocatore che ha bloccato l'effetto della carta.
 * @param card_used Puntatore alla carta che ha usato per bloccare l'effetto.
 * @param card_blocked Puntatore alla carta a cui è stato bloccato l'effetto.
 */
void log_block_effect(const Player* player, const Card* card_used, const Card* card_blocked) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    fprintf(log_file, "\"%s\" ha bloccato l'effetto della carta \"%s\" utilizzando la carta \"%s\".\n", player->name, card_blocked->name, card_used->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione dell'effetto scarta nel file di log.
 * 
 * @param player Puntatore al giocatore che ha attivato l'effetto.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta che è stata scartata.
 */
void log_scarta_effect(const Player* player, const Player* target_player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    fprintf(log_file, "\"%s\" ha scartato la carta \"%s\" dalla mano di \"%s\".\n", player->name, card->name, target_player->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione dell'effetto elimina nel file di log.
 * 
 * @param player Puntatore al giocatore che ha attivato l'effetto.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta che è stata eliminata.
 */
void log_elimina_effect(const Player* player, const Player* target_player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    switch (card->type) {
        case STUDENTE:
        case MATRICOLA:
        case STUDENTE_SEMPLICE:
        case LAUREANDO:
            fprintf(log_file, "\"%s\" ha eliminato la carta \"%s\" dall'aula studio di \"%s\".\n", player->name, card->name, target_player->name);
            break;
        case BONUS:
        case MALUS:
            fprintf(log_file, "\"%s\" ha eliminato la carta \"%s\" dall'aula bonus/malus di \"%s\".\n", player->name, card->name, target_player->name);
            break;
    }

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione dell'effetto ruba nel file di log.
 * 
 * @param player Puntatore al giocatore che ha attivato l'effetto.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta che è stata rubata.
 */
void log_ruba_effect(const Player* player, const Player* target_player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    switch (card->type) {
        case STUDENTE:
        case MATRICOLA:
        case STUDENTE_SEMPLICE:
        case LAUREANDO:
            fprintf(log_file, "\"%s\" ha rubato la carta \"%s\" dall'aula studio di \"%s\".\n", player->name, card->name, target_player->name);
            break;
        case BONUS:
        case MALUS:
            fprintf(log_file, "\"%s\" ha rubato la carta \"%s\" dall'aula bonus/malus di \"%s\".\n", player->name, card->name, target_player->name);
            break;
    }

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione dell'effetto prendi nel file di log.
 * 
 * @param player Puntatore al giocatore che ha attivato l'effetto.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta che è stata presa.
 */
void log_prendi_effect(const Player* player, const Player* target_player, const Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    fprintf(log_file, "\"%s\" ha preso la carta \"%s\" dalla mano di \"%s\".\n", player->name, card->name, target_player->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione di scambio delle carte in mano nel file di log.
 * 
 * @param player Puntatore al giocatore che ha attivato l'effetto.
 * @param target_player Puntatore al giocatore target dell'effetto.
 */
void log_scambia_effect(const Player* player, const Player* target_player) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scambio delle carte

    fprintf(log_file, "\"%s\" ha scambiato la propria mano con quella di \"%s\".\n", player->name, target_player->name);

    fclose(log_file); // Chiude il file di log
    return;
}

/**
 * @brief Registra l'azione di vittoria di un giocatore nel file di log.
 * 
 * @param player Puntatore al giocatore vincitore.
 */
void log_winner_game(const Player* player) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* log_file = init_log(); // Inizializzazione del file di log

    int status = log_prefix_round(false, false); // Scrive il prefisso del round

    // SE il round è 0, significa che la partita non è ancora iniziata
    if (status == 0) return; // Dunque non registra l'azione di scarto della carta

    fprintf(log_file, "Il giocatore \"%s\" ha vinto la partita!\n", player->name);

    fclose(log_file); // Chiude il file di log
    return;
}