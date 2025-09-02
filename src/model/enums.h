#ifndef UNSTABLE_STUDENTS_ENUMS_H
#define UNSTABLE_STUDENTS_ENUMS_H

typedef enum {
    ALL,
    STUDENTE,
    MATRICOLA,
    STUDENTE_SEMPLICE,
    LAUREANDO,
    BONUS,
    MALUS,
    MAGIA,
    ISTANTANEA
} Type_card;

typedef enum {
    GIOCA,
    SCARTA,
    ELIMINA,
    RUBA,
    PESCA,
    PRENDI,
    BLOCCA,
    SCAMBIA,
    MOSTRA,
    IMPEDIRE,
    INGEGNERE
} Action;

typedef enum {
    SUBITO,
    INIZIO,
    FINE,
    MAI,
    SEMPRE
} When;

typedef enum {
    IO,
    TU,
    VOI,
    TUTTI
} Type_Player;

#endif