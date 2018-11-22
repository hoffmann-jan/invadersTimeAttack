struct List
{
  struct Entity *Entity;
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