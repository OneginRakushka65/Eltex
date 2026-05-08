#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_PRIORITY 255

typedef struct Node {
	int priority;
	char date[32];
	struct Node* next;
} Node;

typedef struct {
	Node* heads[MAX_PRIORITY + 1];
	Node* tails[MAX_PRIORITY + 1];
} PriorityList;

void init_priority_list(PriorityList* list) {
	for (int i = 0; i <= MAX_PRIORITY; i++) {
		list->heads[i] = NULL;
		list->tails[i] = NULL;
	}
}

void add_priority_obj(PriorityList* list, const char* date, const int priority) {
	if (priority < 0 || priority > MAX_PRIORITY) {
		printf("Unknown priority % d\n", priority);
		return;
	}

	Node* new_node = malloc(sizeof(Node));
	if (new_node == NULL) {
		printf("Out of memory!\n");
		return;
	}
	strncpy(new_node->date, date, 31);

	new_node->priority = priority;
	new_node->next = NULL;

	if (list->tails[priority]) {
		list->tails[priority]->next = new_node;
		list->tails[priority] = new_node;
	}
	else {
		list->heads[priority] = new_node;
		list->tails[priority] = new_node;
	}
}

Node* extraction_priority_obj(PriorityList* list, const char* args, ...) {
	va_list argument_list;
	va_start(argument_list, args);

	Node* temp = NULL;

	const char* p = args;
	while (*p != '\0') {
		switch (*p++) {
		case 'f': // First
			for (int i = 0; i <= MAX_PRIORITY; i++) {
				if (list->heads[i]) {
					temp = list->heads[i];
					list->heads[i] = temp->next;
					if (list->heads[i] == NULL) list->tails[i] = NULL;
					va_end(argument_list);
					return temp;
				}
			}
			printf("List is empty\n");
			break;
		case 'p': // Priority
		{
			int p = va_arg(argument_list, int);
			if (p >= 0 && p <= MAX_PRIORITY && list->heads[p]) {
				temp = list->heads[p];
				list->heads[p] = temp->next;
				if (list->heads[p] == NULL) list->tails[p] = NULL;
				va_end(argument_list);
				return temp;
			}
			break;
		}
		case 'l': // Limit (not lower than)
		{
			int limit = va_arg(argument_list, int);
			for (int i = 0; i <= limit; i++) {
				if (i <= MAX_PRIORITY && list->heads[i]) {
					temp = list->heads[i];
					list->heads[i] = temp->next;
					if (list->heads[i] == NULL) list->tails[i] = NULL;
					va_end(argument_list);
					return temp;
				}
			}
			printf("List is empty\n");
			break;
		}
		default:
			printf("Incorrect input\n");
			break;
		}
	}

	va_end(argument_list);
	return NULL;
}

void free_priority_list(PriorityList* list) {
	for (int i = 0; i <= MAX_PRIORITY; i++) {
		Node* current = list->heads[i];
		while (current != NULL) {
			Node* temp = current;
			current = current->next;
			free(temp);
		}

		list->heads[i] = NULL;
		list->tails[i] = NULL;
	}
	printf("Memory cleared successfully.\n");
}

int main() {
	PriorityList q;
	init_priority_list(&q);

	printf("Generating messages\n");
	add_priority_obj(&q, "A", 0);
	add_priority_obj(&q, "B", 100);
	add_priority_obj(&q, "C", 255);
	add_priority_obj(&q, "D", 0);
	add_priority_obj(&q, "F", 0);

	printf("\nExtraction tests\n");

	Node* n = extraction_priority_obj(&q, "f");
	if (n) { printf("1. Extract First: %s (p:%d)\n", n->date, n->priority); free(n); }

	n = extraction_priority_obj(&q, "f");
	if (n) { printf("1. Extract First: %s (p:%d)\n", n->date, n->priority); free(n); }

	n = extraction_priority_obj(&q, "p", 255);
	if (n) { printf("2. Extract Priority 255: %s (p:%d)\n", n->date, n->priority); free(n); }

	n = extraction_priority_obj(&q, "l", 50);
	if (n) { printf("3. Extract Limit <= 50: %s (p:%d)\n", n->date, n->priority); free(n); }

	free_priority_list(&q);
	return 0;
}