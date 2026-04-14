#define _CRT_SECURE_NO_WARNINGS
#include "phonebook.h"

//ВЫДАТЬ ИДЕНТИФИКАТОР
int set_ident(Book* book) {
	for (int i = 0; i < BYTES_FOR_BITS(MAX_COUNT); i++) {
		for (int j = 0; j < 8; j++) {
			uint8_t temp_mask = 1 << j;
			if ((book->mask[i] & temp_mask) == 0) {
				book->mask[i] |= temp_mask;
				return i * 8 + j;
			}
		}
	}
	return OVERFLOW;
}

//ПОЛУЧИТЬ СТАТУС ИДЕНТИФИКАТОРА
int get_ident(const Book* book, const int ident) {
	return (book->mask[ident / 8] & (1 << (ident % 8))) != 0;
}

//ДОБАВИТЬ КОНТАКТ (INSERT)
int set_contact(Book* book, const char* first_name, const char* second_name, const char* args, ...) { //следует добавить возможность ввода сразу нескольких номеров и почт 
	int ident = set_ident(book);
	if (ident == -1) return OVERFLOW;

	va_list argument_list;
	va_start(argument_list, args);
	Contact* contact = &book->contact[ident];

	contact->ident = ident;
	strcpy(contact->first_name, first_name);
	strcpy(contact->second_name, second_name);

	while (*args != 0) {
		switch (*(args++)) {
		case 'm':
			strncpy(contact->middle_name, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->middle_name[MAX_STRING - 1] = '\0';
			break;
		case 'j':
			strncpy(contact->job, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->job[MAX_STRING - 1] = '\0';
			break;
		case 'p':
			strncpy(contact->position, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->position[MAX_STRING - 1] = '\0';
			break;
		case 'n':
			if (contact->phone_count >= MAX_NUMBER_COUNT) {
				va_end(argument_list);
				return OVERFLOW_NUMBERS;
			}
			strncpy(contact->number[contact->phone_count], va_arg(argument_list, char*), MAX_PHONE_LEN - 1);
			contact->number[contact->phone_count][MAX_PHONE_LEN - 1] = '\0';
			contact->phone_count++;
			break;
		case 'e':
			if (contact->email_count >= MAX_EMAIL_COUNT) {
				va_end(argument_list);
				return OVERFLOW_EMAILS;
			}
			strncpy(contact->email[contact->email_count], va_arg(argument_list, char*), MAX_STRING - 1);
			contact->email[contact->email_count][MAX_STRING - 1] = '\0';
			contact->email_count++;
			break;
		case 't':
			strncpy(contact->profile.tg, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.tg[MAX_STRING - 1] = '\0';
			break;
		case 'v':
			strncpy(contact->profile.vk, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.vk[MAX_STRING - 1] = '\0';
			break;
		case 'i':
			strncpy(contact->profile.inst, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.inst[MAX_STRING - 1] = '\0';
			break;
		default:
			va_end(argument_list);
			return UNKNOWN_EXPRESSION;
		}
	}

	va_end(argument_list);
	book->count++;

	return ident;
}

//ПОЛУЧИТЬ КОНТАКТ (SELECT)
Contact get_contact(const Book* book, const int ident) {
	return book->contact[ident];
}

//УДАЛИТЬ КОНТАКТ (DELETE)
int delete_contact(Book* book, const int ident) {
	if (get_ident(book, ident) == 0) return NO_ENTRY;
	book->mask[ident / 8] &= ~(1 << (ident % 8));
	Contact temp = { 0 };
	book->contact[ident] = temp;
	book->count--;
	return ident;
}

//ОБНОВИТЬ КОНТАКТ (UPDATE)
int update_contact(Book* book, const int ident, const char* args, ...) {
	if (get_ident(book, ident) == 0) return NO_ENTRY;

	va_list argument_list;
	va_start (argument_list, args);

	Contact* contact = &book->contact[ident];

	while (*args != 0) {
		switch (*(args++)) {
		case 'f':
			strncpy(contact->first_name, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->first_name[MAX_STRING - 1] = '\0';
			break;
		case 's':
			strncpy(contact->second_name, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->second_name[MAX_STRING - 1] = '\0';
			break;
		case 'm':
			strncpy(contact->middle_name, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->middle_name[MAX_STRING - 1] = '\0';
			break;
		case 'j':
			strncpy(contact->job, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->job[MAX_STRING - 1] = '\0';
			break;
		case 'p':
			strncpy(contact->position, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->position[MAX_STRING - 1] = '\0';
			break;
		case 'n':
			{
				int count = va_arg(argument_list, int);
				for (int i = 0; i < count; i++) {
					int idx = va_arg(argument_list, int);
					char* number = va_arg(argument_list, char*);

					if (idx < 0 || idx >= MAX_NUMBER_COUNT) continue;

					if (contact->number[idx][0] == '\0') contact->phone_count++;

					strncpy(contact->number[idx], number, MAX_PHONE_LEN - 1);
					contact->number[idx][MAX_PHONE_LEN - 1] = '\0';
				}
			}
			break;
		case 'e':
			{
				int count = va_arg(argument_list, int);
				for (int i = 0; i < count; i++) {
					int idx = va_arg(argument_list, int);
					char* email = va_arg(argument_list, char*);

					if (idx < 0 || idx >= MAX_EMAIL_COUNT) continue;

					if (contact->email[idx][0] == '\0') contact->email_count++;

					strncpy(contact->email[idx], email, MAX_STRING - 1);
					contact->email[idx][MAX_STRING - 1] = '\0';
				}
			}
			break;
		case 't':
			strncpy(contact->profile.tg, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.tg[MAX_STRING - 1] = '\0';
			break;
		case 'v':
			strncpy(contact->profile.vk, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.vk[MAX_STRING - 1] = '\0';
			break;
		case 'i':
			strncpy(contact->profile.inst, va_arg(argument_list, char*), MAX_STRING - 1);
			contact->profile.inst[MAX_STRING - 1] = '\0';
			break;
		default:
			va_end(argument_list);
			return UNKNOWN_EXPRESSION;
		}
	}

	va_end(argument_list);
	return ident;
}

//УДАЛИТЬ АРГУМЕНТ
int delete_attribute(Book* book, const int ident, const char* args, ...) {
	if (get_ident(book, ident) == 0) return NO_ENTRY;

	va_list argument_list;
	va_start(argument_list, args);

	Contact* contact = &book->contact[ident];

	while (*args != 0) {
		switch (*(args++)) {
		case 'm':
			memset(contact->middle_name, 0, MAX_STRING);
			break;
		case 'j':
			memset(contact->job, 0, MAX_STRING);
			break;
		case 'p':
			memset(contact->position, 0, MAX_STRING);
			break;
		case 'n':
		{
			int count = va_arg(argument_list, int);

			for (int i = 0; i < count; i++) {
				int idx = va_arg(argument_list, int);
				if (idx >= 0 && idx < MAX_NUMBER_COUNT) {
					contact->number[idx][0] = '\0';
				}
			}

			int write_ptr = 0;
			for (int read_ptr = 0; read_ptr < MAX_NUMBER_COUNT; read_ptr++) {
				if (contact->number[read_ptr][0] != '\0') {
					if (write_ptr != read_ptr) {
						memcpy(contact->number[write_ptr], contact->number[read_ptr], MAX_PHONE_LEN);
					}
					write_ptr++;
				}
			}

			contact->phone_count = write_ptr;
			for (int k = write_ptr; k < MAX_NUMBER_COUNT; k++) {
				memset(contact->number[k], 0, MAX_PHONE_LEN);
			}
		}
		break;
		case 'e':
		{
			int count = va_arg(argument_list, int);

			for (int i = 0; i < count; i++) {
				int idx = va_arg(argument_list, int);
				if (idx >= 0 && idx < MAX_EMAIL_COUNT) contact->email[idx][0] = '\0';
			}

			int write_ptr = 0;
			for (int read_ptr = 0; read_ptr < MAX_EMAIL_COUNT; read_ptr++) {
				if (contact->email[read_ptr][0] != '\0') {
					if (write_ptr != read_ptr) {
						memcpy(contact->email[write_ptr], contact->email[read_ptr], MAX_STRING);
					}
					write_ptr++;
				}
			}

			contact->email_count = write_ptr;
			for (int k = write_ptr; k < MAX_EMAIL_COUNT; k++) memset(contact->email[k], 0, MAX_STRING);
			break;
		}
		break;
		case 't':
			memset(contact->profile.tg, 0, MAX_STRING);
			break;
		case 'v':
			memset(contact->profile.vk, 0, MAX_STRING);
			break;
		case 'i':
			memset(contact->profile.inst, 0, MAX_STRING);
			break;
		default:
			va_end(argument_list);
			return UNKNOWN_EXPRESSION;
		}
	}

	va_end(argument_list);
	return ident;
}

//ОТОБРАЗИТЬ КОНТАКТ
void display_contact(const Contact* contact) {
	printf("\n=== Contact ID: %d ===\n", contact->ident);
	printf("First name: %s\n", contact->first_name);
	printf("Second name: %s\n", contact->second_name);
	printf("Middle name: %s\n", contact->middle_name);
	printf("Job: %s\n", contact->job);
	printf("Position: %s\n", contact->position);

	// Телефоны
	printf("Phones (%d):\n", contact->phone_count);
	for (int i = 0; i < contact->phone_count; i++) {
		printf("  [%d] %s\n", i, contact->number[i]);
	}

	// Email
	printf("Emails (%d):\n", contact->email_count);
	for (int i = 0; i < contact->email_count; i++) {
		printf("  [%d] %s\n", i, contact->email[i]);
	}

	// Соцсети
	printf("Social networks:\n");
	printf("  Telegram: %s\n", contact->profile.tg);
	printf("  VK: %s\n", contact->profile.vk);
	printf("  Instagram: %s\n", contact->profile.inst);
	printf("===========================\n");
}

//ОТОБРАЗИТЬ ТАБЛИЦУ
void display_all_contacts(const Book* book) {
	printf("\n========= PHONEBOOK =========\n");
	printf("Total contacts: %d\n", book->count);

	int printed = 0;
	for (int i = 0; i < MAX_COUNT; i++) {
		if (get_ident(book, i)) {
			display_contact(&book->contact[i]);
			printed++;
		}
	}

	if (printed == 0) {
		printf("No contacts found.\n");
	}
	printf("=============================\n\n");
}

void load_book(Book* book, const char* filename) {
    int fd = open(filename, O_RDONLY);

    if (fd < 0) return;

    Contact temp;
    while (read(fd, &temp, sizeof(Contact)) == sizeof(Contact)) {
        int id = temp.ident;
        
        if (id >= 0 && id < MAX_COUNT) {
            book->contact[id] = temp;
            book->mask[id / 8] |= (1 << (id % 8));
            book->count++;
        }
    }

    close(fd);
}

void save_book(Book* book, const char* filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    
    if (fd < 0) {
        perror("Ошибка при открытии файла для сохранения");
        return;
    }

    for (int i = 0; i < MAX_COUNT; i++) {
        if (get_ident(book, i)) {
            write(fd, &book->contact[i], sizeof(Contact));
        }
    }

    close(fd);
}