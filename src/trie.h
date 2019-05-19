#include "types.h"

typedef struct trie
{
    char_t is_leaf; /* 1 when node is a leaf node */
    uint32_t value; /* 0 when node is not a leaf */
    struct trie *character[ALPHABET_SIZE];
} trie_t;

typedef struct dictionary
{
	char_t key[STR_SIZE];
	uint32_t value;
} dictionary_t;

trie_t* trie_new_node();
void trie_insert(trie_t **head, char_t *str, uint32_t value);
trie_t* trie_search(trie_t* head, char_t *str);
char_t trie_delete(trie_t **curr, char_t *str);
void trie_delete_all(trie_t **curr);
void trie_get_all(trie_t *curr, char_t str[], int32_t level);
char_t trie_update_value(trie_t *head, char_t *str, uint32_t new_value);
char_t trie_from_file(int32_t fd, trie_t **head);
void trie_to_file(int32_t fd, trie_t *curr, char_t str[], int32_t level);
