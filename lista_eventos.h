//Declaracion de metodos de la lista de eventos
struct node* createNode(int type, float clock, float numDeClock);
struct node* insertNode(struct node* head, struct node* newNodePtr);
struct node* removeNode(struct node* head,struct node** nextRef);
