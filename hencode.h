#ifndef HENCODE_H
#define HENCODE_H


typedef struct Node{
   char c;
   int count;
   struct Node *next, *left, *right;
} Node;

Node *new_node(char ci, int count);

Node **count_occurance(int fd);

Node *sort(Node **arr);

void insert_sort(Node **head, Node *new_node);

int less(Node *one, Node *two);

int size(Node *linked_list);

Node *remove_from_head(Node **head);

Node *huffman_tree(Node **occurance);

void gen_codes(Node *ht, int arr[], int top, int codes[256][128]);

void free_tree(Node *node);

void header(int fd, int fd2, Node **occurance);

void encode(int fd, int fd2, int codes[256][128]);

#endif
