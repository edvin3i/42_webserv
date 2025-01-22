/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aguyon <aguyon@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/03 11:51:01 by aguyon            #+#    #+#             */
/*   Updated: 2024/04/07 19:14:59 by aguyon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

/* 2. Manipulation d'arbres binaires */

typedef struct TreeChar
{
	char data;
	struct TreeChar *left;
	struct TreeChar *right;
} TreeChar;

TreeChar *node(char c, TreeChar *left, TreeChar *right)
{
	TreeChar *n = malloc(sizeof(*n));

	assert(n != NULL);
	n->data = c;
	n->left = left;
	n->right = right;
	return (n);
}

TreeChar *leaf(char c)
{
	return (node(c, NULL, NULL));
}

bool is_leaf(TreeChar *tree)
{
	return (tree->right == NULL && tree->left == NULL);
}

void treechar_free(TreeChar **p_tree)
{
	if (*p_tree == NULL)
		return ;
	treechar_free(&(*p_tree)->left);
	treechar_free(&(*p_tree)->right);
	free(*p_tree);
	*p_tree = NULL;
}

static void treechar_print_aux(TreeChar *tree, int level)
{
	if (tree == NULL)
		return ;
	for (int i = 0; i < level; ++i)
		printf(i == level - 1 ? "|-" : "  ");
	if (isprint(tree->data))
		printf("%c\n", tree->data);
	else
		printf("\'%c\'\n", tree->data);
	treechar_print_aux(tree->left, level + 1);
	treechar_print_aux(tree->right, level + 1);
}

void treechar_print(TreeChar *tree)
{
	treechar_print_aux(tree, 0);
}

/* 3. Decodage et encodage */

typedef struct ListChar
{
	char data;
	struct ListChar *next;
}	ListChar;


ListChar *listchar_new(char c)
{
	ListChar *n = malloc(sizeof(*n));

	assert(n != NULL);
	n->data = c;
	n->next = NULL;
	return (n);
}

void listchar_push_front(ListChar **lst, char c)
{
	ListChar *elem = listchar_new(c);

	elem->next = *lst;
	*lst = elem;
}

ListChar *listchar_reverse(ListChar *lst)
{
	ListChar *new_lst = NULL;

	while (lst != NULL)
	{
		listchar_push_front(&new_lst, lst->data);
		lst = lst->next;
	}
	return (new_lst);
}

void listchar_free(ListChar **p_lst)
{
	if (*p_lst == NULL)
		return ;
	listchar_free(&(*p_lst)->next);
	free(*p_lst);
	*p_lst = NULL;
}

ListChar *decode_one(TreeChar *tree, ListChar *seq)
{
	assert(tree != NULL);
	if (tree->data == '.')
	{
		if (seq->data == '0')
			return (decode_one(tree->left, seq->next));
		else if (seq->data == '1')
			return (decode_one(tree->right, seq->next));
		else
		{
			fprintf(stderr, "Error, decode_one invalide character in seq\n");
			exit(1);
		}
	}
	else
	{
		printf("%c", tree->data);
		return (seq);
	}
}

void decode(TreeChar *tree, ListChar *seq)
{
	assert(tree != NULL);
	while (seq != NULL)
		seq = decode_one(tree, seq);
}

bool code_one(TreeChar *tree, char c, ListChar **seq)
{
	if (is_leaf(tree))
		return (tree->data == c);

	if (code_one(tree->right, c, seq))
	{
		listchar_push_front(seq, '1');
		return (true);
	}
	if (code_one(tree->left, c, seq))
	{
		listchar_push_front(seq, '0');
		return (true);
	}
	return (false);
}

ListChar *code(TreeChar *tree, const char *word)
{
	ListChar *seq;

	if (word[0] == '\0')
		return (NULL);
	seq = code(tree, word + 1);
	if (!code_one(tree, word[0], &seq))
	{
		fprintf(stderr, "Error, code_one tree doesn't contain %c\n", word[0]);
		exit(1);
	}
	return (seq);
}

/* 4. Construction de l'arbre de codage */

typedef struct PairTreeWeight
{
	TreeChar *subtree;
	int weight;
} PairTreeWeight;

typedef struct ListPair
{
	PairTreeWeight data;
	struct ListPair *next;
} ListPair;

ListPair *listpair_new(PairTreeWeight pair)
{
	ListPair *lst = malloc(sizeof(*lst));
	assert(lst != NULL);

	lst->data = pair;
	lst->next = NULL;
	return (lst);
}

ListPair *listpair_remove_first(ListPair **lst)
{
	if (*lst == NULL)
		return (NULL);
	ListPair *elem = *lst;
	*lst = elem->next;
	elem->next = NULL;
	return (elem);
}

void listpair_add(ListPair **lst, ListPair *elem)
{
	if (*lst == NULL)
	{
		*lst = elem;
		return ;
	}
	if (elem->data.weight <= (*lst)->data.weight)
	{
		elem->next = *lst;
		*lst = elem;
		return ;
	}
	ListPair *current = *lst, *next;
	while (current != NULL)
	{
		next = current->next;
		if (next == NULL || elem->data.weight <= next->data.weight)
		{
			current->next = elem;
			elem->next = next;
			return ;
		}
		current = next;
	}
}

TreeChar *make_huff(ListPair **lst)
{
	ListPair *p1, *p2;
	PairTreeWeight pair;

	if (lst == NULL)
		return (NULL);
	while ((*lst)->next != NULL)
	{
		p1 = listpair_remove_first(lst);
		p2 = listpair_remove_first(lst);

		pair = (PairTreeWeight){
			.subtree = node('.', p1->data.subtree, p2->data.subtree),
			.weight = p1->data.weight + p2->data.weight};
		listpair_add(lst, listpair_new(pair));
		free(p1);
		free(p2);
	}
	return ((*lst)->data.subtree);
}

ListPair *make_list(const char *str)
{
	ListPair *lst = NULL;
	PairTreeWeight pair;
	bool chars[256];
	memset(chars, 0, sizeof(bool) * 256);
	int freq;

	for (size_t i = 0; str[i]; ++i)
	{
		if (chars[(int)str[i]] == false)
		{
			freq = 1;
			for (size_t j = i + 1; str[j]; ++j)
			{
				if (str[j] == str[i])
					freq += 1;
			}
			pair = (PairTreeWeight){
				.subtree = leaf(str[i]),
				.weight = freq
			};
			listpair_add(&lst, listpair_new(pair));
			chars[(int)str[i]] = true;
		}
	}
	return (lst);
}

TreeChar *huff(const char *str)
{
	ListPair *lst = make_list(str);
	if (lst == NULL)
		return (NULL);
	TreeChar *res = make_huff(&lst);

	free(lst);
	return (res);
}

char *file_to_str(FILE *f)
{
	char *res = NULL, *buffer = NULL;
	size_t res_len = 0;
	size_t res_size = 0;
	ssize_t bread;

	while (1)
	{
		bread = getline(&buffer, &res_size, f);
		if (bread < 0)
		{
			if (errno == ENOMEM)
			{
				free(res);
				res = NULL;
			}
			free(buffer);
			break ;
		}
		res = realloc(res, res_len + bread + 1);
		assert(res != NULL);
		memcpy(res + res_len, buffer, bread);
		res[res_len + bread] = '\0';
		res_len += bread;
		free(buffer);
		buffer = NULL;
	}
	return (res);
}

void write_huff_file(ListChar *seq, FILE *f)
{
	__uint8_t buffer;
	while (seq != NULL)
	{
		for (size_t i = 0; i < 8; ++i)
		{
			
		}
		fwrite(&buffer, 1, 1, f);
		seq = seq->next;
	}
}

int main(int argc, char *argv[])
{
	switch (argc)
	{
		case 2:
			FILE *infile = fopen(argv[1], "r");

			if (!infile)
			{
				fprintf(stderr, "cannot open file: %s\n", argv[1]);
				exit(1);
			}
			char *file_str = file_to_str(infile);
			TreeChar *tree = huff(file_str);
			ListChar *seq = code(tree, file_str);
			listchar_free(tree);
			#if 0
			treechar_print(tree);
			#endif

			listchar_free(seq);
			free(file_str);
			fclose(infile);
			break ;
		case 3:
			fprintf(stderr, "not implemented\n");
			exit(1);
		default:
			fprintf(stderr, "wrong argument number\n");
			exit(1);
	}
	return (0);
}
