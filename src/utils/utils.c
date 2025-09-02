#include "utils.h"

#include "../model/constants.h"
#include "../model/colors.h"
#include <assert.h>
#include <string.h>

/**
 * @brief Stampa un messaggio di debug con il nome della funzione.
 * 
 * @param func Nome della funzione.
 */
void dbg_func(const char* func) {
    #ifdef DEBUG
        printf("[DEBUG] Entrato nella funzione \"%s\".\n", func);
    #endif
    return;
}

/**
 * @brief Pulisce il buffer di input.
 */
void clear_buffer() {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug
    while (getchar() != '\n'); // Pulisce il buffer di input
    return;
}

/**
 * @brief Pulisce lo schermo della console.
 */
void clear_screen() {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    #ifdef _WIN32
        system("cls"); 
    #elif _WIN64
        system("cls");
    #elif __linux__
        system("clear");
    #elif __APPLE__
        system("clear");
    #endif

    return;
}

/**
 * @brief Stampa una stringa con effetto di scrittura manuale.
 * 
 * @param str Stringa da stampare.
 * @param speed Velocità di stampa in millisecondi.
 */
void slow_print(const char* str, unsigned int speed) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    for (int i = 0; str[i] != '\0'; i++) {
        printf("%c", str[i]); // Stampa un carattere
        fflush(stdout); // Forza la scrittura immediata di tutti i dati nel buffer su stdout
        usleep(speed * SPEED_COEFFICIENT); // Tempo di attesa in microsecondi (1 secondo = 1.000.000 microsecondi)
    }
    printf("\n");

    return;
}

/**
 * @brief Legge un numero intero da input.
 * 
 * @param num Puntatore alla variabile in cui salvare il numero letto.
 */
void read_int(int* num) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Continua a leggere finché l'input non è un intero
    while (scanf(" %d", num) != 1) { // Legge un intero
        getchar(); // Pulisce il buffer di input
    }
    clear_buffer(); // Pulisce il buffer di input

    return;
}

/**
 * @brief Richiede un input numerico all'utente entro un intervallo specificato.
 * 
 * @param max_range Valore massimo accettato.
 * @param min_range Valore minimo accettato.
 * @param msg Messaggio da visualizzare per l'input.
 * @param error_msg Messaggio di errore in caso di input non valido.
 * @return Il numero scelto dall'utente.
 */
int choice_int(int max_range, int min_range, char* msg, char* error_msg) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int choice_int; // Variabile per la scelta dell'utente
    do {
        printf(msg); // Stampa il messaggio per la scelta
        read_int(&choice_int); // Legge un intero

        // Controlla SE la scelta è valida
        if (choice_int < min_range || choice_int > max_range) {
            printf(error_msg); // Stampa il messaggio di errore
        }
    } while (choice_int < min_range || choice_int > max_range);

    return choice_int;
}

/**
 * @brief Controlla se un carattere è alfanumerico.
 * 
 * @param c Carattere da controllare.
 * @return true se il carattere è alfanumerico, false altrimenti.
 */
bool char_isalnum(const char c) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug
    // Controlla se il carattere è una lettera o un numero
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

/**
 * @brief Controlla se un carattere è presente in una stringa.
 * 
 * @param str Stringa da analizzare.
 * @param c Carattere da cercare.
 * @return true se il carattere è presente, false altrimenti.
 */
bool char_find(const char* str, const char c) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Scorre tutti i caratteri della stringa
    for (int i = 0; i < strlen(str); i++) {
        if (str[i] == c) { // SE il carattere è uguale a c
            return true;
        }
    }

    return false;
}

/**
 * @brief Conta il numero di occorrenze di un carattere in una stringa.
 * 
 * @param str Stringa da analizzare.
 * @param c Carattere da contare.
 * @return Numero di occorrenze del carattere.
 */
int count_char(const char* str, const char c) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int count = 0; // Contatore delle occorrenze

    for (int i = 0; i < strlen(str); i++) {
        // Se il carattere è uguale a c, incrementa il contatore
        if (str[i] == c) {
            count++;
        }
    }

    return count;
}

/**
 * @brief Legge un carattere tra quelli consentiti.
 * 
 * @param choices Stringa contenente i caratteri validi.
 * @param error_msg Messaggio di errore da visualizzare se l'input non è valido.
 * @return Il carattere letto.
 */
char read_char(const char* choices, const char* msg, const char* error_msg) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    char c; // Initializza la variabile per la scelta dell'utente
    bool valid = false; // Flag per la validità della scelta

    do {
        printf(msg); // Stampa il messaggio per la scelta
        scanf(" %c", &c); // Legge un carattere
        clear_buffer(); // Pulisce il buffer di input

        // Controlla se il carattere è tra quelli validi
        if (char_find(choices, c)) {
            valid = true;
        } else {
            printf("%s", error_msg); // Stampa il messaggio di errore
        }

    } while (!valid);

    return c;
}

/**
 * @brief Restituisce il valore minimo tra due interi.
 * 
 * @param a Primo intero.
 * @param b Secondo intero.
 * @return Il valore minimo.
 */
int min(const int a, const int b) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug
    return a < b ? a : b; // Restituisce il valore minimo
}

/**
 * @brief Restituisce il valore massimo tra due interi.
 * 
 * @param a Primo intero.
 * @param b Secondo intero.
 * @return Il valore massimo.
 */
int max(const int a, const int b) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug
    return a > b ? a : b; // Restituisce il valore massimo
}

/**
 * @brief Controlla se un nome di file è valido.
 * 
 * @param filename Nome del file da controllare.
 * @return true se il nome del file è valido, false altrimenti.
 */
bool is_valid_filename(const char* filename) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Controlla la lunghezza del nome del file
    if (strlen(filename) == 0 || strlen(filename) > MAX_FILENAME_LENGTH) {
        return false;
    }

    // Controlla che il nome del file contenga solo caratteri alfanumerici, '_' e '-'
    for (int i = 0; i < strlen(filename); i++) {
        if (!char_isalnum(filename[i]) && filename[i] != '_' && filename[i] != '-') {
            return false;
        }
    }

    return true;
}

/**
 * @brief Controlla se un file esiste.
 * 
 * @param path Percorso del file.
 * @return true se il file esiste, false altrimenti.
 */
bool file_exists(const char* path) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE *file = fopen(path, "r"); // Apre il file in modalità lettura
    if (file) {
        fclose(file); // Chiude il file
        return true;
    }

    return false;
}

/**
 * @brief Apre un file in modo sicuro.
 * 
 * @param filename Nome del file da aprire.
 * @param mode Modalità di apertura del file.
 * @return Puntatore al file aperto.
 */
FILE* safe_fopen(const char* filename, const char* mode) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    FILE* file = fopen(filename, mode); // Apre il file

    // Controlla SE il file è stato aperto correttamente
    if (file == NULL) {
        printf(RED "[!] Errore nell'apertura del file \"%s\"!" RESET, filename);
        exit(EXIT_FAILURE);
    }

    return file;
}

/**
 * @brief Legge in modo sicuro da un file.
 * 
 * @param ptr Puntatore al buffer di destinazione.
 * @param element_size Dimensione di un elemento.
 * @param count Numero di elementi da leggere.
 * @param stream Puntatore al file da cui leggere.
 */
void safe_fread(void* ptr, size_t element_size, size_t count, FILE* stream) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    size_t read = fread(ptr, element_size, count, stream); // Legge dal file

    // Controlla SE la lettura è andata a buon fine
    if (read != count) {
        printf(RED "[!] Errore nella lettura dell'input dal file!" RESET);
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * @brief Scrive in modo sicuro su un file.
 * 
 * @param ptr Puntatore ai dati da scrivere.
 * @param element_size Dimensione di un elemento.
 * @param count Numero di elementi da scrivere.
 * @param stream Puntatore al file su cui scrivere.
 */
void safe_fwrite(void* ptr, size_t element_size, size_t count, FILE* stream) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    size_t written = fwrite(ptr, element_size, count, stream); // Scrive sul file

    // Controlla SE la scrittura è andata a buon fine
    if (written != count) {
        printf(RED "[!] Errore nella scrittura dell'output sul file!" RESET);
        exit(EXIT_FAILURE);
    }

    return;
}

/**
 * @brief Alloca memoria in modo sicuro.
 * 
 * @param size Dimensione della memoria da allocare.
 * @return Puntatore alla memoria allocata.
 */
void* safe_malloc(size_t size) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    void* ptr = malloc(size); // Alloca memoria dinamicamente

    // Controlla SE la memoria è stata allocata correttamente
    if (ptr == NULL) {
        printf(RED "[!] Errore di allocazione di memoria dinamica!" RESET);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/**
 * @brief Alloca e azzera memoria in modo sicuro.
 * 
 * @param num Numero di elementi da allocare.
 * @param size Dimensione di ciascun elemento.
 * @return Puntatore alla memoria allocata.
 */
void* safe_calloc(size_t num, size_t size) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    void* ptr = calloc(num, size); // Alloca e azzera memoria dinamicamente

    // Controlla SE la memoria è stata allocata correttamente
    if (ptr == NULL) {
        printf(RED "[!] Errore di allocazione di memoria dinamica!" RESET);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

/**
 * @brief Rialloca memoria in modo sicuro.
 * 
 * @param ptr Puntatore alla memoria da riallocare.
 * @param size Nuova dimensione della memoria.
 * @return Puntatore alla nuova memoria allocata.
 */
void* safe_realloc(void* ptr, size_t size) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    void* new_ptr = realloc(ptr, size); // Rialloca memoria dinamicamente

    // Controlla SE la memoria è stata riallocata correttamente
    if (new_ptr == NULL) {
        printf(RED "[!] Errore di riallocazione di memoria dinamica!" RESET);
        exit(EXIT_FAILURE);
    }

    return new_ptr;
}

/**
 * @brief Rimuove i caratteri di nuova riga da una stringa.
 * 
 * @param str Stringa da sanificare.
 */
void sanitize_string(char* str) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int i, j = 0; // Offset iniziali
    int len = strlen(str); // Lunghezza della stringa

    for (i = 0; i < len; i++) {
        if (str[i] != '\r' && str[i] != '\n') {
            str[j++] = str[i];
        }
    }

    str[j] = '\0'; // Termina correttamente la stringa
    return;
}

/**
 * @brief Rimuove gli spazi iniziali e finali da una stringa.
 * 
 * @param str Stringa da elaborare.
 */
void strip_string(char* str) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    // Offset iniziali
    int left_index = 0;
    int right_index = strlen(str) - 1;

    // Aggiorna la lunghezza dell'offset di sinistra della stringa
    while (left_index <= right_index && str[left_index] == ' ') {
        left_index++;
    }

    if (left_index > 0) {
        // Sposta la stringa all'inizio del buffer
        for (int i = 0; i < strlen(str); i++) {
            str[i] = str[i + left_index];
        }
    }

    // Aggiorna la lunghezza dell'offset di destra della stringa
    while (right_index >= 0 && str[right_index] == ' ') {
        right_index--;
    }

    str[right_index + 1] = '\0'; // Termina correttamente la stringa
    return;
}

/**
 * @brief Confronta due stringhe ignorando la differenza tra maiuscole e minuscole.
 *
 * @param s1 La prima stringa da confrontare.
 * @param s2 La seconda stringa da confrontare.
 * @return true Se le stringhe sono uguali, false altrimenti.
 */
bool strcmp_insensitive(const char* s1, const char* s2) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    if (strlen(s1) != strlen(s2)) {
        return false;
    }

    // Inizializza le variabili temporanee
    char c1, c2;
    int i = 0;
    bool different = false;

    // Continua finché non si raggiunge la fine di una delle due stringhe
    while (!different && s1[i] != '\0' && s2[i] != '\0') {
        // Se i caratteri sono lettere maiuscole, trasformali in minuscole
        c1 = (s1[i] >= 'A' && s1[i] <= 'Z') ? s1[i] + ('a' - 'A') : s1[i];
        c2 = (s2[i] >= 'A' && s2[i] <= 'Z') ? s2[i] + ('a' - 'A') : s2[i];

        // Se i caratteri sono diversi esci dal ciclo
        if (c1 != c2) {
            different = true;
        } else {
            i++;
        }
    }

    // Restituisci la differenza tra i caratteri
    return !different;
}

/**
 * @brief Divide una stringa in più righe di lunghezza massima specificata.
 * 
 * @param text Testo da spezzare.
 * @param max_width Lunghezza massima di ogni riga.
 * @param num_lines Puntatore per restituire il numero di righe generate.
 * @return Array di stringhe contenenti il testo wrappato.
 */
char** wrap_text(const char* text, const int max_width, int* num_lines) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int len = strlen(text); // Lunghezza del testo
    int lines = 0; // Numero di righe generate

    char** wrapped_text = NULL;

    int start = 0;
    while (start < len) {
        int end = start + max_width; // Calcola la fine del segmento

        if (end >= len) {
            end = len; // Imposta la fine del segmento alla fine del testo
        } else {
            // Cerca l'ultimo spazio all'interno del segmento
            while (end > start && text[end] != ' ') {
                end--;
            }

            if (end == start) {
                end = start + max_width;
            }
        }

        // Calcola la lunghezza della riga e alloca la memoria
        int line_length = end - start;
        char* line = (char*) safe_malloc((max_width + 1) * sizeof(char));

        // Copia la riga nel buffer
        strncpy(line, text + start, line_length);

        // Aggiunge spazi alla fine della riga
        for (int i = line_length; i < max_width; i++) {
            line[i] = ' ';
        }

        // Termina correttamente la stringa
        line[max_width] = '\0';

        // Aggiunge la nuova riga al puntatore di righe
        wrapped_text = (char**) safe_realloc(wrapped_text, (lines + 1) * sizeof(char*));
        wrapped_text[lines++] = line;

        // Aggiorna l'inizio del prossimo segmento
        start = end;

        // Salta spazi consecutivi all'inizio del prossimo segmento
        while (start < len && text[start] == ' ') {
            start++;
        };
    }

    *num_lines = lines;
    return wrapped_text;
}

/**
 * @brief Ripete un carattere per un numero specificato di volte.
 * 
 * @param c Carattere da ripetere.
 * @param times Numero di ripetizioni.
 * @return Stringa contenente il carattere ripetuto.
 */
char* repeat_char(const char c, const int times) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    char* repeated_str = (char*) safe_malloc((times + 1) * sizeof(char)); // Alloca la memoria dinamicamente per la nuova stringa
    repeated_str[times] = '\0'; // Termina correttamente la stringa

    // Assegna il carattere
    for (int i = 0; i < times; i++) {
        repeated_str[i] = c;
    }

    return repeated_str;
}

/**
 * @brief Ripete una stringa per un numero specificato di volte.
 * 
 * @param str Stringa da ripetere.
 * @param times Numero di ripetizioni.
 * @return Nuova stringa contenente la ripetizione della stringa originale.
 */
char* repeat_string(const char* str, const int times) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int len = strlen(str); // Lunghezza della stringa
    char* repeated_str = (char*) safe_malloc((len * times + 1) * sizeof(char)); // Alloca la memoria dinamicamente per la nuova stringa
    repeated_str[0] = '\0'; // Inizializza la stringa

    // Concatena la stringa
    for (int i = 0; i < times; i++) {
        strcat(repeated_str, str);
    }

    return repeated_str;
}

/**
 * @brief Calcola il padding necessario per centrare una stringa.
 * 
 * @param max_width Lunghezza massima disponibile.
 * @param len Lunghezza della stringa.
 * @return Numero di spazi di padding necessari.
 */
int calculate_padding(const int max_width, const int len) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug
    return (max_width - len) / 2;
}

/**
 * @brief Aggiunge padding a una stringa per centrarla entro una lunghezza specificata.
 * 
 * @param str Stringa da formattare.
 * @param width Lunghezza desiderata della stringa.
 * @return Nuova stringa con padding aggiunto.
 */
char* padding_string(const char* str, const int width) {
    dbg_func(__func__); // Stampa il nome della funzione in modalità debug

    int len = strlen(str); // Lunghezza della stringa
    assert (len <= width && "Stringa più lunga della larghezza specificata!");

    // Alloca la nuova stringa
    char* padded_str = (char*) safe_malloc((width + 1) * sizeof(char));
    padded_str[0] = '\0'; // Inizializza la stringa

    // Calcola la lunghezza del padding a sinistra
    int left_padding_len = calculate_padding(width, len);
    char* left_padding = repeat_char(' ', left_padding_len);

    // Calcola la lunghezza del padding a destra
    int right_padding_len = width - len - left_padding_len;
    char* right_padding = repeat_char(' ', right_padding_len);

    // Concatena le stringhe
    strcat(padded_str, left_padding);
    strcat(padded_str, str);
    strcat(padded_str, right_padding);

    // Libera la memoria allocata dinamicamente
    free(left_padding);
    free(right_padding);

    return padded_str;
}