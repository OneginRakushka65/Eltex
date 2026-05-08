#define _CRT_SECURE_NO_WARNINGS
#include "phonebook.h"
#include "main.h"

enum Command { CMD_DISPLAY_BOOK, CMD_DISPLAY_CONTACT, CMD_FIND, CMD_ADD, CMD_UPDATE, CMD_DELETE, CMD_DELETE_ATTRIBUTE, CMD_EXIT, CMD_UNKNOWN };

enum Command parse_command(const char* command) {
    if (strcmp(command, "display_book") == 0) return CMD_DISPLAY_BOOK;
    if (strcmp(command, "display_contact") == 0) return CMD_DISPLAY_CONTACT;
    if (strcmp(command, "find") == 0) return CMD_FIND; // Новая команда
    if (strcmp(command, "add") == 0) return CMD_ADD;
    if (strcmp(command, "update") == 0) return CMD_UPDATE;
    if (strcmp(command, "delete") == 0) return CMD_DELETE;
    if (strcmp(command, "delete_attribute") == 0) return CMD_DELETE_ATTRIBUTE;
    if (strcmp(command, "exit") == 0) return CMD_EXIT;
    return CMD_UNKNOWN;
}

void main_find_by_surname(Book* book, char* args) {
    if (args == NULL || args[0] == '\0') {
        printf("Usage: find <Surname>\n");
        return;
    }

    TreeNode* node = find_by_surname(book->root, args);
    if (node) {
        display_contact(&node->contact);
    }
    else {
        printf("No contact found with surname: %s\n", args);
    }
}

void main_display_contact(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') {
		printf("Features: display_contact <ID>\n");
		return;
	}

	int ident = atoi(args);

	if (!get_ident(book, ident)) {
		printf("Contact with ID %d not found.\n", ident);
		return;
	}

	Contact c = get_contact(book, ident);
	display_contact(&c);
}

void main_add_contact(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') {
		printf("Usage: add <FName> <SName> [-n count idx1 num1...] [-e count idx1 mail1...]\n");
		return;
	}

	char* fname = strtok(args, " ");
	char* sname = strtok(NULL, " ");

	if (fname == NULL || sname == NULL) {
		printf("Error: First name and Second name are required.\n");
		return;
	}

	int ident = set_contact(book, fname, sname, "");
	if (ident < 0) return;

	char* token = strtok(NULL, " ");
	while (token != NULL) {
		if (token[0] == '-' && strlen(token) == 2) {
			char flag = token[1];
			char flag_str[2] = { flag, '\0' };

			if (flag == 'n' || flag == 'e') {
				char* count_str = strtok(NULL, " ");
				if (!count_str) break;
				int count = atoi(count_str);

				int idxs[10] = { 0 };
				char* vals[10] = { NULL };

				for (int i = 0; i < count && i < 10; i++) {
					char* idx_ptr = strtok(NULL, " ");
					char* val_ptr = strtok(NULL, " ");
					if (idx_ptr && val_ptr) {
						idxs[i] = atoi(idx_ptr);
						vals[i] = val_ptr;
					}
				}

				update_contact(book, ident, flag_str, count,
					idxs[0], vals[0], idxs[1], vals[1], idxs[2], vals[2],
					idxs[3], vals[3], idxs[4], vals[4], idxs[5], vals[5],
					idxs[6], vals[6], idxs[7], vals[7], idxs[8], vals[8],
					idxs[9], vals[9]);
			}
			else {
				char* val = strtok(NULL, " ");
				if (val) update_contact(book, ident, flag_str, val);
			}
		}
		token = strtok(NULL, " ");
	}
	printf("Contact added successfully with ID: %d\n", ident);
}

void main_update_contact(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') return;

	char* id_token = strtok(args, " ");
	int ident = atoi(id_token);

	char* token = strtok(NULL, " ");
	while (token != NULL) {
		if (token[0] == '-' && strlen(token) == 2) {
			char flag = token[1];
			char flag_str[2] = { flag, '\0' };

			if (flag == 'n' || flag == 'e') {
				int count = atoi(strtok(NULL, " "));

				int idxs[10] = { 0 };
				char* vals[10] = { NULL };

				for (int i = 0; i < count && i < 10; i++) {
					idxs[i] = atoi(strtok(NULL, " "));
					vals[i] = strtok(NULL, " ");
				}

				update_contact(book, ident, flag_str, count,
					idxs[0], vals[0], idxs[1], vals[1], idxs[2], vals[2],
					idxs[3], vals[3], idxs[4], vals[4], idxs[5], vals[5],
					idxs[6], vals[6], idxs[7], vals[7], idxs[8], vals[8],
					idxs[9], vals[9]);

				printf("Packet update for '-%c' (%d entries) done.\n", flag, count);
			}
			else {
				char* val = strtok(NULL, " ");
				if (val) update_contact(book, ident, flag_str, val);
			}
		}
		token = strtok(NULL, " ");
	}
}

void main_delete_contact(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') {
		printf("Features: delete <ID>\n");
		return;
	}

	int ident = atoi(args);

	if (!get_ident(book, ident)) {
		printf("Error: Contact with ID %d not found.\n", ident);
		return;
	}

	int result = delete_contact(book, ident);

	if (result == NO_ENTRY) {
		printf("Error: Could not delete contact (Entry not found).\n");
	}
	else {
		printf("Contact with ID %d has been successfully deleted.\n", result);
	}
}

void main_delete_attribute(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') return;

	int ident = atoi(strtok(args, " "));
	char* token = strtok(NULL, " ");

	while (token != NULL) {
		if (token[0] == '-' && strlen(token) == 2) {
			char flag = token[1];
			char flag_str[2] = { flag, '\0' };

			if (flag == 'n' || flag == 'e') {
				int count = atoi(strtok(NULL, " "));
				int idxs[10] = { 0 };

				for (int i = 0; i < count && i < 10; i++) {
					idxs[i] = atoi(strtok(NULL, " "));
				}

				delete_attribute(book, ident, flag_str, count,
					idxs[0], idxs[1], idxs[2], idxs[3], idxs[4],
					idxs[5], idxs[6], idxs[7], idxs[8], idxs[9]);

				printf("Packet delete for '-%c' (%d indices) done.\n", flag, count);
			}
			else {
				delete_attribute(book, ident, flag_str);
			}
		}
		token = strtok(NULL, " ");
	}
}

int command_handler(Book* book) {
	printf("Phonebook v3.0 (Binary Search Tree) ready.\n");
	printf("Type 'find <surname>' for fast searching.\n");

	while (1) {
		printf(">> ");
		char command[MAX_COMMAND];
		if (!fgets(command, sizeof(command), stdin)) break;
		command[strcspn(command, "\n")] = 0;

		char* token = strtok(command, " ");
		if (!token) continue;
		char* args = strtok(NULL, "");

		switch (parse_command(token)) {
		case CMD_DISPLAY_BOOK:    display_all_contacts(book); break;
		case CMD_DISPLAY_CONTACT: main_display_contact(book, args); break;
		case CMD_FIND:            main_find_by_surname(book, args); break;
		case CMD_ADD:             main_add_contact(book, args); break;
		case CMD_UPDATE:          main_update_contact(book, args); break;
		case CMD_DELETE:          main_delete_contact(book, args); break;
		case CMD_DELETE_ATTRIBUTE:main_delete_attribute(book, args); break;
		case CMD_EXIT:
			free_book(book);
			printf("Goodbye!\n");
			return 0;
		default:
			printf("Unknown command.\n");
			break;
		}
	}
	return 0;
}

int main() {
	Book book;
	book.root = NULL;
	book.count = 0;
	book.next_id = 1;

	command_handler(&book);
	return 0;
}