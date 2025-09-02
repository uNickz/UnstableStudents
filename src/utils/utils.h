#ifndef UNSTABLE_STUDENTS_UTILS_H
#define UNSTABLE_STUDENTS_UTILS_H

// #define DEBUG 1 // Decommentare per abilitare la modalità debug

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

void dbg_func(const char* func);
void clear_buffer();
void clear_screen();
void slow_print(const char* str, unsigned int speed);
void read_int(int* num);
int choice_int(int max_range, int min_range, char* msg, char* error_msg);
bool char_isalnum(const char c);
bool char_find(const char* str, const char c);
int count_char(const char* str, const char c);
char read_char(const char* choices, const char* msg, const char* error_msg);
int min(const int a, const int b);
int max(const int a, const int b);
bool is_valid_filename(const char* filename);
bool file_exists(const char* path);
FILE* safe_fopen(const char* filename, const char* mode);
void safe_fread(void* ptr, size_t element_size, size_t count, FILE* stream);
void safe_fwrite(void* ptr, size_t element_size, size_t count, FILE* stream);
void* safe_malloc(size_t size);
void* safe_calloc(size_t num, size_t size);
void* safe_realloc(void* ptr, size_t size);
void sanitize_string(char* str);
void strip_string(char* str);
bool strcmp_insensitive(const char* s1, const char* s2);
char** wrap_text(const char* text, const int max_width, int* num_lines);
char* repeat_char(const char c, const int times);
char* repeat_string(const char* str, const int times);
int calculate_padding(const int max_width, const int len);
char* padding_string(const char* str, const int width);

#endif