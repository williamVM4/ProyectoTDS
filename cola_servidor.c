//defini null porque no lo reconocia
#define NULL 0LL
//el nodo que tiene tipo que es llegada o salida, el clock que es el tiempo en que ocurre ese evento, numDeClock es el numero de clock al que pertenece
//y el puntero hacia el otro nodo
struct node{
	int type;
	float clock;
	float numDeClock;
	struct node* next;
};

//Inserta el nodo en la lista de menor a mayor
void insertNodeServidor(struct node** raiz, struct node** tope, struct node* newNodePtr){
	if ((*raiz) != NULL){
		(*tope)->next = newNodePtr;
		(*tope) = newNodePtr;
	}else{
		(*raiz) = newNodePtr;
		(*tope) = newNodePtr;
	}
	
}

//saca los nodos de la lista de menor a mayor y guarda la direccion en el puntero que se le da como segundo parametro
void removeNodeServidor(struct node** raiz, struct node** tope,struct node** nextRef){
	if ((*raiz) != NULL){
		(*nextRef) = *(raiz);
		if ((*raiz) == (*tope)){
			(*raiz) = NULL;
            (*tope) = NULL;
		}
		else{
			(*raiz) = (*raiz)->next;
		}
	}

}
