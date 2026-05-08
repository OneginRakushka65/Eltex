#define _CRT_SECURE_NO_WARNINGS
#include "phonebook.h"

int get_ident(const Book* book, const int ident) {
	return find_node(book->root, ident) != NULL;
}

Contact get_contact(const Book* book, const int ident) {
	TreeNode* node = find_node(book->root, ident);
	if (node != NULL) {
		return node->contact;
	}

	Contact empty = { 0 };
	return empty;
}

int compare_contacts(const Contact* a, const Contact* b) {
    return strcmp(a->second_name, b->second_name);
}

TreeNode* insert_node(TreeNode* root, TreeNode* new_node) {
    if (root == NULL) {
        return new_node;
    }

    if (compare_contacts(&new_node->contact, &root->contact) < 0) {
        root->left = insert_node(root->left, new_node);
    }
    else {
        root->right = insert_node(root->right, new_node);
    }

    return root;
}

int set_contact(Book* book, const char* first_name, const char* second_name, const char* args, ...) {
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (!new_node) return OVERFLOW;

    memset(new_node, 0, sizeof(TreeNode));

    new_node->contact.ident = book->next_id++;
    strncpy(new_node->contact.first_name, first_name, MAX_STRING - 1);
    strncpy(new_node->contact.second_name, second_name, MAX_STRING - 1);

	va_list vl;
	va_start(vl, args);
	Contact* c = &new_node->contact;

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

	book->root = insert_node(book->root, new_node);
	book->count++;

	if (book->count > 0 && book->count % 10 == 0) {
		balance_book(book);
		printf("\nTree balanced\n");
	}

	return new_node->contact.ident;
}

TreeNode* find_node(TreeNode* root, const int ident) {
	if (root == NULL) return NULL;

	if (root->contact.ident == ident) return root;

	TreeNode* found = find_node(root->left, ident);
	if (found != NULL) return found;

	return find_node(root->right, ident);
}

TreeNode* find_by_surname(TreeNode* root, const char* surname) {
	if (root == NULL) return NULL;

	int cmp = strcmp(surname, root->contact.second_name);

	if (cmp == 0) return root;

	if (cmp < 0) {
		return find_by_surname(root->left, surname);
	}
	else {
		return find_by_surname(root->right, surname);
	}
}

TreeNode* find_min(TreeNode* node) {
	while (node->left != NULL) node = node->left;
	return node;
}

TreeNode* delete_tree_node(TreeNode* root, int ident, int* deleted_flag) {
	if (root == NULL) return NULL;

	if (root->contact.ident == ident) {
		*deleted_flag = 1;

		if (root->left == NULL) {
			TreeNode* temp = root->right;
			free(root);
			return temp;
		}
		else if (root->right == NULL) {
			TreeNode* temp = root->left;
			free(root);
			return temp;
		}

		TreeNode* temp = find_min(root->right);
		root->contact = temp->contact;
		root->right = delete_tree_node(root->right, temp->contact.ident, deleted_flag);
	}
	else {
		root->left = delete_tree_node(root->left, ident, deleted_flag);
		root->right = delete_tree_node(root->right, ident, deleted_flag);
	}

	return root;
}

int delete_contact(Book* book, const int ident) {
	int deleted = 0;
	book->root = delete_tree_node(book->root, ident, &deleted);

	if (deleted) {
		book->count--;
		return ident;
	}

	printf("Контакт с ID %d не найден\n", ident);
	return -1;
}

void tree_to_array(TreeNode* root, TreeNode** arr, int* idx) {
	if (root == NULL) return;
	tree_to_array(root->left, arr, idx);
	arr[(*idx)++] = root;
	tree_to_array(root->right, arr, idx);
}

TreeNode* build_balanced_tree(TreeNode** arr, int start, int end) {
	if (start > end) return NULL;

	int mid = (start + end) / 2;
	TreeNode* root = arr[mid];

	root->left = build_balanced_tree(arr, start, mid - 1);
	root->right = build_balanced_tree(arr, mid + 1, end);

	return root;
}

void update_contact_fields(Contact* c, const char* flag, va_list vl) {
	if (strcmp(flag, "m") == 0) strncpy(c->middle_name, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "j") == 0) strncpy(c->job, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "p") == 0) strncpy(c->position, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "t") == 0) strncpy(c->profile.tg, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "v") == 0) strncpy(c->profile.vk, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "i") == 0) strncpy(c->profile.inst, va_arg(vl, char*), MAX_STRING - 1);
	else if (strcmp(flag, "n") == 0) {
		int count = va_arg(vl, int);
		for (int i = 0; i < count; i++) {
			int idx = va_arg(vl, int);
			char* val = va_arg(vl, char*);
			if (idx >= 0 && idx < MAX_NUMBER_COUNT) {
				strncpy(c->number[idx], val, MAX_PHONE_LEN - 1);
				if (idx >= c->phone_count) c->phone_count = idx + 1;
			}
		}
	}
	else if (strcmp(flag, "e") == 0) {
		int count = va_arg(vl, int);
		for (int i = 0; i < count; i++) {
			int idx = va_arg(vl, int);
			char* val = va_arg(vl, char*);
			if (idx >= 0 && idx < MAX_EMAIL_COUNT) {
				strncpy(c->email[idx], val, MAX_STRING - 1);
				if (idx >= c->email_count) c->email_count = idx + 1;
			}
		}
	}
}

int update_contact(Book* book, int ident, const char* flag, ...) {
	TreeNode* node = find_node(book->root, ident);
	if (!node) return NO_ENTRY;

	va_list vl;
	va_start(vl, flag);
	update_contact_fields(&node->contact, flag, vl);
	va_end(vl);

	return ident;
}

int delete_attribute(Book* book, int ident, const char* flag, ...) {
	TreeNode* node = find_node(book->root, ident);
	if (!node) return NO_ENTRY;

	Contact* c = &node->contact;
	if (strcmp(flag, "m") == 0) memset(c->middle_name, 0, MAX_STRING);
	else if (strcmp(flag, "j") == 0) memset(c->job, 0, MAX_STRING);
	else if (strcmp(flag, "p") == 0) memset(c->position, 0, MAX_STRING);
	else if (strcmp(flag, "t") == 0) memset(c->profile.tg, 0, MAX_STRING);
	else if (strcmp(flag, "v") == 0) memset(c->profile.vk, 0, MAX_STRING);
	else if (strcmp(flag, "i") == 0) memset(c->profile.inst, 0, MAX_STRING);
	else if (strcmp(flag, "n") == 0 || strcmp(flag, "e") == 0) {
		va_list vl;
		va_start(vl, flag);
		int count = va_arg(vl, int);
		for (int i = 0; i < count; i++) {
			int idx = va_arg(vl, int);
			if (flag[0] == 'n' && idx < c->phone_count) {
				memset(c->number[idx], 0, MAX_PHONE_LEN);
				if (idx == c->phone_count - 1) c->phone_count--;
			}
			if (flag[0] == 'e' && idx < c->email_count) {
				memset(c->email[idx], 0, MAX_STRING);
				if (idx == c->email_count - 1) c->email_count--;
			}
		}
		va_end(vl);
	}
	return ident;
}

void balance_book(Book* book) {
	if (book->count < 3) return;

	TreeNode** arr = (TreeNode**)malloc(sizeof(TreeNode*) * book->count);
	int idx = 0;

	tree_to_array(book->root, arr, &idx);

	book->root = build_balanced_tree(arr, 0, book->count - 1);

	free(arr);
}

void free_nodes(TreeNode* node) {
	if (node == NULL) return;
	free_nodes(node->left);
	free_nodes(node->right);
	free(node);
}

void free_book(Book* book) {
	if (book->root != NULL) {
		free_nodes(book->root);
		book->root = NULL;
	}
	book->count = 0;
	book->next_id = 0;
}

void display_tree(TreeNode* node) {
	if (node == NULL) return;
	display_tree(node->left);
	display_contact(&node->contact);
	display_tree(node->right);
}

void display_contact(const Contact* contact) {
	printf("\n=== Contact ID: %d ===\n", contact->ident);
	printf("First name: %s\n", contact->first_name);
	printf("Second name: %s\n", contact->second_name);
	printf("Middle name: %s\n", contact->middle_name);
	printf("Job: %s\n", contact->job);
	printf("Position: %s\n", contact->position);

	printf("Phones (%d):\n", contact->phone_count);
	for (int i = 0; i < contact->phone_count; i++) {
		printf("  [%d] %s\n", i, contact->number[i]);
	}

	printf("Emails (%d):\n", contact->email_count);
	for (int i = 0; i < contact->email_count; i++) {
		printf("  [%d] %s\n", i, contact->email[i]);
	}

	printf("Social networks:\n");
	printf("  Telegram: %s\n", contact->profile.tg);
	printf("  VK: %s\n", contact->profile.vk);
	printf("  Instagram: %s\n", contact->profile.inst);
	printf("===========================\n");
}

void display_all_contacts(const Book* book) {
	printf("\n========= PHONEBOOK (TREE) =========\n");
	printf("Total contacts: %d\n", book->count);
	if (book->root == NULL) {
		printf("No contacts found.\n");
	}
	else {
		display_tree(book->root);
	}
	printf("====================================\n");
}