typedef struct Node{
   char c;
   int count;
   struct Node *next, *left, *right;
} Node;

Node *new_node(unsigned char c, int count);

Node *sort(Node **arr);

void insert_sort(Node **head, Node *new_node);

int less(Node *one, Node *two);

int size(Node *linked_list);

Node *remove_from_head(Node **head);

Node *huffman_tree(Node **occurance);

void free_tree(Node *node);

Node **read_header(int fd);

void decode(int fd, int fd2, Node *huff);
