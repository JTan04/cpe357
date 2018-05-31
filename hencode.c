#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hencode.h"
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

Node **count_occurance(int fd)
{
   int i;
   char c;
   char buf[1];
   Node **array_list = (Node **)malloc(sizeof(Node) * 256);


   for(i = 0; i < 256; i++)
   {
      array_list[i] = NULL;
   }



   while(read(fd, buf, 1) > 0)
   {
      c = (int)*buf;
      if(array_list[(unsigned int)c] == NULL)
      {
         array_list[(unsigned int)c] = new_node(c, 1);
      }
      else
      {
         array_list[(unsigned int)c]->count++;
      }
   }


   close(fd);
   return array_list;
}

Node *new_node(char c, int count)
{
   Node *new = (Node *)malloc(sizeof(Node));
   new->c = c;
   new->count = count;
   new->next = NULL;
   new->left = NULL;
   new->right = NULL;
   return new;
}


Node *sort(Node **arr)
{
   int i;
   Node *new = NULL;
   for(i = 0; i < 256; i++)
   {
      if(arr[i] != NULL)
      {
         insert_sort(&new, arr[i]);
      }
   }
   return new;
}


void insert_sort(Node **head, Node *new_node)
{
   Node *curr;
   if(*head == NULL || !less(*head, new_node))
   {
      new_node->next = *head;
      *head = new_node;
   }
   else
   {
      curr = *head;
      while(curr->next != NULL && less(curr->next, new_node))
      {
         curr = curr->next;
      }
      new_node->next = curr->next;
      curr->next = new_node;
   }
}

int less(Node *one, Node *two)
{
   if(one->count < two->count)
   {
      return 1;
   }
   else if((one->count == two->count) && (one->c < two->c))
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

int size(Node *linked_list)
{
   int size = 0;
   while(linked_list != NULL)
   {
      size++;
      linked_list = linked_list->next;
   }
   return size;
}

Node *remove_from_head(Node **head)
{
   Node *temp = NULL;

   if(*head == NULL)
   {
      return NULL;
   }
   else
   {
      temp = *head;
      *head = (*head)->next;
   }
   return temp;
}



Node *huffman_tree(Node **occurance)
{
   Node *leaf1 = NULL;
   Node *leaf2 = NULL;
   Node *parent = NULL;
   Node *linked_list = sort(occurance);
   int s = size(linked_list);
   while(s > 1)
   {
      leaf1 = remove_from_head(&linked_list);
      leaf2 = remove_from_head(&linked_list);
      parent = new_node('\0', leaf1->count + leaf2->count);
      parent->left = leaf1;
      parent->right = leaf2;
      insert_sort(&linked_list, parent);
      s--;
   }
   return linked_list;
}

void gen_codes(Node *ht, int arr[], int top, int codes[256][128])
{
   int i = 0;
   if(ht->left)
   {
      arr[top] = 0;
      gen_codes(ht->left, arr, top+1, codes);
   }
   if(ht->right)
   {
      arr[top] = 1;
      gen_codes(ht->right, arr, top+1, codes);
   }
   if(!(ht->left) && !(ht->right))
   {
      for(i=0; i<top; i++)
      {
         codes[(int)(ht->c)][i] = arr[i];
      }
   }
}


void free_tree(Node *node)
{
   if(node == NULL)
   {
      return;
   }
   free_tree(node->right);
   free_tree(node->left);
   free(node);
}

void header(int fd, int fd2, Node **occurance)
{
   uint8_t *ch;
   uint32_t *cn;
   int size;
   uint32_t usize;
   int i;
   ch = malloc(sizeof(uint8_t));
   cn = malloc(sizeof(uint32_t));
   size = 0;
   for(i = 0; i < 256; i++)
   {
      if(occurance[i] != NULL) size++;
   }
   usize = (uint32_t)size;
   write(fd2, &usize, sizeof(uint32_t));
   for(i = 0; i < 256; i++)
   {
      if(occurance[i] != NULL)
      {
         *ch = (uint8_t)(occurance[i]->c);
         *cn = (uint32_t)(occurance[i]->count);
         write(fd2, ch, sizeof(uint8_t));
         write(fd2, cn, sizeof(uint32_t));
      }
   }
   close(fd);
   free(ch);
   free(cn);
}

void encode(int fd, int fd2, int codes[256][128])
{
   char buf[1];
   char c;
   int i;
   int h = 0;
   uint8_t ch = 0;
 
   while(read(fd, buf, 1) > 0)
   {
      c = (int)*buf;
      i = 0;
      while(codes[(int)c][i] != -1)
      {
         if(codes[(int)c][i] == 0)
         {
            ch = ch << 1;
            h++;
            if(h==8)
            {
               write(fd2, &ch, sizeof(uint8_t));
               h = 0;
            }
         }
         else if(codes[(int)c][i] == 1)
         {
            ch = ch << 1;
            ch = ch | 1;
            h++;
            if(h==8)
            {
               write(fd2, &ch, sizeof(uint8_t));
               h = 0;
            }
         }
         
         i++;
      }
   }   
   if(h < 8 && h != 0)
   {
      while(h < 8)
      {
         ch = ch << 1;
         h++;
      }
      write(fd2, &ch, sizeof(uint8_t));
   }
   close(fd);
   close(fd2);
}












int main(int argc, char **argv)
{
   Node **array_list;
   int codetable[256][128];
   int arr[128];
   int i;
   int x;
   int fd;
   int fd2;
   int size = 0;
   Node *huff = NULL;

   printf("HELLO");
   if((argc < 1) || (argc > 3))
   {
      fprintf(stderr, "usage: hencode infile [outfile]\n");
      return 1;
   }


   for(i = 0; i < 128; i++)
   {
      arr[i] = -1;
   }

   for(i = 0; i < 256; i++)
   {
      for(x = 0; x < 128; x++)
      {
         codetable[i][x] = -1;
      }
   }

   if(argc == 3)
   {
      if((fd = open(argv[1], O_RDONLY, 0)) < 0)
      {
         fprintf(stderr, "usage: fw");
         exit(0);
      }
      if((fd2 = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR)) > 0)
      {
         
         if(!(array_list = count_occurance(fd)))
         {
            exit(0);
         }
         for(i = 0; i < 256; i++)
         {
            if(array_list[i] != NULL) size++;
         }

         header(fd, fd2, array_list);
         huff = huffman_tree(array_list);
         if((fd = open(argv[1], O_RDONLY, 0)) < 0)
         {
            fprintf(stderr, "usage: fw");
            exit(0);
         }
         if(size == 0) 
         { 
            close(fd);
            close(fd2);
            return 1;
         }
         gen_codes(huff, arr, 0, codetable);
         encode(fd, fd2, codetable);
         free_tree(huff);
         free(array_list);
      }
      if(fd2 < 0)
      {
         free_tree(huff);
         free(array_list);
         fprintf(stderr, "usage");
         exit(0);
      }
   }
   return 0;
}
