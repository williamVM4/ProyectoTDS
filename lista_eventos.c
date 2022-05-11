//defini null porque no lo reconocia
#define NULL 0LL
//el nodo que tiene tipo que es llegada o salida, el clock que es el tiempo en que ocurre ese evento, numDeClock es el numero de clock al que pertenece
//y el puntero hacia el otro nodo
struct node{
	int type;
	float clock;
	float numDeClock;
	struct node* next;
	struct node* prev;
};

//Crea el nodo y le asigna los parametros
struct node* createNode(int type, float clock, float numDeClock){
	struct node* nodePtr = (struct node*) malloc (sizeof(struct node));
	nodePtr->type = type;
	nodePtr->clock = clock;
	nodePtr->numDeClock = numDeClock;
	nodePtr->next = NULL;
	nodePtr->prev = NULL;
	return nodePtr;
}

//Inserta el nodo en la lista de menor a mayor
struct node* insertNode(struct node* head, struct node* newNodePtr){
	if (head == NULL){
		return newNodePtr;
	}else{  
		if(head->clock > newNodePtr->clock){
			newNodePtr->next = head;
			head->prev = newNodePtr;
			return newNodePtr;
		}
		else{
			struct node* prev = NULL;
			struct node* curr = head;
			while((curr!=NULL) && (curr->clock <= newNodePtr->clock)){
				prev = curr; 
				curr= curr->next;
			}
			prev->next = newNodePtr;
			newNodePtr->next = curr;
			newNodePtr ->prev = prev;
			curr->prev=newNodePtr;
			return head;
		}
	}
}

//saca los nodos de la lista de menor a mayor y guarda la direccion en el puntero que se le da como segundo parametro
struct node* removeNode(struct node* head,struct node** nextRef){
	*nextRef = head;
 	if (head != NULL) {
		head = head->next;
	}
	(*nextRef)->next = NULL;
	return head;

}

