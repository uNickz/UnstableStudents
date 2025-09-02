#include "game.h"
#include "../player/player.h"
#include "../card/card.h"
#include "../save_load/save_load.h"
#include "../logging/logging.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief Inizializza una nuova partita.
 * 
 * @param registered_saves Puntatore al puntatore all'array dei salvataggi registrati.
 * @param num_registered_saves Puntatore al numero di salvataggi registrati.
 */
void init_new_game(char*** registered_saves, int* num_registered_saves) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Costruzione del separatore del round
    char* round_separator = repeat_string(UNICODE_BORDER_HORIZONTAL, ROUND_SEPARATOR_SIZE);
    printf(BOLD "%s%s%s\n" RESET, UNICODE_BORDER_VERTICAL, round_separator, UNICODE_BORDER_VERTICAL);

    printf("\n[" HGRN "+" RESET "] Inizializzazione di una nuova partita...\n");

    char game_name[MAX_FILENAME_LENGTH + 1]; // Buffer per il nome della partita
    bool is_valid_name = false; // Flag per il nome valido della partita

    do {
        printf("\n[" HBLU "i" RESET "] Inserisci il nome della partita (caratteri ammessi a-z, A-Z, 0-9, \"_\" e \"-\"):\n");
        printf("> ");
        scanf(" %" STR_MAX_FILENAME_LENGTH "[^\n]s", game_name);
        clear_buffer();

        // SE il nome della partita non è valido
        if (!is_valid_filename(game_name)) {
            printf("[" RED "!" RESET "] Nome della partita non valido. Riprova!\n");
        } else if (is_already_registered(game_name, *registered_saves, *num_registered_saves)) {
            // SE il nome della partita è già stato utilizzato
            printf("[" RED "!" RESET "] Il nome della partita è già stato utilizzato. Riprova!\n");
        } else {
            is_valid_name = true; // Imposta il nome della partita come valido
        }

    } while (!is_valid_name); // Continua finché il nome della partita non è valido

    // Inizializzazione del numero di giocatori
    int num_players = 0;
    do {
        printf("\n[" HBLU "i" RESET "] Quanti giocatori parteciperanno alla partita? (min. 2, max. 4)\n");
        printf("> ");

        read_int(&num_players); // Legge il numero di giocatori
        if (num_players < MIN_PLAYERS || num_players > MAX_PLAYERS) {
            printf("[" RED "!" RESET "] Numero di giocatori non valido. Riprova!\n");
        }

    } while (num_players < MIN_PLAYERS || num_players > MAX_PLAYERS);

    Player* players = NULL; // Inizializzazione della lista dei giocatori
    for (int i = 0; i < num_players; i++) {
        char temp_name[MAX_NAME_LENGTH + 1]; // Buffer per il nome del giocatore

        do {
            printf("\n[" HBLU "i" RESET "] Inserisci il nome del %d° giocatore:\n", i + 1);
            printf("> ");
            scanf(" %" STR_MAX_NAME_LENGTH "[^\n]s", temp_name);

            // SE il nome del giocatore non è valido
            if (strlen(temp_name) == 0) {
                printf("[" RED "!" RESET "] Nome non valido. Riprova!\n");
            }

        } while (strlen(temp_name) == 0); // Continua finché il nome del giocatore non è valido

        // Sanitizza e rimuove gli spazi dal nome
        sanitize_string(temp_name);
        strip_string(temp_name);
        players = add_player(players, temp_name); // Aggiunge il giocatore alla lista
    }

    // Inizializzazione del mazzo di carte
    int num_draw_deck_cards = 0, num_unique_cards = 0;
    Card* draw_deck = load_cards(DEFAULT_DECK_PATH, &num_draw_deck_cards, &num_unique_cards);

    draw_deck = shuffle_deck(draw_deck, num_draw_deck_cards); // Mescola il mazzo di carte

    // Inizializzazione del mazzo dell'aula studio
    Card* study_room = NULL;
    separate_matricola_cards(&draw_deck, &study_room); // Separa le carte MATRICOLA dal mazzo di carte

    Card* discard_deck = NULL; // Placeholder per il mazzo degli scarti
    init_hand(players, &draw_deck, &study_room, &discard_deck); // Inizializza le carte in mano e dell'aula studio per ogni giocatore

    // Libera la memoria dinamica del separatore del round
    free(round_separator);

    // Registra il salvataggio della partita
    *registered_saves = register_new_save(DEFAULT_SAVES_LIST_PATH, game_name, *registered_saves, num_registered_saves);

    // Registra l'azione di inizio di una nuova partita nel file di log
    log_init_game(game_name);

    // Avvia la partita
    play_game(game_name, players, draw_deck, discard_deck, study_room);
    return;
}

/**
 * @brief Gioca una partita.
 * 
 * @param game_name Nome della partita.
 * @param players Puntatore alla testa della lista circolare dei giocatori.
 * @param draw_deck Punatore al mazzo di carte da cui pescare.
 * @param discard_deck Puntatore al mazzo degli scarti.
 * @param study_room Puntatore al mazzo dell'aula studio.
 */
void play_game(const char* game_name, Player* players, Card* draw_deck, Card* discard_deck, Card* study_room) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Costruzione del percorso del file di salvataggio
    char full_path_game_name[MAX_FILENAME_LENGTH + SAVES_FOLDER_LEN + SAVES_EXTENSION_LEN + 1];
    sprintf(full_path_game_name, "%s%s%s", SAVES_FOLDER, game_name, SAVES_EXTENSION);

    bool is_new_game = !file_exists(full_path_game_name); // Flag per sapere se è una nuova partita

    const char* players_colors[] = {RED, GRN, YEL, BLU}; // Colori dei giocatori
    int num_players = count_players(players); // Numero di giocatori

    int round = 0; // Contatore dei round

    Player* current_player = players; // Puntatore al giocatore corrente
    Player* p = NULL; // Puntatore temporaneo per la gestione dei giocatori
    int player_action = 0; // Scelta dell'azione del giocatore
    int round_offset = 0; // Offset per il calcolo del colore del giocatore
    bool valid_action = false; // Flag per l'azione valida del giocatore
    bool game_over = false; // Flag per il termine del gioco

    // Costruzione del separatore del round
    char* round_separator = repeat_string(UNICODE_BORDER_HORIZONTAL, ROUND_SEPARATOR_SIZE);

    do {
        // Fase 0: Inizio del turno
        log_prefix_round(true, false); // Incrementa il contatore dei round nel prefisso del log

        // Fase 1: Salvataggio della partita
        // Prima dell'inizio di ogni turno va salvata la partita nell'apposito file binario di salvataggio (vedi slide 63 e 64: File di salvataggio).
        // SE è una nuova partita o il round è maggiore di 0 => salva la partita
        if (is_new_game || round > 0) {
            save_game(full_path_game_name, current_player, draw_deck, discard_deck, study_room); // Salva la partita
        }

        printf(BOLD "\n%s%s%s\n" RESET, UNICODE_BORDER_VERTICAL, round_separator, UNICODE_BORDER_VERTICAL);
        printf("\n[" HCYN "#" RESET "] %d° Round -> Turno di %s%s%s\n", round+1, players_colors[round % num_players], current_player->name, RESET);

        // Fase 1.5: Attivazione degli effetti delle carte presenti nel campo di gioco del giocatore
        // Si dovrà controllare se nel campo di gioco del giocatore sono presenti carte bonus/malus che hanno come QUANDO dell’Effetto "INIZIO" e, in caso affermativo, svolgere gli effetti di queste carte.
        for (Card* current_card = current_player->magic_cards; current_card != NULL; current_card = current_card->next_card) {
            check_effect(current_player, current_card, INIZIO, &draw_deck, &discard_deck); // Controlla l'effetto della carta bonus/malus
        }
        // Successivamente fare la stessa cosa con le carte presenti nell'aula studio.
        for (Card* current_card = current_player->classroom; current_card != NULL; current_card = current_card->next_card) {
            check_effect(current_player, current_card, INIZIO, &draw_deck, &discard_deck); // Controlla l'effetto della carta studente
        }

        // Fase 2: Pescare una carta dal mazzo
        draw_card(&current_player, &draw_deck, &discard_deck, true); // Pesca una carta dal mazzo

        // Fase 3: Azione del giocatore
        do {
            printf(ACTION_MENU); // Stampa il menu delle azioni
            read_int(&player_action); // Legge l'azione del giocatore
            valid_action = true; // Imposta l'azione del giocatore come valida

            switch (player_action) {
                case PLAY_ACTION: // Gioca una carta
                    // Controllo se il giocatore ha carte giocabili
                    // SE non dispone di carte giocabili => verrà considerata come azione non valida
                    // e l'unica azione possibile sarà quella di pescare una carta
                    if (count_playable_cards(current_player, ALL) == 0) {
                        printf("\n[" RED "!" RESET "] %s non hai carte giocabili! Devi pescare una carta!\n", current_player->name);
                        valid_action = false; // Imposta l'azione del giocatore come non valida
                    } else {
                        // Non è possibile inserire due carte identiche nella propria aula (o bonusmalus) o quella di un altro giocatore.
                        // SE si prova a giocare una carta già presente nell'aula o bonus/malus
                        // => non sarà possibile giocare la carta => la carta verrà scartata
                        play_hand(current_player, ALL, &draw_deck, &discard_deck, true); // Gioca una carta
                    }
                    break;
                case DRAW_ACTION:
                    // Pesca un'ulteriore carta dal mazzo
                    draw_card(&current_player, &draw_deck, &discard_deck, true);
                    break;
                case SHOW_ACTION:
                    // Mostra la tua mano, l'aula studio e le carte bonus/malus
                    print_player(current_player, true, players_colors[round % num_players]);
                    break;
                case SHOW_OTHERS_ACTION:
                    // Visualizza lo stato degli altri giocatori (numero carte in mano, l'aula studio e le carte bonus/malus)
                    p = current_player->next_player; // Passa al prossimo giocatore (il primo giocatore dopo il giocatore corrente)
                    round_offset = round+1; // Inizializza l'offset per il calcolo del colore del giocatore

                    // Continua finché non torna al giocatore corrente
                    while (p != current_player) {
                        // NB: per [TASTO 4] tenere a mente l'effetto MOSTRA
                        bool show_hand = deck_contains_effect(p->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(p->classroom, MOSTRA, IO, ALL); // Controlla se il giocatore deve mostrare la mano a tutti i giocatori
                        print_player(p, show_hand, players_colors[round_offset++ % num_players]); // Stampa il giocatore
                        printf("\n");
                        p = p->next_player; // Passa al prossimo giocatore
                    }

                    break;
                case EXIT_ACTION:
                    printf("\n[" RED "-" RESET "] Uscita dalla partita in corso...\n");
                    break;
                default:
                    printf("\n[" RED "!" RESET "] Scelta non valida! Riprova!\n");
                    valid_action = false; // Imposta l'azione del giocatore come non valida
                    break;
            }
        } while (!valid_action || (player_action == SHOW_ACTION || player_action == SHOW_OTHERS_ACTION));

        // Fase 4: Fine del turno
        // Ogni giocatore alla fine del proprio turno può avere un massimo di 5 carte.
        // Scartare il quantitativo di carte necessario affinché il numero di carte in mano sia
        // minore o uguale a 5. Il giocatore sceglie che carte scartare nel mazzo degli scarti.
        if (player_action != EXIT_ACTION) {
            check_hand(current_player, players_colors[round % num_players], &discard_deck); // Controlla la mano del giocatore

            // Fase 5: Fine del gioco
            // Alla fine di ogni turno si controlla la condizione di vittoria del giocatore corrente.
            // Se la condizione è soddisfatta il gioco finisce e si da un messaggio di congratulazioni
            // al giocatore, altrimenti si passa il turno al giocatore successivo.
            if (check_win_condition(current_player)) {
                printf("\n[" HCYN "#" RESET "] Partita Terminata! - Recap Partita:\n\n");

                // Visualizza lo stato finale della partita
                Player *p = current_player;
                round_offset = round; // Inizializza l'offset per il calcolo del colore del giocatore

                do {
                    print_player(p, true, players_colors[round_offset++ % num_players]); // Stampa il giocatore
                    printf("\n");
                    p = p->next_player; // Passa al prossimo giocatore
                } while (p != current_player); // Continua finché non torna al giocatore corrente

                // Visualizza lo stato del mazzo degli scarti
                print_deck(discard_deck, "Mazzo degli Scarti", 0, false, DISCARD_DECK_COLOR);

                printf("\n[" RED "!!!" RESET "] Il giocatore \"%s%s%s\" ha vinto! Congratulazioni!\n", players_colors[round % num_players], current_player->name, RESET);

                // Registra l'azione di vittoria nel file di log
                log_winner_game(current_player);

                game_over = true; // Imposta il termine del gioco
            } else {
                current_player = current_player->next_player; // Passa al prossimo giocatore
                round++; // Incrementa il contatore dei round
            }
        }

    } while (player_action != EXIT_ACTION && !game_over);

    // Fase 4: Fine del gioco
    printf(BOLD "\n%s%s%s\n" RESET, UNICODE_BORDER_VERTICAL, round_separator, UNICODE_BORDER_VERTICAL);

    // Libera la memoria allocata dinamicamente
    free(round_separator);
    free_deck(draw_deck);
    free_deck(study_room);
    free_deck(discard_deck);
    free_players(players);

    return;
}

/**
 * @brief Controlla la condizione di vittoria di un giocatore.
 * 
 * @param player Puntatore al giocatore da controllare.
 * @return true se il giocatore ha vinto, false altrimenti.
 */
bool check_win_condition(Player* player) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int num_students = count_cards(player->classroom); // Conteggio degli studenti presenti nell'aula studio

    // NB: la condizione di vittoria può essere influenzata da dei malus (vedi INGEGNERIZZAZIONE)
    if (deck_contains_effect(player->magic_cards, INGEGNERE, IO, STUDENTE) || deck_contains_effect(player->magic_cards, INGEGNERE, IO, ALL)) {
        return false;
    }

    return num_students >= NUM_STUDENTS_TO_WIN;
}