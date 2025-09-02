#include "../save_load/save_load.h"
#include "../utils/utils.h"
#include "game.h"
#include <time.h>
#include <string.h>

int main(int argc, char* argv[]) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Pulizia dello schermo all'avvio del gioco
    clear_screen();

    // Inizializzazione del generatore di numeri casuali
    srand(time(NULL));

    // Stampa del banner del gioco
    printf(GAME_BANNER);

    // Caricamento dei salvataggi registrati
    int num_registered_saves = 0;
    char** registered_saves = load_registered_saves(DEFAULT_SAVES_LIST_PATH, &num_registered_saves);

    // Buffer per il nome del file di salvataggio
    char filename[MAX_FILENAME_LENGTH + 1];
    char save_path[MAX_FILENAME_LENGTH + SAVES_FOLDER_LEN + SAVES_EXTENSION_LEN + 1];

    // SE è stato passato un argomento da riga di comando
    if (argc >= 2) {
        // Sanitizzazione del nome del file di salvataggio
        sanitize_string(argv[1]);
        strip_string(argv[1]);

        // Controllo SE il nome del file di salvataggio è valido
        if (!is_valid_filename(argv[1])) {
            printf("[" RED "!" RESET "] Nome del file di salvataggio non valido!\n");
            printf("[" HBLU "i" RESET "] Assicurati di avere il file di salvataggio nella cartella \"" GRN "saves/" RESET "\" e che il nome del file contenga solo caratteri alfanumeri (a-z, A-Z, 0-9) o trattini!\n");
        } else {
            // Costruzione del percorso del file di salvataggio
            sprintf(save_path, "%s%s%s", SAVES_FOLDER, argv[1], SAVES_EXTENSION);

            // SE il file di salvataggio esiste carica la partita salvata
            if (file_exists(save_path)) {
                load_saved_game(save_path, argv[1]);
                // Registra il salvataggio nella lista dei salvataggi registrati
                registered_saves = register_new_save(DEFAULT_SAVES_LIST_PATH, argv[1], registered_saves, &num_registered_saves);
            } else {
                printf("[" RED "-" RESET "] Il file di salvataggio \"" GRN "%s" RESET "\" non è stato trovato!\n", save_path);
                printf("[" HBLU "i" RESET "] Assicurati di avere il file di salvataggio nella cartella \"" GRN "saves/" RESET "\" e che il nome del file contenga solo caratteri alfanumeri (a-z, A-Z, 0-9) o trattini!\n");
            }
        }
    }

    // Inizializzazione delle variabili per la scelta dell'utente
    int user_choice, user_choice_save;
    char loading_save_choice;
    bool valid_filename, valid_index_saves;

    do {
        user_choice = 0;           // Reset della scelta dell'utente
        user_choice_save = 0;      // Reset della scelta del salvataggio
        loading_save_choice = 'n'; // Reset della scelta di caricamento del salvataggio
        valid_filename = false;    // Reset del flag di validità del nome del file
        valid_index_saves = false; // Reset del flag di validità dell'indice dei salvataggi

        printf(GAME_MENU); // Stampa del menù principale del gioco
        read_int(&user_choice); // Lettura della scelta dell'utente
        printf("\n");

        switch (user_choice) {
            case NEW_GAME_ACTION:
                init_new_game(&registered_saves, &num_registered_saves); // Inizializzazione di una nuova partita
                break;

            case LOAD_GAME_ACTION:
                printf("[" RED "!" RESET "] Assicurati di avere il file di salvataggio nella cartella \"" GRN "saves/" RESET "\" e che il nome del file contenga solo caratteri alfanumeri (a-z, A-Z, 0-9) o trattini!\n");

                // SE ci sono salvataggi registrati
                if (num_registered_saves > 0) {
                    printf("\n[" HBLU "i" RESET "] Cronologia dei salvataggi registrati:\n");
                    for (int i = 0; i < num_registered_saves; i++) {
                        // Costruzione del percorso del file di salvataggio
                        sprintf(save_path, "%s%s%s", SAVES_FOLDER, registered_saves[i], SAVES_EXTENSION);
                        printf("  %d. " HYEL "%s" RESET "\n", i + 1, save_path); // Stampa del salvataggio registrato
                    }

                    // Chiede all'utente se vuole caricare uno dei salvataggi registrati
                    loading_save_choice = read_char("sSnN", "\n[" HBLU "i" RESET "] Vuoi caricare uno dei salvataggi registrati? (" GRN "s" RESET "/" RED "n" RESET "):\n> ", "[" RED "!" RESET "] Scelta non valida! Riprova!\n");
                }

                // SE l'utente vuole caricare uno dei salvataggi registrati
                if (loading_save_choice == 's' || loading_save_choice == 'S') {
                    do {
                        printf("\n[" HBLU "i" RESET "] Inserisci il numero del salvataggio da caricare:\n");
                        printf("> ");
                        read_int(&user_choice_save);

                        // Controllo SE il numero del salvataggio è valido
                        if (user_choice_save < 1 || user_choice_save > num_registered_saves) {
                            printf("[" RED "!" RESET "] Numero del salvataggio non valido. Riprova!\n");
                        } else {
                            strncpy(filename, registered_saves[user_choice_save - 1], MAX_FILENAME_LENGTH);
                            valid_index_saves = true;
                        }
                    } while (!valid_index_saves);
                } else {
                    do {
                        printf("\n[" HBLU "i" RESET "] Inserisci il nome del file di salvataggio (senza l'estensione " RED ".sav" RESET ") da caricare:\n");
                        printf("> ");
                        scanf(" %" STR_MAX_FILENAME_LENGTH "[^\n]s", filename);
                        clear_buffer();

                        // Sanitizzazione del nome del file di salvataggio
                        sanitize_string(filename);
                        strip_string(filename);

                        // Controllo SE il nome del file di salvataggio è valido
                        if (!is_valid_filename(filename)) {
                            printf("[" RED "!" RESET "] Nome del file non valido. Riprova!\n");
                        } else {
                            valid_filename = true;
                        }

                    } while (!valid_filename);
                }

                // Costruzione del percorso del file di salvataggio
                sprintf(save_path, "%s%s%s", SAVES_FOLDER, filename, SAVES_EXTENSION);

                // SE il file di salvataggio esiste carica la partita salvata
                if (file_exists(save_path)) {
                    printf("\n");
                    load_saved_game(save_path, filename);
                    // Registra il salvataggio nella lista dei salvataggi registrati
                    registered_saves = register_new_save(DEFAULT_SAVES_LIST_PATH, filename, registered_saves, &num_registered_saves);
                } else {
                    printf("[" RED "-" RESET "] Il file di salvataggio \"" GRN "%s" RESET "\" non è stato trovato!\n", save_path);
                }
                break;

            case EXIT_GAME_ACTION:
                printf("[" RED "-" RESET "] Uscita dal programma in corso...\n");
                break;

            default:
                printf("[" RED "!" RESET "] Scelta non valida! Riprova!\n");
                break;
        }

    } while (user_choice != EXIT_GAME_ACTION);

    log_prefix_round(false, true); // Reset del contatore dei round

    // Deallocazione della memoria dinamica per i salvataggi registrati
    free_registered_saves(registered_saves, num_registered_saves);
    return 0;
}