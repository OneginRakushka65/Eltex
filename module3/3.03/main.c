#define _CRT_SECURE_NO_WARNINGS
#include "phonebook.h"
#include "main.h"

enum Command {CMD_DISPLAY_BOOK, CMD_DISPLAY_CONTACT, CMD_ADD, CMD_UPDATE, CMD_DELETE, CMD_DELETE_ATTRIBUTE, CMD_EXIT, CMD_UNKNOWN};

enum Command parse_command(const char* command) {
	if (strcmp(command, "display_book") == 0) return CMD_DISPLAY_BOOK;
	if (strcmp(command, "display_contact") == 0) return CMD_DISPLAY_CONTACT;
	if (strcmp(command, "add") == 0) return CMD_ADD;
	if (strcmp(command, "update") == 0) return CMD_UPDATE;
	if (strcmp(command, "delete") == 0) return CMD_DELETE;
	if (strcmp(command, "delete_attribute") == 0) return CMD_DELETE_ATTRIBUTE;
	if (strcmp(command, "exit") == 0) return CMD_EXIT;
	return CMD_UNKNOWN;
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

	display_contact(&book->contact[ident]);
}

void main_add_contact(Book* book, char* args) {
	if (args == NULL || args[0] == '\0') {
		printf("Features: add <FirstName> <SecondName> [-m middle] [-j job] [-p pos] [-n phone] [-e email] [-t tg] [-v vk] [-i inst]\n");
		return;
	}

	char* fname = strtok(args, " ");
	char* sname = strtok(NULL, " ");

	if (fname == NULL || sname == NULL) {
		printf("Error: First name and Second name are required.\n");
		return;
	}

	char format[20] = { 0 };
	char* val_ptrs[20] = { 0 };
	int arg_count = 0;

	char* token = strtok(NULL, " ");
	while (token != NULL && arg_count < 19) {
		if (token[0] == '-' && strlen(token) == 2) {
			char flag = token[1];
			char* val = strtok(NULL, " ");

			if (val) {
				format[arg_count] = flag;
				val_ptrs[arg_count] = val;
				arg_count++;
			}
			else {
				printf("Warning: Missing value for flag %s. Skipping.\n", token);
				break;
			}
		}
		token = strtok(NULL, " ");
	}

	int result = set_contact(book, fname, sname, format,
		val_ptrs[0], val_ptrs[1], val_ptrs[2], val_ptrs[3],
		val_ptrs[4], val_ptrs[5], val_ptrs[6], val_ptrs[7],
		val_ptrs[8], val_ptrs[9], val_ptrs[10], val_ptrs[11],
		val_ptrs[12], val_ptrs[13], val_ptrs[14], val_ptrs[15],
		val_ptrs[16], val_ptrs[17], val_ptrs[18], val_ptrs[19]);

	if (result >= 0) {
		printf("Contact added successfully with ID: %d\n", result);
	}
	else {
		printf("Failed to add contact. Error: ");
		switch (result) {
		case OVERFLOW: printf("Phonebook is full.\n"); break;
		case OVERFLOW_NUMBERS: printf("Too many phone numbers.\n"); break;
		case OVERFLOW_EMAILS: printf("Too many emails.\n"); break;
		case UNKNOWN_EXPRESSION: printf("Unknown attribute flag.\n"); break;
		default: printf("Unknown error (%d).\n", result); break;
		}
	}
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
	printf("Module 2 Task 1 for Eltex now working\n");
	printf("Features: display_book, display_contact, add, update, delete, delete_attribute, exit\n");

	while (1) {
		printf(">> ");

		char command[MAX_COMMAND];
		char* token;

		fgets(command, sizeof(command), stdin);
		command[strcspn(command, "\n")] = 0;

		token = strtok(command, " ");
		if (!token) continue;

		char* args = strtok(NULL, "");

		switch (parse_command(token)) {
		case CMD_DISPLAY_BOOK:
			display_all_contacts(book);
			break;
		case CMD_DISPLAY_CONTACT:
			main_display_contact(book, args);
			break;
		case CMD_ADD:
            main_add_contact(book, args);
			break;
		case CMD_UPDATE:
			main_update_contact(book, args);
			break;
		case CMD_DELETE:
			main_delete_contact(book, args);
			break;
		case CMD_DELETE_ATTRIBUTE:
			main_delete_attribute(book, args);
			break;
		case CMD_EXIT:
			printf("Goodbye! Thanks for using Phonebook!\n");
			return 0;
		default:
			printf("Unknown command: '%s'.\n", token);
			printf("Features: add, update, delete, display_book, display_contact, delete_attribute, exit\n");
			break;
		}
	}
}

int main() {
    const char* filename = "phonebook.dat";
	Book book = { 0 };
    load_book(&book, filename);
	command_handler(&book);
    save_book(&book, filename);
    return 0;
}