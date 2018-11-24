#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "List.h"
#include "Entity.h"
#include "Position.h"

struct List* AddElement(struct List *ListElement, struct List *newElement)
{
  if (ListElement == NULL)
  {
    newElement->Previous = NULL;
    newElement->Next = NULL;
    return newElement;
  }
  else
  {
    ListElement = GetLastElement(ListElement);
    ListElement->Next = newElement;
    ListElement->Next->Previous = ListElement;
    ListElement->Next->Next = NULL;
  }

  return ListElement;
}

struct List* InsertElement(struct List *ListElement, struct List *newElement)
{
  if (ListElement == NULL)
  {
    newElement->Previous = NULL;
    newElement->Next = NULL;
    return newElement;
  }
  else
  {
    ListElement = GetFirstElement(ListElement);
    ListElement->Previous = newElement;
    ListElement->Previous->Next = ListElement;
    ListElement->Previous->Previous = NULL;
  }

  return ListElement;
}

struct List* RemoveAndDestroyElement(struct List *removeElement)
{
    struct List *returnElement = NULL;
  /* element in the middle of List */
  if (removeElement->Next != NULL && removeElement->Previous != NULL)
  {
    struct List *leftElement = removeElement->Previous;
    struct List *rightElement = removeElement->Next;

    leftElement->Next = rightElement;
    rightElement->Previous = leftElement;

    returnElement = rightElement;
  }
  /* element stand alone */
  else if (removeElement->Next == NULL && removeElement->Previous == NULL)
  {
    returnElement = NULL;
  }
  /* is first element */
  else if (removeElement == GetFirstElement(removeElement))
  {
    struct List *rightElement = removeElement->Next;
    rightElement->Previous = NULL;

    returnElement = rightElement;
  }
  /* is last element */
  else if (removeElement == GetLastElement(removeElement))
  {
    struct List *leftElement = removeElement->Previous;
    leftElement->Next = NULL;

    returnElement = leftElement;
  }

  free(removeElement->Entity->Position);
  free(removeElement->Entity);
  free(removeElement);
  return returnElement;
}

struct List* AllocFullListElement()
{
  struct List * listElement = (struct List*)malloc( sizeof(struct List));
  struct Entity *entity =  (struct Entity*)malloc( sizeof(struct Entity));
  struct Position *position =  (struct Position*)malloc( sizeof(struct Position));

  /* link */
  listElement->Entity = entity;
  listElement->Entity->Position = position;

  return listElement;
}


struct List* GetFirstElement(struct List *ListElement)
{
  if (ListElement == NULL)
    return NULL;
  while (ListElement->Previous != NULL)
  {
    ListElement = ListElement->Previous;
  }
  return ListElement;
}

struct List* GetLastElement(struct List *ListElement)
{
  if (ListElement == NULL)
    return NULL;
  while (ListElement->Next != NULL)
  {
    ListElement = ListElement->Next;
  }
  return ListElement;
}

int GetListCount(struct List *ListElement)
{
  if (ListElement == NULL)
    return 0;

  ListElement = GetFirstElement(ListElement);

  int count = 1;
  while (ListElement->Next != NULL)
  {
    ListElement = ListElement->Next;
    count++;
  }

  return count;
}

