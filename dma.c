#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

//Have a global array of one MB
//Have a pagesize
#define PAGESIZE 4096 //or 2048, or 4096 ...

typedef struct chunkhead
{
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
} chunkhead;

unsigned char *mymalloc(unsigned int size);
void myfree(unsigned char *address);
void analyze();
chunkhead *head;
int heapsize = 0;
void *startofheap = NULL;
int main()
{


    // unsigned char *a, *b, *c, *d;
    // a = mymalloc(1001);
    // b = mymalloc(4500);
    // c = mymalloc(1000);
    // analyze();
    // myfree(c);
    // analyze();
    // myfree(b);
    // analyze();
    unsigned char *a[100];
    analyze(); //50% points
    for (int i = 0; i < 100; i++)
        a[i] = mymalloc(1000);
    for (int i = 0; i < 90; i++)
        myfree(a[i]);
    analyze(); //50% of points if this is correct
    myfree(a[95]);
    a[95] = mymalloc(1000);
    analyze(); //25% points, this new chunk should fill the smaller free one
               //(best fit)
    for (int i = 90; i < 100; i++)
        myfree(a[i]);
    analyze();
}

unsigned char *mymalloc(unsigned int size)
{
    int actualSize = size + sizeof(chunkhead);
    int allocate;
    if (actualSize < PAGESIZE)
    {
        allocate = PAGESIZE;
    }
    else if (actualSize % PAGESIZE != 0)
    {
        allocate = ((actualSize / PAGESIZE) + 1) * PAGESIZE;
    }
    else
    {
        allocate = actualSize;
    }
    chunkhead *current;
    current = head;
    if (heapsize != 0)
    {
        while ((((current->size) < size) || ((current->info) == 1)) && (current->next != 0))
        {
            current = (chunkhead *)current->next;
        }
    }
    if (heapsize == 0)
    {
        head = startofheap = sbrk(0);
        sbrk(allocate);
        heapsize = allocate;
        head->size = heapsize;
        head->info = 0;
        head->next = head->prev = 0;
        current = head;
    }
    if (current->size == allocate && current->info != 1)
    {
        current->info = 1;
        return (unsigned char *)++current;
    }
    else if (current->size > allocate && current->info != 1)
    {
        chunkhead *div = (chunkhead *)((unsigned char *)current + allocate);
        div->size = current->size - allocate;
        div->info = 0;
        div->next = current->next;
        div->prev = (unsigned char *)current;
        current->size = allocate;
        current->info = 1;
        current->next = (unsigned char *)div;
        return (unsigned char *)++current;
    }

    else if (current->info == 1)
    {
        chunkhead *div = sbrk(0);
        sbrk(allocate);
        heapsize += allocate;
        div->size = allocate;
        div->info = 1;
        div->next = 0;
        div->prev = (unsigned char *)current;
        current->next = div;

        return (unsigned char *)++div;
    }
    else
    {
        printf("Not enough memory to allocate\n");
        return 0;
    }
}
void myfree(unsigned char *address)
{
    if (head <= address)
    {
        chunkhead *current = (chunkhead *)address;
        --current;
        current->info = 0;
        current = head;
        while (current != 0)
        {
            if (current->info == 0 && current->next != 0 && ((chunkhead *)(current->next))->info == 0)
            {
                current->size += ((chunkhead *)(current->next))->size;
                current->next = ((chunkhead *)(current->next))->next;
            }
            current = (chunkhead *)current->next;
        }
    }
    else
    {
        printf("pointer not within heap\n");
    }
}
void analyze()
{
    printf("\n--------------------------------------------------------------\n");
    if (!startofheap)
    {
        printf("no heap");
        return;
    }
    chunkhead *ch = (chunkhead *)startofheap;
    for (int no = 0; ch; ch = (chunkhead *)ch->next, no++)
    {
        printf("%d | current addr: %x |", no, ch);
        printf("size: %d | ", ch->size);
        printf("info: %d | ", ch->info);
        printf("next: %x | ", ch->next);
        printf("prev: %x", ch->prev);
        printf("      \n");
    }
    printf("program break on address: %x\n", sbrk(0));
}