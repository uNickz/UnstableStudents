#include "effect.h"

#include "../utils/utils.h"
#include "../player/player.h"
#include "../card/card.h"
#include "../logging/logging.h"

/**
 * @brief Controlla se un determinato effetto è presente in una carta.
 * 
 * @param effects Puntaore all'array di effetti della carta.
 * @param num_effects Numero di effetti della carta.
 * @param action Azione dell'effetto richiesto.
 * @param target_player Giocatore bersaglio dell'effetto richiesto.
 * @param target_card  Carta bersaglio dell'effetto richiesto.
 * @return true se l'effetto è presente nella carta, false altrimenti.
 */
bool has_effect(Effect* effects, int num_effects, Action action, Type_Player target_player, Type_card target_card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // -1 è un valore valido per action, target_player e target_card
    // in quanto si tratta di un valore speciale per indicare che non si vuole
    // effettuare il confronto per quel campo

    // Scorre tutti gli effetti della carta
    for (int i = 0; i < num_effects; i++) {
        if (
            (effects[i].action_effect == action || action == -1) &&               // SE l'azione dell'effetto è uguale all'azione richiesta o non è richiesta
            (effects[i].target_player == target_player || target_player == -1) && // SE il giocatore bersaglio dell'effetto è uguale al giocatore richiesto o non è richiesto
            (effects[i].target_card == target_card || target_card == -1)          // SE la carta bersaglio dell'effetto è uguale alla carta richiesta o non è richiesta
        ) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Controlla se una carta ha effetti attivabili in un determinato momento, e in caso affermativo li attiva.
 * 
 * @param owner_card Puntaore al giocatore proprietario della carta.
 * @param card Puntatore alla carta da controllare.
 * @param when_activate Valore enumerativo del momento di attivazione dell'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntaore al mazzo degli scarti.
 */
void check_effect(Player* owner_card, Card* card, When when_activate, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Controllo se la carta ha effetti
    if (card->num_effects == 0) {
        return;
    }

    // Controllo se la carta ha effetti attivabili in questo momento
    if (card->when_activate != when_activate) {
        return;
    }

    printf("\n[" HMAG "@" RESET "] Attivazione degli effetti in corso...\n");
    print_card(card); // Stampa la carta giocata

    // Controllo se l'effetto è opzionale
    if (card->optional == true) {
        // Chiedere all'utente se attivare l'effetto
        printf("\n[" HBLU "i" RESET "] %s vuoi attivare l'effetto della carta \"%s%s%s\"? (" GRN "s" RESET "/" RED "n" RESET "):\n", owner_card->name, get_color_by_type(card->type), card->name, RESET);
        char choice = read_char("sSnN", "> ", "[" RED "!" RESET "] Scelta non valida! Riprova!\n");

        if (choice == 'n' || choice == 'N') {
            printf("\n[" HBLU "i" RESET "] %s hai scelto di " RED "non attivare" RESET " l'effetto della carta \"%s%s%s\"!\n", owner_card->name, get_color_by_type(card->type), card->name, RESET);
            return;
        }

        printf("\n[" HBLU "i" RESET "] %s hai scelto di " GRN "attivare" RESET " l'effetto della carta \"%s%s%s\"!\n", owner_card->name, get_color_by_type(card->type), card->name, RESET);
    }

    // Inizializzazione delle variabili
    bool blocked_effect = false;
    Player* current_player = NULL;
    Player* target_player = NULL;
    Card* card_used_for_block = NULL;

    // Scorre tutti gli effetti della carta
    for (int i = 0; i < card->num_effects && !blocked_effect; i++) {
        // In base al giocatore target dell'effetto, controlla se l'effetto può essere bloccato e in caso affermativo chiede se bloccarlo
        // altrimenti applica l'effetto
        switch (card->effects[i].target_player) {
            case IO:
                target_player = NULL; // Ripristina il giocatore target (viene usato solo per gli effetti con target giocatore == TU)
                //  Gli effetti con TargetGiocatore IO e quelli che si giocano su se stessi non dovrebbero triggerare la possibilità di giocare MAI.
                activate_effect(card, &card->effects[i], owner_card, target_player, draw_deck, discard_deck); // Applica l'effetto
                break;
            case TU:
                if (target_player == NULL) {
                    // Chiede all'utente a quale giocatore applicare l'effetto
                    target_player = choose_player(owner_card, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                }

                // SE il giocatore target può bloccare l'effetto, chiedo SE vuole bloccarlo
                if (can_block_effect(target_player, card) && ask_block_effect(target_player, card)) {
                    // Blocca l'effetto utilizzando una carta ISTANTANEA
                    card_used_for_block = block_effect(target_player, card, discard_deck);
                    blocked_effect = true; // Imposta il flag di blocco a true
                } else {
                    activate_effect(card, &card->effects[i], owner_card, target_player, draw_deck, discard_deck); // Applica l'effetto
                }

                break;
            case VOI:
            case TUTTI: // IO non posso MAI difendermi da un effetto
                target_player = NULL; // Ripristina il giocatore target (viene usato solo per gli effetti con target giocatore == TU)

                current_player = owner_card->next_player; // Salto il giocatore corrente (me stesso => IO => owner_card)
                do {
                    // SE il giocatore target può bloccare l'effetto, chiedo SE vuole bloccarlo
                    if (can_block_effect(current_player, card) && ask_block_effect(current_player, card)) {
                        // Blocca l'effetto utilizzando una carta ISTANTANEA
                        card_used_for_block = block_effect(current_player, card, discard_deck);
                        blocked_effect = true; // Imposta il flag di blocco a true
                    }
                    current_player = current_player->next_player; // Passa al prossimo giocatore
                } while (current_player != owner_card && !blocked_effect);

                // Se un giocatore decide di bloccare l'effetto della carta lo blocca per tutti i giocatori coinvolti
                if (!blocked_effect) {
                    activate_effect(card, &card->effects[i], owner_card, target_player, draw_deck, discard_deck); // Applica l'effetto
                }
                break;
        }
    }

    return;
}

/**
 * @brief Attiva un effetto di una carta.
 * 
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param player Puntatore al giocatore che possiede la carta.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void activate_effect(Card* card, Effect* effect, Player* player, Player* target_player, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // In base all'azione dell'effetto, attiva l'effetto
    switch (effect->action_effect) {
        case GIOCA:
            // Attiva l'effetto di giocare una carta
            gioca_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case SCARTA:
            // Attiva l'effetto di scartare una carta
            scarta_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case ELIMINA:
            // Attiva l'effetto di eliminare una carta
            elimina_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case RUBA:
            // Attiva l'effetto di rubare una carta
            ruba_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case PESCA:
            // Attiva l'effetto di pescare una carta
            pesca_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case PRENDI:
            // Attiva l'effetto di prendere una carta
            prendi_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        case SCAMBIA:
            // Attiva l'effetto di scambiare una carta
            scambia_effect(player, target_player, card, effect, draw_deck, discard_deck);
            break;
        // Gli effetti di default non fanno nulla
        case BLOCCA:
        case MOSTRA:
        case IMPEDIRE:
        case INGEGNERE:
        default:
            printf("\n[" RED "!" RESET "] Attivazione dell'effetto di default!\n");
            break;
    }

    return;
}

/**
 * @brief Controlla se un giocatore può bloccare l'effetto di una carta.
 * 
 * @param player Puntatore al giocatore.
 * @param card Puntatore alla carta da bloccare.
 * @return true Se il giocatore può bloccare l'effetto della carta, false altrimenti.
 */
bool can_block_effect(Player* player, Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    bool can_block = false;     // Inizializza il flag di blocco a false
    bool can_play_card = true; // Inizializza il flag di giocabilità della carta a true

    // Scorre tutte le carte in mano del giocatore
    for (Card* current_card = player->hand; current_card != NULL && !can_block; current_card = current_card->next_card) {
        // SE la carta corrente è una carta ISTANTANEA
        if (current_card->type == ISTANTANEA) {
            // Controlla se la carta ISTANTANEA ha come effetto il blocco dell'effetto della carta
            can_block = has_effect(current_card->effects, current_card->num_effects, BLOCCA, IO, card->type) || has_effect(current_card->effects, current_card->num_effects, BLOCCA, IO, ALL);
        }
    }

    // SE il giocatore ha una carta ISTANTANEA che può bloccare l'effetto della carta
    if (can_block) {
        // Controlla se il giocatore ha delle carte MALUS che impediscono di giocare carte ISTANTANEE
        printf("\n[" HBLU "i" RESET "] %s possiedi una carta " BHGRN "ISTANTANEA" RESET " che può bloccare l'effetto di \"%s%s%s\"!\n", player->name, get_color_by_type(card->type), card->name, RESET);

        // Scorre tutte le carte del mazzo bonus/malus del giocatore
        for (Card* current_card = player->magic_cards; current_card != NULL && can_play_card; current_card = current_card->next_card) {
            // SE la carta corrente ha effetti e si attiva SEMPRE
            if (current_card->num_effects > 0 && current_card->when_activate == SEMPRE) {
                // Controlla se la carta corrente ha come effetto l'impedimento di giocare carte ISTANTANEE
                can_play_card = !has_effect(current_card->effects, current_card->num_effects, IMPEDIRE, IO, ISTANTANEA);
            }
        }

        // SE il giocatore non può giocare carte ISTANTANEE
        if (!can_play_card) {
            printf("\n[" RED "!" RESET "] Tuttavia non puoi giocare una carta " BHGRN "ISTANTANEA" RESET ", a causa di un " BHRED "MALUS" RESET ", per bloccare l'effetto di \"%s%s%s\"!\n", player->name, get_color_by_type(card->type), card->name, RESET);
        }
    }

    // Restituisce true se il giocatore può bloccare l'effetto della carta e può giocare carte ISTANTANEE, altrimenti false
    return can_block & can_play_card;
}

/**
 * @brief Chiede al giocatore se vuole bloccare l'effetto di una carta.
 * 
 * @param player Puntatore al giocatore.
 * @param card Puntatore alla carta da bloccare.
 * @return true Se il giocatore vuole bloccare l'effetto della carta, false altrimenti.
 */
bool ask_block_effect(Player* player, Card* card) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Chiede all'utente se vuole bloccare l'effetto della carta
    printf("\n[" HBLU "i" RESET "] %s vuoi bloccare l'effetto di \"%s%s%s\" utilizzando una carta " BHGRN "ISTANTANEA" RESET "? (" GRN "s" RESET "/" RED "n" RESET "):\n", player->name, get_color_by_type(card->type), card->name, RESET);
    char choice = read_char("sSnN", "> ", "[" RED "!" RESET "] Scelta non valida! Riprova!\n");

    if (choice == 'n' || choice == 'N') {
        printf("\n[" RED "-" RESET "] Hai scelto di " RED "non bloccare" RESET " l'effetto di \"%s%s%s\"!\n", get_color_by_type(card->type), card->name, RESET);
        return false;
    }

    printf("\n[" GRN "+" RESET "] Hai scelto di " GRN "bloccare" RESET " l'effetto di \"%s%s%s\"!\n", get_color_by_type(card->type), card->name, RESET);
    return true;
}

/**
 * @brief Blocca l'effetto di una carta utilizzando una carta ISTANTANEA.
 * 
 * @param player Puntatore al giocatore.
 * @param card Puntatore alla carta da bloccare.
 * @param discard_deck Puntatore al mazzo degli scarti.
 * @return Card* Puntatore alla carta ISTANTANEA giocata.
 */
Card* block_effect(Player* player, Card* card, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Chiede al giocatore di scegliere una carta ISTANTANEA da utilizzare per bloccare l'effetto della carta
    printf("\n[" HBLU "i" RESET "] %s scegli una carta " BHGRN "ISTANTANEA" RESET " da utilizzare per bloccare l'effetto di \"%s%s%s\":\n\n", player->name, get_color_by_type(card->type), card->name, RESET);
    print_deck(player->hand, "Carte in Mano", 0, false, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore

    // Inizializzazione delle variabili per la scelta della carta ISTANTANEA da utilizzare
    int card_index;
    Card* played_card;
    bool is_valid = false;
    do {
        // Chiede all'utente di scegliere una carta ISTANTANEA da utilizzare
        card_index = choice_card(player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da utilizzare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
        played_card = select_card(card_index, &player->hand, false); // Seleziona la carta scelta

        // SE la carta selezionata non è una carta ISTANTANEA
        if (played_card->type != ISTANTANEA) {
            printf("\n[" RED "!" RESET "] La carta selezionata non è una carta " BHGRN "ISTANTANEA" RESET "! Riprova!\n");
        } else if (!has_effect(played_card->effects, played_card->num_effects, BLOCCA, IO, card->type) && !has_effect(played_card->effects, played_card->num_effects, BLOCCA, IO, ALL)) {
            // SE la carta ISTANTANEA non può bloccare l'effetto della carta
            printf("\n[" RED "!" RESET "] La carta selezionata non può bloccare l'effetto di \"%s%s%s\"! Riprova!\n", get_color_by_type(card->type), card->name, RESET);
        } else {
            is_valid = true; // La carta selezionata è valida
        }

    } while (!is_valid);

    printf("\n[" GRN "+" RESET "] Hai utilizzato una carta " BHGRN "ISTANTANEA" RESET " per bloccare l'effetto di \"%s%s%s\"!\n", get_color_by_type(card->type), card->name, RESET);
    print_card(played_card); // Stampa la carta utilizzata per bloccare l'effetto

    // Scollega effettivamente la carta dalla mano del giocatore
    played_card = select_card(card_index, &player->hand, true);

    // Registra l'azione di blocco dell'effetto della carta nel file di log
    log_block_effect(player, played_card, card);

    // Applica l'effetto della carta istantanea, la carta viene scartata
    discard_card(played_card, discard_deck);

    return played_card;
}

/**
 * @brief Applica l'effetto di giocare una carta dalla propria mano.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void gioca_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // SE il giocatore non ha carte giocabili
            if (count_playable_cards(player, effect->target_card) == 0) {
                printf("\n[" RED "!" RESET "] %s non hai carte giocabili!\n", player->name);
            } else {
                // Fa giocare una carta dalla mano del giocatore
                play_hand(player, effect->target_card, draw_deck, discard_deck, true);
            }
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            // SE il giocatore target non ha carte giocabili
            if (count_playable_cards(target_player, effect->target_card) == 0) {
                printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte giocabili!\n", target_player->name);
            } else {
                // Fa giocare una carta dalla mano del giocatore target
                play_hand(target_player, effect->target_card, draw_deck, discard_deck, true);
            }
            break;

        case VOI:
            current_player = current_player->next_player; // Salta il giocatore corrente (me stesso => IO => player)
            do {
                // SE il giocatore corrente non ha carte giocabili
                if (count_playable_cards(current_player, effect->target_card) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte giocabili!\n", current_player->name);
                } else {
                    // Fa giocare una carta dalla mano del giocatore corrente
                    play_hand(current_player, effect->target_card, draw_deck, discard_deck, true);
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;

        case TUTTI:
            do {
                // SE il giocatore corrente non ha carte giocabili
                if (count_playable_cards(current_player, effect->target_card) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte giocabili!\n", current_player->name);
                } else {
                    // Fa giocare una carta dalla mano del giocatore corrente
                    play_hand(current_player, effect->target_card, draw_deck, discard_deck, true);
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di scartare una carta dalla propria mano.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void scarta_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta
    int card_index = 0;              // Inizializza l'indice della carta da scartare a 0
    bool show_hand = false;          // Inizializza il flag per mostrare la mano del giocatore a false
    Card* selected_card = NULL;      // Inizializza la carta selezionata a NULL

    // Si assume che effect->target_card sia sempre ALL
    // in quanto, solitamente, non dovresti poter vedere la mano degli altri giocatori
    // (a meno che non ci siano effetti malus che lo permettano) e quindi non poter vedere
    // e scegliere il tipo richiesto di carta da scartare

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // SE il giocatore non ha carte in mano
            if (count_cards(player->hand) == 0) {
                printf("\n[" RED "!" RESET "] %s non hai carte in mano da scartare!\n", player->name);
            } else {
                printf("\n[" HBLU "i" RESET "] %s scegli una carta da scartare:\n\n", player->name);
                print_deck(player->hand, "Carte in Mano", 0, false, HAND_DECK_COLOR); // Stampa le carte in mano

                // Chiede all'utente di scegliere una carta da scartare dalla mano
                card_index = choice_card(player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da scartare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                printf("\n[" RED "-" RESET "] %s hai scartato una carta dalla tua mano!\n", player->name);
                selected_card = select_card(card_index, &player->hand, false); // Seleziona la carta scelta dalla mano del giocatore
                print_card(selected_card); // Stampa la carta scartata

                // Registra l'azione di scartare la carta nel file di log
                log_scarta_effect(player, player, selected_card);

                // Seleziona, scollega e scarta la carta scelta dalla mano del giocatore
                select_and_discard_card(card_index, &player->hand, discard_deck);
            }
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            // SE il giocatore target non ha carte in mano
            if (count_cards(target_player->hand) == 0) {
                printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da scartare!\n", target_player->name);
            } else {
                printf("\n[" HBLU "i" RESET "] %s scegli una carta da scartare dalla mano di %s:\n\n", player->name, target_player->name);

                // Controlla se il giocatore può vedere la mano del giocatore target (effetti MOSTRA)
                show_hand = deck_contains_effect(target_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(target_player->classroom, MOSTRA, IO, ALL);
                print_deck(target_player->hand, "Carte in Mano", 0, show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore target

                // Chiede all'utente di scegliere una carta da scartare dalla mano del giocatore target
                card_index = choice_card(target_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da scartare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                printf("\n[" RED "-" RESET "] %s hai scartato una carta dalla mano di %s!\n", player->name, target_player->name);
                selected_card = select_card(card_index, &target_player->hand, false); // Seleziona la carta scelta dalla mano del giocatore target
                print_card(selected_card); // Stampa la carta scartata

                // Registra l'azione di scartare la carta nel file di log
                log_scarta_effect(player, target_player, selected_card);

                // Seleziona, scollega e scarta la carta scelta dalla mano del giocatore target
                select_and_discard_card(card_index, &target_player->hand, discard_deck);
            }
            break;

        case VOI:
            current_player = current_player->next_player; // Salta il giocatore corrente (me stesso => IO => player)
            do {
                // SE il giocatore corrente non ha carte in mano
                if (count_cards(current_player->hand) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da scartare!\n", current_player->name);
                } else {
                    printf("\n[" HBLU "i" RESET "] %s scegli una carta da scartare dalla mano di %s:\n\n", player->name, current_player->name);

                    // Controlla se il giocatore può vedere la mano del giocatore corrente (effetti MOSTRA)
                    show_hand = deck_contains_effect(current_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(current_player->classroom, MOSTRA, IO, ALL);
                    print_deck(current_player->hand, "Carte in Mano", 0, show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore corrente

                    // Chiede all'utente di scegliere una carta da scartare dalla mano del giocatore corrente
                    card_index = choice_card(current_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da scartare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                    printf("\n[" RED "-" RESET "] %s hai scartato una carta dalla mano di %s!\n", player->name, current_player->name);
                    selected_card = select_card(card_index, &current_player->hand, false); // Seleziona la carta scelta dalla mano del giocatore corrente
                    print_card(selected_card); // Stampa la carta scartata

                    // Registra l'azione di scartare la carta nel file di log
                    log_scarta_effect(player, current_player, selected_card);

                    // Seleziona, scollega e scarta la carta scelta dalla mano del giocatore corrente
                    select_and_discard_card(card_index, &current_player->hand, discard_deck);
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;

        case TUTTI:
            do {
                // SE il giocatore corrente non ha carte in mano
                if (count_cards(current_player->hand) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da scartare!\n", current_player->name);
                } else {
                    printf("\n[" HBLU "i" RESET "] %s scegli una carta da scartare dalla mano di %s:\n\n", player->name, current_player->name);

                    // Controlla se il giocatore può vedere la mano del giocatore corrente (effetti MOSTRA) o se è il giocatore corrente
                    show_hand = (player == current_player) || deck_contains_effect(current_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(current_player->classroom, MOSTRA, IO, ALL);
                    print_deck(current_player->hand, "Carte in Mano", 0, show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore corrente

                    // Chiede all'utente di scegliere una carta da scartare dalla mano del giocatore corrente
                    card_index = choice_card(current_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da scartare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                    printf("\n[" RED "-" RESET "] %s hai scartato una carta dalla mano di %s!\n", player->name, current_player->name);
                    selected_card = select_card(card_index, &current_player->hand, false); // Seleziona la carta scelta dalla mano del giocatore corrente
                    print_card(selected_card); // Stampa la carta scartata

                    // Registra l'azione di scartare la carta nel file di log
                    log_scarta_effect(player, current_player, selected_card);

                    // Seleziona, scollega e scarta la carta scelta dalla mano del giocatore corrente
                    select_and_discard_card(card_index, &current_player->hand, discard_deck);
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di eliminare una carta dall'aula studio o bonus/malus.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void elimina_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta
    int num_cards_classroom = 0, num_cards_magic = 0, card_index = 0; // Inizializza il numero di carte presenti nell'aula studio, nel mazzo bonus/malus e l'indice della carta scelta dall'utente a 0
    Card* deleted_card = NULL; // Inizializza la carta eliminata a NULL
    bool is_valid_choice = false; // Inizializza il flag per la validità della scelta dell'utente a false

    // Valori ammessi da effect->target_card per l'effetto ELIMINA:
    // STUDENTE, MATRICOLA, STUDENTE_SEMPLICE, LAUREANDO
    // BONUS, MALUS
    // ALL (qualsiasi carta presente nell'aula studio o bonus/malus)

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // In base al tipo di carta da eliminare 
            switch (effect->target_card) {
                case STUDENTE:
                case MATRICOLA:
                case STUDENTE_SEMPLICE:
                case LAUREANDO:
                    // SE il giocatore non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                    if (!deck_contains_type(player->classroom, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non hai carte di tipo %s%s%s da eliminare!\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        print_deck(player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore

                        do {
                            // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da eliminare
                            card_index = choice_card(player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            deleted_card = select_card(card_index, &player->classroom, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                            if (effect->target_card != STUDENTE && deleted_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, player, deleted_card);

                        // Seleziona e scarta la carta
                        select_and_discard_card(card_index, &player->classroom, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                case BONUS:
                case MALUS:
                    // SE il giocatore non ha carte bonus/malus
                    if (!deck_contains_type(player->magic_cards, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non hai carte di tipo %s%s%s da eliminare!\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        print_deck(player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte bonus/malus del giocatore

                        do {
                            // Chiede all'utente di scegliere una carta di tipo BONUS o MALUS da eliminare
                            card_index = choice_card(player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            deleted_card = select_card(card_index, &player->magic_cards, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è del tipo richiesto
                            if (deleted_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, player, deleted_card);

                        // Seleziona e scarta la carta
                        select_and_discard_card(card_index, &player->magic_cards, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                case ALL:
                    num_cards_classroom = count_cards(player->classroom); // Conta il numero di carte presenti nell'aula studio
                    num_cards_magic = count_cards(player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus

                    // SE il giocatore non ha carte da eliminare (aula studio + bonus/malus)
                    if ((num_cards_classroom + num_cards_magic) == 0) {
                        printf("\n[" RED "!" RESET "] Non hai carte da eliminare!\n");
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta da eliminare:\n\n", player->name);
                        print_deck(player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore
                        printf("\n");
                        print_deck(player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore

                        // Chiede all'utente di scegliere una carta da eliminare (aula studio + bonus/malus)
                        card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                        // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                        if (card_index > num_cards_classroom) {
                            card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus
                            deleted_card = select_card(card_index-1, &player->magic_cards, true); // Seleziona la carta scelta
                        } else {
                            // Altrimenti seleziona la carta nell'aula studio
                            deleted_card = select_card(card_index-1, &player->classroom, true); // Seleziona la carta scelta
                        }

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, player, deleted_card);

                        // Scarta la carta selezionata
                        discard_card(deleted_card, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                default:
                    printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto ELIMINA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    break;
            }
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            // In base al tipo di carta da eliminare
            switch (effect->target_card) {
                case STUDENTE:
                case MATRICOLA:
                case STUDENTE_SEMPLICE:
                case LAUREANDO:
                    // SE il giocatore target non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                    if (!deck_contains_type(target_player->classroom, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", target_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, target_player->name);
                        print_deck(target_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore target

                        do {
                            // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da eliminare al giocatore target
                            card_index = choice_card(target_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            deleted_card = select_card(card_index, &target_player->classroom, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                            if (effect->target_card != STUDENTE && deleted_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, target_player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, target_player, deleted_card);

                        // Seleziona e scarta la carta
                        select_and_discard_card(card_index, &target_player->classroom, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(target_player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                case BONUS:
                case MALUS:
                    // SE il giocatore target non ha carte bonus/malus
                    if (!deck_contains_type(target_player->magic_cards, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", target_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, target_player->name);
                        print_deck(target_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore target

                        do {
                            // Chiede all'utente di scegliere una carta di tipo BONUS o MALUS da eliminare al giocatore target
                            card_index = choice_card(target_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            deleted_card = select_card(card_index, &target_player->magic_cards, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è del tipo richiesto
                            if (deleted_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, target_player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, target_player, deleted_card);

                        // Seleziona e scarta la carta
                        select_and_discard_card(card_index, &target_player->magic_cards, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(target_player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                case ALL:
                    num_cards_classroom = count_cards(target_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore target
                    num_cards_magic = count_cards(target_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore target

                    // SE il giocatore target non ha carte da eliminare (aula studio + bonus/malus)
                    if ((num_cards_classroom + num_cards_magic) == 0) {
                        printf("\n[" RED "!" RESET "] %s non ha carte da eliminare!\n", target_player->name);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta da eliminare a %s:\n\n", player->name, target_player->name);
                        print_deck(target_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore target
                        printf("\n");
                        print_deck(target_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore target

                        // Chiede all'utente di scegliere una carta da eliminare (aula studio + bonus/malus) al giocatore target
                        card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                        // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                        if (card_index > num_cards_classroom) {
                            card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore target
                            deleted_card = select_card(card_index-1, &target_player->magic_cards, true); // Seleziona la carta scelta
                        } else {
                            // Altrimenti seleziona la carta nell'aula studio del giocatore target
                            deleted_card = select_card(card_index-1, &target_player->classroom, true); // Seleziona la carta scelta
                        }

                        printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, target_player->name);
                        print_card(deleted_card); // Stampa la carta eliminata

                        // Registra l'azione di eliminare la carta nel file di log
                        log_elimina_effect(player, target_player, deleted_card);

                        // Scarta la carta selezionata
                        discard_card(deleted_card, discard_deck);
                        // Controlla e attiva eventuali effetti della carta eliminata
                        check_effect(target_player, deleted_card, FINE, draw_deck, discard_deck);
                    }
                    break;

                default:
                    printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto ELIMINA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    break;
            }
            break;

        case VOI:
            current_player = current_player->next_player; // Salta il giocatore corrente (me stesso => IO => player)
            do {
                // In base al tipo di carta da eliminare
                switch (effect->target_card) {
                    case STUDENTE:
                    case MATRICOLA:
                    case STUDENTE_SEMPLICE:
                    case LAUREANDO:
                        // SE il giocatore corrente non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                        if (!deck_contains_type(current_player->classroom, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da eliminare al giocatore corrente
                                card_index = choice_card(current_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                deleted_card = select_card(card_index, &current_player->classroom, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                                if (effect->target_card != STUDENTE && deleted_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Seleziona e scarta la carta
                            select_and_discard_card(card_index, &current_player->classroom, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    case BONUS:
                    case MALUS:
                        // SE il giocatore corrente non ha carte bonus/malus
                        if (!deck_contains_type(current_player->magic_cards, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo BONUS o MALUS da eliminare al giocatore corrente
                                card_index = choice_card(current_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                deleted_card = select_card(card_index, &current_player->magic_cards, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è del tipo richiesto
                                if (deleted_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Seleziona e scarta la carta
                            select_and_discard_card(card_index, &current_player->magic_cards, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    case ALL:
                        num_cards_classroom = count_cards(current_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore corrente
                        num_cards_magic = count_cards(current_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore corrente

                        // SE il giocatore corrente non ha carte da eliminare (aula studio + bonus/malus)
                        if ((num_cards_classroom + num_cards_magic) == 0) {
                            printf("\n[" RED "!" RESET "] %s non ha carte da eliminare!\n", current_player->name);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente
                            printf("\n");
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            // Chiede all'utente di scegliere una carta da eliminare (aula studio + bonus/malus) al giocatore corrente
                            card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                            // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                            if (card_index > num_cards_classroom) {
                                card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore corrente
                                deleted_card = select_card(card_index-1, &current_player->magic_cards, true); // Seleziona la carta scelta
                            } else {
                                // Altrimenti seleziona la carta nell'aula studio del giocatore corrente
                                deleted_card = select_card(card_index-1, &current_player->classroom, true); // Seleziona la carta scelta
                            }

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Scarta la carta selezionata
                            discard_card(deleted_card, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    default:
                        printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto ELIMINA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        break;
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;

        case TUTTI:
            do {
                // In base al tipo di carta da eliminare
                switch (effect->target_card) {
                    case STUDENTE:
                    case MATRICOLA:
                    case STUDENTE_SEMPLICE:
                    case LAUREANDO:
                        // SE il giocatore corrente non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                        if (!deck_contains_type(current_player->classroom, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da eliminare al giocatore corrente
                                card_index = choice_card(current_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                deleted_card = select_card(card_index, &current_player->classroom, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                                if (effect->target_card != STUDENTE && deleted_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Seleziona e scarta la carta
                            select_and_discard_card(card_index, &current_player->classroom, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    case BONUS:
                    case MALUS:
                        // SE il giocatore corrente non ha carte bonus/malus
                        if (!deck_contains_type(current_player->magic_cards, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da eliminare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo BONUS o MALUS da eliminare al giocatore corrente
                                card_index = choice_card(current_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                deleted_card = select_card(card_index, &current_player->magic_cards, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è del tipo richiesto
                                if (deleted_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Seleziona e scarta la carta
                            select_and_discard_card(card_index, &current_player->magic_cards, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    case ALL:
                        num_cards_classroom = count_cards(current_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore corrente
                        num_cards_magic = count_cards(current_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore corrente

                        // SE il giocatore corrente non ha carte da eliminare (aula studio + bonus/malus)
                        if ((num_cards_classroom + num_cards_magic) == 0) {
                            printf("\n[" RED "!" RESET "] %s non ha carte da eliminare!\n", current_player->name);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da eliminare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente
                            printf("\n");
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            // Chiede all'utente di scegliere una carta da eliminare (aula studio + bonus/malus) al giocatore corrente
                            card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da eliminare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                            // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                            if (card_index > num_cards_classroom) {
                                card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore corrente
                                deleted_card = select_card(card_index-1, &current_player->magic_cards, true); // Seleziona la carta scelta
                            } else {
                                // Altrimenti seleziona la carta nell'aula studio del giocatore corrente
                                deleted_card = select_card(card_index-1, &current_player->classroom, true); // Seleziona la carta scelta
                            }

                            printf("\n[" RED "-" RESET "] %s hai eliminato una carta a %s!\n", player->name, current_player->name);
                            print_card(deleted_card); // Stampa la carta eliminata

                            // Registra l'azione di eliminare la carta nel file di log
                            log_elimina_effect(player, current_player, deleted_card);

                            // Scarta la carta selezionata
                            discard_card(deleted_card, discard_deck);
                            // Controlla e attiva eventuali effetti della carta eliminata
                            check_effect(current_player, deleted_card, FINE, draw_deck, discard_deck);
                        }
                        break;

                    default:
                        printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto ELIMINA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        break;
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non torna al giocatore iniziale
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di rubare una carta dall'aula studio o bonus/malus.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void ruba_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta
    int num_cards_classroom = 0, num_cards_magic = 0, card_index = 0; // Inizializza il numero di carte presenti nell'aula studio, nel mazzo bonus/malus e l'indice della carta scelta dall'utente a 0
    Card* stealed_card = NULL; // Inizializza la carta rubata a NULL
    bool is_valid_choice = false; // Inizializza la validità della scelta dell'utente a false

    // Valori ammessi da effect->target_card per l'effetto RUBA:
    // STUDENTE, MATRICOLA, STUDENTE_SEMPLICE, LAUREANDO
    // BONUS, MALUS
    // ALL (qualsiasi carta presente nell'aula studio o bonus/malus)

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // In base al tipo di carta da rubare
            switch (effect->target_card) {
                case STUDENTE:
                case MATRICOLA:
                case STUDENTE_SEMPLICE:
                case LAUREANDO:
                    // SE il giocatore non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                    if (!deck_contains_type(player->classroom, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non hai carte di tipo %s%s%s da rubare!\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        print_deck(player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore

                        do {
                            // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da rubare al giocatore
                            card_index = choice_card(player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            stealed_card = select_card(card_index, &player->classroom, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                            if (effect->target_card != STUDENTE && stealed_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, player->name);
                        print_card(stealed_card); // Stampa la carta rubata

                        // Registra l'azione di rubare la carta nel file di log
                        log_ruba_effect(player, player, stealed_card);

                        // Seleziona e scollega la carta
                        stealed_card = select_card(card_index, &player->classroom, true);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, FINE, draw_deck, discard_deck);
                        // Aggiungi la carta rubata all'aula studio del giocatore
                        player->classroom = add_card(player->classroom, stealed_card);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                    }
                    break;

                case BONUS:
                case MALUS:
                    // SE il giocatore non ha carte bonus/malus
                    if (!deck_contains_type(player->magic_cards, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non hai carte di tipo %s%s%s da rubare!\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        print_deck(player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore

                        do {
                            // Chiede all'utente di scegliere una carta di tipo BONUS o MALUS da rubare al giocatore
                            card_index = choice_card(player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            stealed_card = select_card(card_index, &player->magic_cards, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è del tipo richiesto
                            if (stealed_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, player->name);
                        print_card(stealed_card); // Stampa la carta rubata

                        // Registra l'azione di rubare la carta nel file di log
                        log_ruba_effect(player, player, stealed_card);

                        // Seleziona e scollega la carta
                        stealed_card = select_card(card_index, &player->magic_cards, true);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, FINE, draw_deck, discard_deck);
                        // Aggiungi la carta rubata all'aula studio del giocatore
                        player->magic_cards = add_card(player->magic_cards, stealed_card);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                    }
                    break;

                case ALL:
                    num_cards_classroom = count_cards(player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore
                    num_cards_magic = count_cards(player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore

                    // SE il giocatore non ha carte da rubare (aula studio + bonus/malus)
                    if ((num_cards_classroom + num_cards_magic) == 0) {
                        printf("\n[" RED "!" RESET "] Non hai carte da rubare!\n");
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        print_deck(player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore
                        printf("\n");
                        print_deck(player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore

                        // Chiede all'utente di scegliere una carta da rubare (aula studio + bonus/malus) al giocatore
                        card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                        // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                        if (card_index > num_cards_classroom) {
                            card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore
                            stealed_card = select_card(card_index-1, &player->magic_cards, true); // Seleziona e scollega la carta

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, player, stealed_card);

                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->magic_cards = add_card(player->magic_cards, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        } else {
                            // Altrimenti seleziona la carta nell'aula studio del giocatore
                            stealed_card = select_card(card_index-1, &player->classroom, true); // Seleziona e scollega la carta

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, player, stealed_card);

                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->classroom = add_card(player->classroom, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                    }
                    break;

                default:
                    printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto RUBA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    break;
            }
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            // In base al tipo di carta da rubare
            switch (effect->target_card) {
                case STUDENTE:
                case MATRICOLA:
                case STUDENTE_SEMPLICE:
                case LAUREANDO:
                    // SE il giocatore target non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                    if (!deck_contains_type(target_player->classroom, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", target_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, target_player->name);
                        print_deck(target_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore target

                        do {
                            // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da rubare al giocatore target
                            card_index = choice_card(target_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            stealed_card = select_card(card_index, &target_player->classroom, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                            if (effect->target_card != STUDENTE && stealed_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, target_player->name);
                        print_card(stealed_card); // Stampa la carta rubata

                        // Registra l'azione di rubare la carta nel file di log
                        log_ruba_effect(player, target_player, stealed_card);

                        // Seleziona e scollega la carta
                        stealed_card = select_card(card_index, &target_player->classroom, true);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(target_player, stealed_card, FINE, draw_deck, discard_deck);
                        // Aggiungi la carta rubata all'aula studio del giocatore
                        player->classroom = add_card(player->classroom, stealed_card);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                    }
                    break;

                case BONUS:
                case MALUS:
                    // SE il giocatore target non ha carte bonus/malus
                    if (!deck_contains_type(target_player->magic_cards, effect->target_card)) {
                        printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", target_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, target_player->name);
                        print_deck(target_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore target

                        do {
                            card_index = choice_card(target_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                            stealed_card = select_card(card_index, &target_player->magic_cards, false); // Seleziona la carta scelta

                            // SE la carta selezionata non è del tipo richiesto
                            if (stealed_card->type != effect->target_card) {
                                printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                            } else {
                                is_valid_choice = true; // La scelta dell'utente è valida
                            }

                        } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                        printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, target_player->name);
                        print_card(stealed_card); // Stampa la carta rubata

                        // Registra l'azione di rubare la carta nel file di log
                        log_ruba_effect(player, target_player, stealed_card);

                        // Seleziona e scollega la carta
                        stealed_card = select_card(card_index, &target_player->magic_cards, true);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(target_player, stealed_card, FINE, draw_deck, discard_deck);
                        // Aggiungi la carta rubata all'aula studio del giocatore
                        player->magic_cards = add_card(player->magic_cards, stealed_card);
                        // Controlla e attiva eventuali effetti della carta rubata
                        check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                    }
                    break;

                case ALL:
                    num_cards_classroom = count_cards(target_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore target
                    num_cards_magic = count_cards(target_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore target 

                    // SE il giocatore target non ha carte da rubare (aula studio + bonus/malus)
                    if ((num_cards_classroom + num_cards_magic) == 0) {
                        printf("\n[" RED "!" RESET "] %s non ha carte da rubare!\n", target_player->name);
                    } else {
                        printf("\n[" HBLU "i" RESET "] %s scegli una carta da rubare a %s:\n\n", player->name, target_player->name);
                        print_deck(target_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore target
                        printf("\n");
                        print_deck(target_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore target

                        // Chiede all'utente di scegliere una carta da rubare (aula studio + bonus/malus) al giocatore target
                        card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                        // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                        if (card_index > num_cards_classroom) {
                            card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore target
                            stealed_card = select_card(card_index-1, &target_player->magic_cards, true); // Seleziona e scollega la carta

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, target_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, target_player, stealed_card);

                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(target_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->magic_cards = add_card(player->magic_cards, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        } else {
                            // Altrimenti seleziona la carta nell'aula studio del giocatore target
                            stealed_card = select_card(card_index-1, &target_player->classroom, true); // Seleziona e scollega la carta

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, target_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, target_player, stealed_card);

                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(target_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->classroom = add_card(player->classroom, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                    }
                    break;

                default:
                    printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto RUBA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                    break;
            }
            break;

        case VOI:
            current_player = current_player->next_player; // Passa al prossimo giocatore
            do {
                // In base al tipo di carta da rubare
                switch (effect->target_card) {
                    case STUDENTE:
                    case MATRICOLA:
                    case STUDENTE_SEMPLICE:
                    case LAUREANDO:
                        // SE il giocatore corrente non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                        if (!deck_contains_type(current_player->classroom, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da rubare al giocatore corrente
                                card_index = choice_card(current_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                stealed_card = select_card(card_index, &current_player->classroom, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                                if (effect->target_card != STUDENTE && stealed_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, current_player, stealed_card);

                            // Seleziona e scollega la carta
                            stealed_card = select_card(card_index, &current_player->classroom, true);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->classroom = add_card(player->classroom, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                        break;

                    case BONUS:
                    case MALUS:
                        // SE il giocatore corrente non ha carte bonus/malus
                        if (!deck_contains_type(current_player->magic_cards, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            do {
                                card_index = choice_card(current_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                stealed_card = select_card(card_index, &current_player->magic_cards, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è del tipo richiesto
                                if (stealed_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, current_player, stealed_card);

                            // Seleziona e scollega la carta
                            stealed_card = select_card(card_index, &current_player->magic_cards, true);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->magic_cards = add_card(player->magic_cards, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                        break;

                    case ALL:
                        num_cards_classroom = count_cards(current_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore corrente
                        num_cards_magic = count_cards(current_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore corrente

                        // SE il giocatore corrente non ha carte da rubare (aula studio + bonus/malus)
                        if ((num_cards_classroom + num_cards_magic) == 0) {
                            printf("\n[" RED "!" RESET "] %s non ha carte da rubare!\n", current_player->name);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta da rubare a %s:\n\n", player->name, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente
                            printf("\n");
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            // Chiede all'utente di scegliere una carta da rubare (aula studio + bonus/malus) al giocatore corrente
                            card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                            // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                            if (card_index > num_cards_classroom) {
                                card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore corrente
                                stealed_card = select_card(card_index-1, &current_player->magic_cards, true); // Seleziona e scollega la carta

                                printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                                print_card(stealed_card); // Stampa la carta rubata

                                // Registra l'azione di rubare la carta nel file di log
                                log_ruba_effect(player, current_player, stealed_card);

                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                                // Aggiungi la carta rubata all'aula studio del giocatore
                                player->magic_cards = add_card(player->magic_cards, stealed_card);
                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                            } else {
                                // Altrimenti seleziona la carta nell'aula studio del giocatore corrente
                                stealed_card = select_card(card_index-1, &current_player->classroom, true); // Seleziona e scollega la carta

                                printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                                print_card(stealed_card); // Stampa la carta rubata

                                // Registra l'azione di rubare la carta nel file di log
                                log_ruba_effect(player, current_player, stealed_card);

                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                                // Aggiungi la carta rubata all'aula studio del giocatore
                                player->classroom = add_card(player->classroom, stealed_card);
                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                            }
                        }
                        break;

                    default:
                        printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto RUBA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        break;
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non si torna al giocatore iniziale
            break;

        case TUTTI:
            do {
                // In base al tipo di carta da rubare
                switch (effect->target_card) {
                    case STUDENTE:
                    case MATRICOLA:
                    case STUDENTE_SEMPLICE:
                    case LAUREANDO:
                        // SE il giocatore corrente non ha carte di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO
                        if (!deck_contains_type(current_player->classroom, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente

                            do {
                                // Chiede all'utente di scegliere una carta di tipo STUDENTE, MATRICOLA, STUDENTE_SEMPLICE o LAUREANDO da rubare al giocatore corrente
                                card_index = choice_card(current_player->classroom, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                stealed_card = select_card(card_index, &current_player->classroom, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è di tipo STUDENTE o del tipo richiesto
                                if (effect->target_card != STUDENTE && stealed_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, current_player, stealed_card);

                            // Seleziona e scollega la carta
                            stealed_card = select_card(card_index, &current_player->classroom, true);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->classroom = add_card(player->classroom, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                        break;

                    case BONUS:
                    case MALUS:
                        // SE il giocatore corrente non ha carte bonus/malus
                        if (!deck_contains_type(current_player->magic_cards, effect->target_card)) {
                            printf("\n[" RED "!" RESET "] %s non ha carte di tipo %s%s%s da rubare!\n", current_player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta di tipo %s%s%s da rubare a %s:\n\n", player->name, get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET, current_player->name);
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", 0, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            do {
                                card_index = choice_card(current_player->magic_cards, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                                stealed_card = select_card(card_index, &current_player->magic_cards, false); // Seleziona la carta scelta

                                // SE la carta selezionata non è del tipo richiesto
                                if (stealed_card->type != effect->target_card) {
                                    printf("\n[" RED "!" RESET "] La carta selezionata non è di tipo %s%s%s! Riprova!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                                } else {
                                    is_valid_choice = true; // La scelta dell'utente è valida
                                }

                            } while (!is_valid_choice); // Continua finché la scelta dell'utente non è valida

                            printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                            print_card(stealed_card); // Stampa la carta rubata

                            // Registra l'azione di rubare la carta nel file di log
                            log_ruba_effect(player, current_player, stealed_card);

                            // Seleziona e scollega la carta
                            stealed_card = select_card(card_index, &current_player->magic_cards, true);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                            // Aggiungi la carta rubata all'aula studio del giocatore
                            player->magic_cards = add_card(player->magic_cards, stealed_card);
                            // Controlla e attiva eventuali effetti della carta rubata
                            check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                        }
                        break;

                    case ALL:
                        num_cards_classroom = count_cards(current_player->classroom); // Conta il numero di carte presenti nell'aula studio del giocatore corrente
                        num_cards_magic = count_cards(current_player->magic_cards);   // Conta il numero di carte presenti nel mazzo bonus/malus del giocatore corrente

                        // SE il giocatore corrente non ha carte da rubare (aula studio + bonus/malus)
                        if ((num_cards_classroom + num_cards_magic) == 0) {
                            printf("\n[" RED "!" RESET "] %s non ha carte da rubare!\n", current_player->name);
                        } else {
                            printf("\n[" HBLU "i" RESET "] %s scegli una carta da rubare a %s:\n\n", player->name, current_player->name);
                            print_deck(current_player->classroom, "Aula Studio", 0, false, CLASSROOM_DECK_COLOR); // Stampa le carte presenti nell'aula studio del giocatore corrente
                            printf("\n");
                            print_deck(current_player->magic_cards, "Carte Bonus/Malus", num_cards_classroom, false, MAGIC_DECK_COLOR); // Stampa le carte presenti nel mazzo bonus/malus del giocatore corrente

                            // Chiede all'utente di scegliere una carta da rubare (aula studio + bonus/malus) al giocatore corrente
                            card_index = choice_int((num_cards_classroom+num_cards_magic), 1, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da rubare:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");

                            // SE l'indice della carta scelta è maggiore del numero di carte presenti nell'aula studio
                            if (card_index > num_cards_classroom) {
                                card_index -= num_cards_classroom; // Calcola l'indice della carta nel mazzo bonus/malus del giocatore corrente
                                stealed_card = select_card(card_index-1, &current_player->magic_cards, true); // Seleziona e scollega la carta

                                printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                                print_card(stealed_card); // Stampa la carta rubata

                                // Registra l'azione di rubare la carta nel file di log
                                log_ruba_effect(player, current_player, stealed_card);

                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                                // Aggiungi la carta rubata all'aula studio del giocatore
                                player->magic_cards = add_card(player->magic_cards, stealed_card);
                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                            } else {
                                // Altrimenti seleziona la carta nell'aula studio del giocatore corrente
                                stealed_card = select_card(card_index-1, &current_player->classroom, true); // Seleziona e scollega la carta

                                printf("\n[" GRN "+" RESET "] %s hai rubato una carta da %s!\n", player->name, current_player->name);
                                print_card(stealed_card); // Stampa la carta rubata

                                // Registra l'azione di rubare la carta nel file di log
                                log_ruba_effect(player, current_player, stealed_card);

                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(current_player, stealed_card, FINE, draw_deck, discard_deck);
                                // Aggiungi la carta rubata all'aula studio del giocatore
                                player->classroom = add_card(player->classroom, stealed_card);
                                // Controlla e attiva eventuali effetti della carta rubata
                                check_effect(player, stealed_card, SUBITO, draw_deck, discard_deck);
                            }
                        }
                        break;

                    default:
                        printf("\n[" RED "!" RESET "] Tipo di carta \"%s%s%s\" non valido per l'effetto RUBA!\n", get_color_by_type(effect->target_card), get_type_card(effect->target_card), RESET);
                        break;
                }
                current_player = current_player->next_player;
            } while (current_player != player);
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di pescare una carta.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void pesca_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            draw_card(&player, draw_deck, discard_deck, false); // Pesca una carta
            printf("\n[" GRN "+" RESET "] %s hai pescato una carta!\n", player->name);
            print_card(
                select_card(count_cards(player->hand)-1, &player->hand, false) // Seleziona l'ultima carta pescata
            ); // Stampa la carta pescata
            break;
        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            draw_card(&target_player, draw_deck, discard_deck, false); // Pesca una carta
            printf("\n[" GRN "+" RESET "] %s hai pescato una carta!\n", target_player->name);
            print_card(
                select_card(count_cards(target_player->hand)-1, &target_player->hand, false) // Seleziona l'ultima carta pescata
            ); // Stampa la carta pescata
            break;

        case VOI:
            current_player = current_player->next_player; // Salta il giocatore corrente (me stesso => IO => player)
            do {
                draw_card(&current_player, draw_deck, discard_deck, false); // Pesca una carta
                printf("\n[" GRN "+" RESET "] %s hai pescato una carta!\n", current_player->name);
                print_card(
                    select_card(count_cards(current_player->hand)-1, &current_player->hand, false) // Seleziona l'ultima carta pescata
                ); // Stampa la carta pescata
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non si ritorna al giocatore iniziale

            break;

        case TUTTI:
            do {
                draw_card(&current_player, draw_deck, discard_deck, false); // Pesca una carta
                printf("\n[" GRN "+" RESET "] %s hai pescato una carta!\n", current_player->name);
                print_card(
                    select_card(count_cards(current_player->hand)-1, &current_player->hand, false) // Seleziona l'ultima carta pescata
                ); // Stampa la carta pescata
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non si ritorna al giocatore iniziale
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di prendere una carta dalla mano di un giocatore.
 * 
 * @param player 
 * @param target_player 
 * @param card 
 * @param effect 
 * @param draw_deck 
 * @param discard_deck 
 */
void prendi_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta
    int card_index = 0; // Indice della carta da prendere
    bool show_hand = false; // Mostra la mano del giocatore target dell'effetto
    Card* selected_card = NULL; // Puntatore alla carta selezionata

    // Si assume che effect->target_card sia sempre ALL
    // in quanto, solitamente, non dovresti poter vedere la mano degli altri giocatori
    // (a meno che non ci siano effetti malus che lo permettano) e quindi non poter vedere
    // e scegliere il tipo richiesto di carta da prendere

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // SE non ci sono carte in mano
            if (count_cards(player->hand) == 0) {
                printf("\n[" RED "!" RESET "] %s non hai carte in mano da prendere!\n", player->name);
            } else {
                printf("\n[" HBLU "i" RESET "] %s scegli una carta da prendere dalla tua mano:\n\n", player->name);
                print_deck(player->hand, "Carte in Mano", 0, false, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore

                // Chiedere all'utente l'indice della carta da prendere dalla mano
                card_index = choice_card(player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da prendere:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                selected_card = select_card(card_index, &player->hand, true); // Seleziona e scollega la carta dalla mano del giocatore
                player->hand = add_card(player->hand, selected_card); // Aggiungi la carta presa alla mano del giocatore

                printf("\n[" GRN "+" RESET "] %s hai preso una carta da %s!\n", player->name, player->name);
                print_card(selected_card); // Stampa la carta presa

                log_prendi_effect(player, player, selected_card); // Registra l'effetto di prendere una carta dalla mano di un giocatore
            }
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            // SE non ci sono carte in mano
            if (count_cards(target_player->hand) == 0) {
                printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da prendere!\n", target_player->name);
            } else {
                printf("\n[" HBLU "i" RESET "] %s scegli una carta da prendere dalla mano di %s:\n\n", player->name, target_player->name);

                // Controlla se il giocatore può vedere la mano del giocatore target (effetti MOSTRA)
                show_hand = deck_contains_effect(target_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(target_player->classroom, MOSTRA, IO, ALL);
                print_deck(target_player->hand, "Carte in Mano", 0, !show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore target

                // Chiedere all'utente l'indice della carta da prendere dalla mano del giocatore target
                card_index = choice_card(target_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da prendere:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                selected_card = select_card(card_index, &target_player->hand, true); // Seleziona e scollega la carta dalla mano del giocatore target
                player->hand = add_card(player->hand, selected_card); // Aggiungi la carta presa alla mano del giocatore

                printf("\n[" GRN "+" RESET "] %s hai preso una carta da %s!\n", player->name, target_player->name);
                print_card(selected_card); // Stampa la carta presa

                log_prendi_effect(player, target_player, selected_card); // Registra l'effetto di prendere una carta dalla mano di un giocatore
            }
            break;

        case VOI:
            current_player = current_player->next_player; // Salta il giocatore corrente (me stesso => IO => player)
            do {
                // SE non ci sono carte in mano
                if (count_cards(current_player->hand) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da prendere!\n", current_player->name);
                } else {
                    printf("\n[" HBLU "i" RESET "] %s scegli una carta da prendere dalla mano di %s:\n\n", player->name, current_player->name);

                    // Controlla se il giocatore può vedere la mano del giocatore corrente (effetti MOSTRA)
                    show_hand = deck_contains_effect(current_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(current_player->classroom, MOSTRA, IO, ALL);
                    print_deck(current_player->hand, "Carte in Mano", 0, !show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore corrente

                    // Chiedere all'utente l'indice della carta da prendere dalla mano del giocatore corrente
                    card_index = choice_card(current_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da prendere:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                    selected_card = select_card(card_index, &current_player->hand, true); // Seleziona e scollega la carta dalla mano del giocatore corrente
                    player->hand = add_card(player->hand, selected_card); // Aggiungi la carta presa alla mano del giocatore

                    printf("\n[" GRN "+" RESET "] %s hai preso una carta da %s!\n", player->name, current_player->name);
                    print_card(selected_card); // Stampa la carta presa

                    log_prendi_effect(player, current_player, selected_card); // Registra l'effetto di prendere una carta dalla mano di un giocatore
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non si ritorna al giocatore iniziale
            break;

        case TUTTI:
            do {
                // SE non ci sono carte in mano
                if (count_cards(current_player->hand) == 0) {
                    printf("\n[" RED "!" RESET "] Il giocatore %s non ha carte in mano da prendere!\n", current_player->name);
                } else {
                    printf("\n[" HBLU "i" RESET "] %s scegli una carta da prendere dalla mano di %s:\n\n", player->name, current_player->name);

                    // Controlla se il giocatore può vedere la mano del giocatore corrente (effetti MOSTRA) o se è il giocatore corrente
                    show_hand = (current_player == player) || deck_contains_effect(current_player->magic_cards, MOSTRA, IO, ALL) || deck_contains_effect(current_player->classroom, MOSTRA, IO, ALL);
                    print_deck(current_player->hand, "Carte in Mano", 0, !show_hand, HAND_DECK_COLOR); // Stampa le carte in mano del giocatore corrente

                    // Chiedere all'utente l'indice della carta da prendere dalla mano del giocatore corrente
                    card_index = choice_card(current_player->hand, "\n[" HBLU "i" RESET "] Inserisci l'indice della carta da prendere:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
                    selected_card = select_card(card_index, &current_player->hand, true); // Seleziona e scollega la carta dalla mano del giocatore corrente
                    player->hand = add_card(player->hand, selected_card); // Aggiungi la carta presa alla mano del giocatore

                    printf("\n[" GRN "+" RESET "] %s hai preso una carta da %s!\n", player->name, current_player->name);
                    print_card(selected_card); // Stampa la carta presa

                    log_prendi_effect(player, current_player, selected_card); // Registra l'effetto di prendere una carta dalla mano di un giocatore
                }
                current_player = current_player->next_player; // Passa al prossimo giocatore
            } while (current_player != player); // Continua finché non si ritorna al giocatore iniziale
            break;
    }
    return;
}

/**
 * @brief Applica l'effetto di scambiare le carte in mano tra due giocatori.
 * 
 * @param player Puntatore al giocatore.
 * @param target_player Puntatore al giocatore target dell'effetto.
 * @param card Puntatore alla carta.
 * @param effect Puntatore all'effetto.
 * @param draw_deck Puntatore al mazzo di pesca.
 * @param discard_deck Puntatore al mazzo degli scarti.
 */
void scambia_effect(Player* player, Player* target_player, Card* card, Effect* effect, Card** draw_deck, Card** discard_deck) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    Player* current_player = player; // Inizializza il giocatore corrente al giocatore proprietario della carta

    // In base al giocatore target dell'effetto
    switch (effect->target_player) {
        case IO:
            // Scambia le carte in mano tra me stesso e me stesso
            // Non ha senso, ma per completezza lo gestisco 
            printf("\n[" HBLU "i" RESET "] Carte in mano di %s scambiate con le carte in mano di %s!\n", player->name, player->name);
            printf("\n");
            print_deck(player->hand, "Vecchia Mano", 0, false, DISCARD_DECK_COLOR); // Stampa la vecchia mano
            printf("\n");
            print_deck(player->hand, "Nuova Mano", 0, false, HAND_DECK_COLOR); // Stampa la nuova mano

            log_scambia_effect(player, player); // Registra l'effetto di scambiare le carte in mano tra due giocatori
            break;

        case TU:
            if (target_player == NULL) {
                // Chiedere all'utente a quale giocatore applicare l'effetto
                // Non dovrebbe mai essere NULL, ma per sicurezza lo controllo
                target_player = choose_player(player, false, "\n[" HBLU "i" RESET "] Scegli un giocatore a cui applicare l'effetto:\n", "\n[" HBLU "i" RESET "] Inserisci l'indice del giocatore:\n> ", "[" RED "!" RESET "] Indice non valido! Riprova!\n");
            }

            printf("\n[" HBLU "i" RESET "] Carte in mano di %s scambiate con le carte in mano di %s!\n", player->name, target_player->name);

            // Scambia le carte in mano tra il giocatore e il giocatore bersaglio
            Card* tmp_cards = target_player->hand; // Salva le carte in mano del giocatore bersaglio in una variabile temporanea
            target_player->hand = player->hand; // Assegna le carte in mano del giocatore al giocatore bersaglio
            player->hand = tmp_cards; // Assegna le carte in mano salvate nella variabile temporanea al giocatore

            printf("\n");
            print_deck(target_player->hand, "Vecchia Mano", 0, false, DISCARD_DECK_COLOR); // Stampa la vecchia mano del giocatore bersaglio
            printf("\n");
            print_deck(player->hand, "Nuova Mano", 0, false, HAND_DECK_COLOR); // Stampa la nuova mano del giocatore

            log_scambia_effect(player, target_player); // Registra l'effetto di scambiare le carte in mano tra due giocatori
            break;

        // Configurazione non valida
        case VOI:
            printf("\n[" RED "!" RESET "] Target Giocatori \"VOI\" non valido per l'effetto SCAMBIA!\n");
            break;

        // Configurazione non valida
        case TUTTI:
            printf("\n[" RED "!" RESET "] Target Giocatori \"TUTTI\" non valido per l'effetto SCAMBIA!\n");
            break;
    }

    return;
}