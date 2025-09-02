#ifndef UNSTABLE_UNICORN_CONSTANTS_H
#define UNSTABLE_UNICORN_CONSTANTS_H

#include "colors.h"

#define PRINT_SPEED 25 // Velocità di stampa del testo espressa in millisecondi
#define SPEED_COEFFICIENT 1000 // Coefficiente di conversione da millisecondi a microsecondi

#define DEFAULT_DECK_PATH "mazzo.txt"                    // Percorso di default del mazzo di carte
#define DEFAULT_LOG_FILE_PATH "log.txt"                  // Percorso di default del file di log
#define DEFAULT_SAVES_LIST_PATH "./saves/saves_list.txt" // Percorso di default del file contenente la lista dei file di salvataggi

#define MAX_PLAYERS 4 // Numero massimo di giocatori
#define MIN_PLAYERS 2 // Numero minimo di giocatori

#define NUM_STUDENTS_TO_WIN 6 // Numero di studenti da "collezionare" per vincere

// Banner iniziale del gioco
#define GAME_BANNER YEL "\n  ██╗   ██╗███╗   ██╗███████╗████████╗ █████╗ ██████╗ ██╗     ███████╗    ███████╗████████╗██╗   ██╗██████╗ ███████╗███╗   ██╗████████╗███████╗\n" \
                          "  ██║   ██║████╗  ██║██╔════╝╚══██╔══╝██╔══██╗██╔══██╗██║     ██╔════╝    ██╔════╝╚══██╔══╝██║   ██║██╔══██╗██╔════╝████╗  ██║╚══██╔══╝██╔════╝\n" \
                          "  ██║   ██║██╔██╗ ██║███████╗   ██║   ███████║██████╔╝██║     █████╗      ███████╗   ██║   ██║   ██║██║  ██║█████╗  ██╔██╗ ██║   ██║   ███████╗\n" \
                          "  ██║   ██║██║╚██╗██║╚════██║   ██║   ██╔══██║██╔══██╗██║     ██╔══╝      ╚════██║   ██║   ██║   ██║██║  ██║██╔══╝  ██║╚██╗██║   ██║   ╚════██║\n" \
                          "  ╚██████╔╝██║ ╚████║███████║   ██║   ██║  ██║██████╔╝███████╗███████╗    ███████║   ██║   ╚██████╔╝██████╔╝███████╗██║ ╚████║   ██║   ███████║\n" \
                          "   ╚═════╝ ╚═╝  ╚═══╝╚══════╝   ╚═╝   ╚═╝  ╚═╝╚═════╝ ╚══════╝╚══════╝    ╚══════╝   ╚═╝    ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝\n" RESET \
                          "                                                           " BOLD "(Made with ❤️ by " HCYN "uNickz" RESET BOLD ")\n\n" RESET

// Primo pattern del retro di una carta nascosta
#define HIDDEN_CARD_FIRST_PATTERN YEL "*   " \
                                 HBLK "~   " \
                                  YEL "*   " \
                                 HBLK "~   " \
                                  YEL "*   " \
                                 HBLK "~   " \
                                  YEL "*" RESET

// Secondo pattern del retro di una carta nascosta
#define HIDDEN_CARD_SECOND_PATTERN HBLK "~   " \
                                    YEL "*   " \
                                   HBLK "~   " \
                                    YEL "*   " \
                                   HBLK "~   " \
                                    YEL "*   " \
                                   HBLK "~" RESET

// Menù principale del gioco
#define GAME_MENU "\n[" HBLU "i" RESET "] Scegli un'opzione per iniziare a giocare:\n" \
                  "  1. " GRN "Inizia" RESET " una nuova partita.\n" \
                  "  2. " HYEL "Carica" RESET " una partita salvata.\n" \
                  "  3. " RED "Esci" RESET " dal programma.\n" \
                  "> "

#define NEW_GAME_ACTION 1  // Azione di iniziare una nuova partita
#define LOAD_GAME_ACTION 2 // Azione di caricare una partita salvata
#define EXIT_GAME_ACTION 3 // Azione di uscire dal gioco

// Menù delle azioni di gioco
#define ACTION_MENU "\n[" HBLU "i" RESET "] Scegli un'azione da compiere:\n" \
                    "  1. " GRN "Gioca" RESET " una carta.\n" \
                    "  2. " CYN "Pesca" RESET " un'ulteriore carta.\n" \
                    "  3. " YEL "Mostra" RESET " la tua mano, l'aula studio e le carte bonus/malus.\n" \
                    "  4. " MAG "Visualizza" RESET " lo stato degli altri giocatori (numero carte in mano, l'aula studio e le carte bonus/malus).\n" \
                    "  5. " RED "Esci" RESET " dalla partita.\n" \
                    "> "

#define PLAY_ACTION 1        // Azione di giocare una carta
#define DRAW_ACTION 2        // Azione di pescare una carta
#define SHOW_ACTION 3        // Azione di mostrare il proprio stato
#define SHOW_OTHERS_ACTION 4 // Azione di mostrare lo stato degli altri giocatori
#define EXIT_ACTION 5        // Azione di uscire dalla partita

#define SAVES_FOLDER "./saves/" // Cartella di salvataggio
#define SAVES_EXTENSION ".sav"        // Estensione dei file di salvataggio

#define SAVES_FOLDER_LEN 8 // Lunghezza della cartella di salvataggio
#define SAVES_EXTENSION_LEN 4    // Lunghezza dell'estensione del file

#define MAX_FILENAME_LENGTH 100       // Lunghezza massima del nome di un file
#define STR_MAX_FILENAME_LENGTH "100" // Lunghezza massima del nome di un file in formato stringa

#define MAX_NAME_LENGTH 31       // Lunghezza massima del nome di un giocatore o di una carta
#define STR_MAX_NAME_LENGTH "31" // Lunghezza massima del nome di un giocatore o di una carta in formato stringa

#define MAX_DESCRIPTION_LENGTH 255       // Lunghezza massima della descrizione di una carta
#define STR_MAX_DESCRIPTION_LENGTH "255" // Lunghezza massima della descrizione di una carta in formato stringa

#define NUM_STARTING_CARDS 5           // Numero di carte iniziali per ogni giocatore
#define NUM_STARTING_MATRICOLA_CARDS 1 // Numero di carte matricola iniziali per ogni giocatore

#define INPUT_UNIT 1        // Unità di input per la lettura di un valore
#define INPUT_EFFECT_UNIT 3 // Unità di input per la lettura di un effetto

#define MAX_HAND_SIZE 5 // Numero massimo di carte in mano

#define ROUND_SEPARATOR_SIZE 117    // Dimensione del separatore tra i round
#define LOG_GAME_SEPARATOR_SIZE 50 // Dimensione del separatore tra le partite nel file di log

#define BASE_PLAYER_HEIGHT 7 // Altezza base di un giocatore (per la visualizzazione grafica su terminale)
#define PLAYER_PADDING 3     // Padding per la formattazione del giocatore (per la visualizzazione grafica su terminale)

// Deck Format Settings
#define DECK_BASE_HEIGHT 2  // Altezza base di un mazzo (per la visualizzazione grafica su terminale)
#define DECK_NAME_WIDTH 25  // Larghezza del nome del mazzo (per la visualizzazione grafica su terminale)
#define DECK_PADDING 2      // Padding per la formattazione del mazzo (per la visualizzazione grafica su terminale)
#define EMPTY_DECK_HEIGHT 5 // Altezza di un mazzo vuoto (per la visualizzazione grafica su terminale)
#define EMPTY_DECK_WIDTH 75 // Larghezza di un mazzo vuoto (per la visualizzazione grafica su terminale)
#define MAX_CARD_PER_ROW 5  // Numero massimo di carte per riga (per la visualizzazione grafica su terminale)

// Deck Colors
#define MAGIC_DECK_COLOR MAG     // Colore del mazzo delle carte magia
#define CLASSROOM_DECK_COLOR YEL // Colore del mazzo dell'aula studio
#define HAND_DECK_COLOR CYN      // Colore del mazzo delle carte in mano
#define DISCARD_DECK_COLOR BBLK  // Colore del mazzo degli scarti

// Card Format Settings
#define CARD_BASE_HEIGHT 7      // Altezza base di una carta (per la visualizzazione grafica su terminale)
#define CARD_HIDDEN_HEIGHT 3    // Altezza di una carta nascosta (per la visualizzazione grafica su terminale)
#define MAX_CARD_WIDTH 33       // Larghezza massima di una carta (per la visualizzazione grafica su terminale)
#define CARD_WIDTH 31           // Larghezza di una carta (per la visualizzazione grafica su terminale)
#define REAL_CARD_WIDTH 29      // Larghezza reale di una carta (per la visualizzazione grafica su terminale)
#define MAX_CARD_INDEX_LENGTH 7 // Lunghezza massima dell'indice di una carta (per la visualizzazione grafica su terminale)

// Card Unicode Borders
#define UNICODE_BORDER_UP_LEFT "┌"    // U+250C
#define UNICODE_BORDER_UP_RIGHT "┐"   // U+2510
#define UNICODE_BORDER_DOWN_LEFT "└"  // U+2514
#define UNICODE_BORDER_DOWN_RIGHT "┘" // U+2518
#define UNICODE_BORDER_HORIZONTAL "─" // U+2500
#define UNICODE_BORDER_VERTICAL "│"   // U+2502

#define COLOR_PADDING 15 // Padding per la formattazione del colore (per la visualizzazione grafica su terminale)

#define SHUFFLE_ROUNDS 1000 // Numero di round per mescolare il mazzo

#endif