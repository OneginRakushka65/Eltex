#define _CRT_SECURE_NO_WARNINGS
#include "phonebook.h"

int set_ident(Book* book) {
	return book->next_id++;
}

int get_ident(const Book* book, const int ident) {
	return find_node(book, ident) != NULL;
}

Node* find_node(const Book* book, const int ident) {
	Node* current = book->head;
	while (current != NULL) {
		if (current->contact.ident == ident) return current;
		current = current->next;
	}

	return NULL;
}

Contact get_contact(const Book* book, const int ident) {
	Node* node = find_node(book, ident);
	if (node != NULL) {
		return node->contact;
	}

	Contact empty = { 0 };
	return empty;
}

int set_contact(Book* book, const char* first_name, const char* second_name, const char* args, ...) {
	Node* new_node = (Node*)malloc(sizeof(Node));
	if (!new_node) return OVERFLOW;

	memset(new_node, 0, sizeof(Node));

	new_node->contact.ident = set_ident(book);
	strncpy(new_node->contact.first_name, first_name, MAX_STRING - 1);
	strncpy(new_node->contact.second_name, second_name, MAX_STRING - 1);

	va_list vl;
	va_start(vl, args);
	Contact* c = &new_node->contact; // Используем 'c' везде ниже

	const char* p = args;
	while (*p != '\0') {
		switch (*p) {
		case 'm': strncpy(c->middle_name, va_arg(vl, char*), MAX_STRING - 1); break;
		case 'j': strncpy(c->job, va_arg(vl, char*), MAX_STRING - 1); break;
		case 'p': strncpy(c->position, va_arg(vl, char*), MAX_STRING - 1); break;
		case 't': strncpy(c->profile.tg, va_arg(vl, char*), MAX_STRING - 1); break;
		case 'v': strncpy(c->profile.vk, va_arg(vl, char*), MAX_STRING - 1); break;
		case 'i': strncpy(c->profile.inst, va_arg(vl, char*), MAX_STRING - 1); break;
		case 'n': {
			if (c->phone_count < MAX_NUMBER_COUNT) {
				char* num = va_arg(vl, char*);
				strncpy(c->number[c->phone_count], num, MAX_PHONE_LEN - 1);
				c->number[c->phone_count][MAX_PHONE_LEN - 1] = '\0';
				c->phone_count++;
			}
			break;
		}
		case 'e': {
			if (c->email_count < MAX_EMAIL_COUNT) {
				char* mail = va_arg(vl, char*);
				strncpy(c->email[c->email_count], mail, MAX_STRING - 1);
				c->email[c->email_count][MAX_STRING - 1] = '\0';
				c->email_count++;
			}
			break;
		}
		}
		p++;
	}
	va_end(vl);

	insert_sorted(book, new_node);
	book->count++;
	return new_node->contact.ident;
}

void insert_sorted(Book* book, Node* new_node) {
	if (book->head == NULL) {
		book->head = book->tail = new_node;
		new_node->next = new_node->prev = NULL;
		return;
	}

	Node* current = book->head;

	while (current != NULL && strcmp(current->contact.second_name, new_node->contact.second_name) < 0) {
		current = current->next;
	}

	if (current == book->head) {
		new_node->next = book->head;
		new_node->prev = NULL;
		book->head->prev = new_node;
		book->head = new_node;
	}
	else if (current == NULL) {
		new_node->next = NULL;
		new_node->prev = book->tail;
		book->tail->next = new_node;
		book->tail = new_node;
	}
	else {
		new_node->next = current;
		new_node->prev = current->prev;
		current->prev->next = new_node;
		current->prev = new_node;
	}
}

int delete_contact(Book* book, const int ident) {
	Node* current = find_node(book, ident);
	if (current == NULL) {
		printf("Node not found");
		return UNKNOWN_EXPRESSION;
	}

	if (current->prev != NULL) {
		current->prev->next = current->next;
	}
	else {
		book->head = current->next;
	}

	if (current->next != NULL) {
		current->next->prev = current->prev;
	}
	else {
		book->tail = current->prev;
	}

	free(current);
	book->count--;
	return ident;
}

static void detach_node(Book* book, Node* node) {
	if (node->prev != NULL) {
		node->prev->next = node->next;
	}
	else {
		book->head = node->next;
	}

	if (node->next != NULL) {
		node->next->prev = node->prev;
	}
	else {
		book->tail = node->prev;
	}

	node->next = node->prev = NULL;
}

int update_contact(Book* book, const int ident, const char* args, ...) {
	Node* current = find_node(book, ident);
	if (current == NULL) return NO_ENTRY;

	char old_surname[MAX_STRING];
	strcpy(old_surname, current->contact.second_name);

	va_list argument_list;
	va_start(argument_list, args);

	Contact* contact = &current->contact;

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

	if (strcmp(old_surname, contact->second_name) != 0) {
		detach_node(book, current);
		insert_sorted(book, current);
	}

	return ident;
}

int delete_attribute(Book* book, const int ident, const char* args, ...) {
	Node* current = find_node(book, ident);
	if (current == NULL) return NO_ENTRY;

	va_list argument_list;
	va_start(argument_list, args);

	Contact* contact = &current->contact;

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

void free_book(Book* book) {
	if (book == NULL || book->head == NULL) return;

	Node* current = book->head;
	while (current != NULL) {
		Node* next_node = current->next;
		free(current);
		current = next_node;
	}

	book->head = NULL;
	book->tail = NULL;
	book->count = 0;
	book->next_id = 0;
}

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

void display_all_contacts(const Book* book) {
	printf("\n========= PHONEBOOK =========\n");
	printf("Total contacts: %d\n", book->count);

	if (book->head == NULL) {
		printf("No contacts found.\n");
	}
	else {
		Node* current = book->head;
		while (current != NULL) {
			display_contact(&current->contact);
			current = current->next;
		}
	}
	printf("=============================\n\n");
}