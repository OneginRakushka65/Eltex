#pragma once
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define MAX_STRING 50
#define MAX_PHONE_LEN 12
#define MAX_COUNT 20
#define MAX_COMMAND 256

#define MAX_NUMBER_COUNT 5
#define MAX_EMAIL_COUNT 5

#define BYTES_FOR_BITS(x) ((x) % 8 == 0 ? x / 8 : x / 8 + 1)

#define OVERFLOW -1
#define UNKNOWN_EXPRESSION -2
#define OVERFLOW_NUMBERS -3
#define OVERFLOW_EMAILS -4
#define NO_ENTRY -5

typedef struct {
	char tg[MAX_STRING];
	char vk[MAX_STRING];
	char inst[MAX_STRING];
} Link;

typedef struct {
	int ident;
	char first_name[MAX_STRING];
	char second_name[MAX_STRING];
	char middle_name[MAX_STRING];
	char job[MAX_STRING];
	char position[MAX_STRING];
	char number[MAX_NUMBER_COUNT][12];
	int phone_count;
	char email[MAX_EMAIL_COUNT][MAX_STRING];
	int email_count;
	Link profile;
} Contact;

typedef struct {
	int count;
	Contact contact[MAX_COUNT];
	uint8_t mask[BYTES_FOR_BITS(MAX_COUNT)];
} Book;

int set_ident(Book* book);
int get_ident(const Book* book, int ident);
int set_contact(Book* book, const char* first_name, const char* second_name, const char* args, ...);
Contact get_contact(const Book* book, const int ident);
int delete_contact(Book* book, const int ident);
int update_contact(Book* book, const int ident, const char* args, ...);
int delete_attribute(Book* book, const int ident, const char* args, ...);

void display_contact(const Contact* contact);
void display_all_contacts(const Book* book);

void load_book(Book* book, const char* filename);
void save_book(Book* book, const char* filename);