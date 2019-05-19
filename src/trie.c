#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "trie.h"

trie_t* trie_new_node()
{
    trie_t *node = (trie_t*) malloc(sizeof(trie_t));
	/* return NULL if memory allocation failed */
	if(node == NULL)
	{
		return NULL;		
	}

    node->is_leaf = 0;

    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
    {
        node->character[i] = NULL;
        node->value = 0;
    }

    return node;
}

//TODO Check return value of malloc, return success
void trie_insert(trie_t **head, char_t *str, uint32_t value)
{
    /* return if the value is not valid */
    if(value == 0)
    {
        return;
    }

    /* start from root node */
    trie_t *curr = *head;
    while(*str != '\0')
    {
        /* create a new node if path does not exist */
        if(curr->character[*str - ALPHABET_START] == NULL)
        {
            curr->character[*str - ALPHABET_START] = trie_new_node();
        }

        /* go to next node */
        curr = curr->character[*str - ALPHABET_START];

        /* move to next character */
        str++;
    }

    /* mark current node as leaf */
    curr->is_leaf = 1;

    /* insert the value */
    curr->value = value;
}

trie_t* trie_search(trie_t* head, char_t *str)
{
    /* return NULL if trie is empty */
    if(head == NULL)
    {
        return NULL;
    }

    trie_t *curr = head;
    while(*str != '\0')
    {
        /* go to next character node */
        curr = curr->character[*str - ALPHABET_START];

        /* reached end of path in trie and string has not been found */
        if(curr == NULL)
        {
            return NULL;
        }

        /* move to next character */
        str++;
    }

    /* if current node is a leaf and we have reached the end of the string, return the node */
    return curr->is_leaf == 1 ? curr : NULL;
}

uint32_t trie_get_value(trie_t *head, char_t *str)
{
    /* return -1 if trie is empty */
    if(head == NULL)
    {
        return -1;
    }

    trie_t *leaf = trie_search(head, str);

    if(leaf != NULL)
    {
        return leaf->value;
    }
    else
    {
        return 0;
    }
}

char_t trie_has_children(trie_t *curr)
{
    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
    {
        if(curr->character[i] != NULL)
        {
            return 1;
        }
    }

    return 0;
}

char_t trie_delete(trie_t **curr, char_t *str)
{
    /* return 0 if trie is empty */
    if(curr == NULL)
    {
        return 0;
    }

    /* if we have not searched the end of the string */
    if(*str != '\0')
    {
        /* recurse for the node corresponding to the next character in the string, if it returns 1 then delete the current node */
        if( *curr != NULL 
			&& (*curr)->character[*str - ALPHABET_START] != NULL 
			&& trie_delete(&((*curr)->character[*str - ALPHABET_START]), str + 1) == 1 
			&& (*curr)->is_leaf == 0 )
        {
            if(trie_has_children(*curr) == 1)
            {
                (*curr)->value = 0;
                free(*curr);
                (*curr) = NULL;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    /* if we have reached the end of the string */
    if(*str == '\0' && (*curr)->is_leaf == 1)
    {
        /* if current node is a leaf node and does not have any children */
        if(trie_has_children(*curr) == 0)
        {
            (*curr)->value = 0;
            free(*curr); /* free current node */
            (*curr) = NULL;
            return 1; /* delete non-leaf parent nodes */
        }
        /* if current node is a leaf node and has children */
        else
        {
            (*curr)->value = 0;
            (*curr)->is_leaf = 0; /* mark current node as leaf and do not delete it */
            return 0; /* do not delete parent nodes */
        }
    }

    return 0;
}

void trie_delete_all(trie_t **curr)
{
    /* return if node is empty */
    if((*curr) == NULL)
    {
        return;
    }

    /* recurse through all nodes */
    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
    {
        trie_delete_all(&((*curr)->character[i]));
    }

    /* free current node */
    (*curr)->is_leaf = 0;
    (*curr)->value = 0;
    free((*curr));
    (*curr) = NULL;
}

void trie_get_all(trie_t *curr, char_t str[], int32_t level)
{
    /* return if node is empty */
    if(curr == NULL)
    {
        return;
    }

    /* we have found a string */
    if(curr->is_leaf == 1)
    {
        str[level] = '\0';
        printf("%s --- %d\n", str, curr->value);
    }

    /* recurse through all nodes */
    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
    {
        if(curr->character[i] != NULL)
        {
            str[level] = i + ALPHABET_START;
            trie_get_all(curr->character[i], str, level + 1);
        }
    }
}

char_t trie_update_value(trie_t *head, char_t *str, uint32_t new_value)
{
    /* return 0 if trie is empty */
    if(head == NULL)
    {
        return 0;
    }

    trie_t *leaf = trie_search(head, str);

    if(leaf != NULL)
    {
        leaf->value = new_value;

        /* delete the node if the value has become invalid */
        if(leaf->value <= 0)
        {
            (void)trie_delete(&head, str);
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

char_t trie_from_file(int32_t fd, trie_t **head)
{
	/* return 0 if file descriptor is not valid i.e. negative */
	if(fd <= 0)
	{
		return 0;
	}

	char_t success_flag = 0;
	int bytes_read = 0;
	dictionary_t data;

	/* infinite loop */
	for(;;)
    {
		bytes_read = read(fd, &data, sizeof(dictionary_t));
		if(bytes_read != 0)
		{
			//TODO Check return value of insert
			trie_insert(head, data.key, data.value);

			/* indicates that data has been inserted into the trie at least once */
			success_flag = 1;
		}
		else
		{
			/* reached the end of the file, discontinue reading */
			break;
		}
    }

	return success_flag;
}

void trie_to_file(int32_t fd, trie_t *curr, char_t str[], int32_t level)
{
	/* return if file descriptor is not valid i.e. negative */
	if(fd <= 0)
	{
		return;
	}

	/* return if node is empty */
    if(curr == NULL)
    {
        return;
    }

	dictionary_t data;
	data.value = 0;
	memset(data.key, 0, sizeof(data.key));

    /* we have found a string */
    if(curr->is_leaf == 1)
    {
        str[level] = '\0';
		strcpy(data.key, str);
		data.value = curr->value;

		//TODO Check return value of write
		write(fd, &data, sizeof(dictionary_t));
    }

    /* recurse through all nodes */
    for(int32_t i = 0; i < ALPHABET_SIZE; i++)
    {
        if(curr->character[i] != NULL)
        {
            str[level] = i + ALPHABET_START;
            trie_to_file(fd, curr->character[i], str, level + 1);
        }
    }
}
