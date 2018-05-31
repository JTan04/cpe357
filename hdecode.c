#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "hdecode.h"
Node *new_node(unsigned char c, int count)
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
   if(s == 0)
   {
      return NULL;
   }
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






Node **read_header(int fd)
{
   unsigned char buf[1];
   unsigned char buf2[4];
   unsigned char c;
   int i;
   Node **array_list = (Node **)malloc(sizeof(Node) * 256);
   int count = 0;
   int count_uniq = 0;

   for(i = 0; i < 256; i++)
   {
      array_list[i] = NULL;
   }

   if(read(fd, buf, 1) > 0)
   {
      count_uniq = (int)*buf;
   }
   
   for(i = 0; i < 3; i++)
   {
      read(fd, buf, 1);
   }
      
   for(i = 0; i < count_uniq; i++)
   {
      if(read(fd, buf2, 1) > 0)
      {
         c = (unsigned char)*buf2;
      }
      if(read(fd, buf2, 4) > 0)
      {
         count = (int)*buf2;
      }
      array_list[(unsigned int)c] = new_node(c, count);
   }
   return array_list;
}

void decode(int fd, int fd2, Node *huff)
{
   char buf[1];
   unsigned char c;
   char ch;
   int i;
   int b;
   unsigned int mask;
   int h = 0;
   Node *curr = huff;
   int total = curr->count;
   printf("%d\n", curr->count);

   if(huff == NULL)
   {
      exit(1);
   }

   if((b = read(fd, buf, 1)) == 0)
   {
      for(i = 0; i < total; i++)
      {
         write(fd2, &(huff->c), sizeof(char));
      }
   }


   while(b  > 0)
   {
      c = (unsigned char)*buf;
      for(i = 0; i < 8; i++)
      {
         mask = c >> 7;


         if((mask & 1) == 1)
         {
            curr = curr->right;
            if(!(curr->right) && !(curr->left))
            {
               ch = curr->c;
               write(fd2, &ch, sizeof(char));
               curr = huff;
               h++;
            }
         }
         else if((mask & 1) == 0)
         {
            curr = curr->left;
            if(!(curr->right) && !(curr->left))
            {
               ch = curr->c;
               write(fd2, &ch, sizeof(char));
               curr = huff;
               h++;
            }
         }
         c = c << 1;
         if(h >= total)
         {
            break;
         }
      }
      b = read(fd, buf, 1);
   }
}



int main(int argc, char **argv)
{
   Node **array_list;
   int fd;
   int fd2;
   Node *huff = NULL;
   if(argc == 3)
   {
      if((fd = open(argv[1], O_RDONLY, 0)) < 0)
      {
         fprintf(stderr, "usage: fw");
         exit(0);
      }
      array_list = read_header(fd);
      huff = huffman_tree(array_list);
      if((fd2 = open(argv[2], O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, S_IRUSR)) > 0)
      {
          decode(fd, fd2, huff);
          close(fd2);
      }
      close(fd);
      free_tree(huff);
      free(array_list);

   }
   return 0;
}
