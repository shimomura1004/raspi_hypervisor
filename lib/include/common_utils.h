#ifndef _COMMON_UTILS_H
#define _COMMON_UTILS_H

#include <stdint.h>
#include <stddef.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

extern void put32(unsigned long, unsigned int);
extern unsigned int get32(unsigned long);
extern unsigned long get_el(void);

extern void *memzero_without_stack(void *, size_t);
extern void delay(unsigned long);

int abs(int);
char *strncpy(char *, const char *, size_t);
size_t strlen(const char *);
size_t strnlen(const char *, size_t);
int strcmp(const char *, const char *);
int strncmp(const char *, const char *, size_t);
char *strdup(const char *);
void *memset(void *, int, size_t);
void *memcpy(void *, const void *, size_t);
void *memzero(void *, size_t);
int memcmp(const void *, const void *, size_t);
void *memmove(void *, const void *, size_t);
void *memchr(const void *, int, size_t);
char *strchr(const char *, int);
char *strcpy(char *, const char *);
char *strncat(char *, const char *, size_t);
char *strcat(char *, const char *);
int isdigit(int);
int isspace(int);
int toupper(int);
int tolower(int);
int atoi(char *s);

#endif /* _COMMON_UTILS_H */