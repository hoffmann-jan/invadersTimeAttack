#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "List.h"
#include "Position.h"
#include "Invader.h"

struct List
{
  struct Invader *Invader;
  struct List *Next;
  struct List *Previous;
};

struct List* AddElement(struct List *ListElement, struct List *newElement);
struct List* AllocFullListElement();
struct List* InsertElement(struct List *ListElement, struct List *newElement);
struct List* RemoveAndDestroyElement(struct List *removeElement);
struct List* GetFirstElement(struct List *ListElement);
struct List* GetLastElement(struct List *ListElement);
int GetListCount(struct List *ListElement);


#endif