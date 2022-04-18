#include <stdio.h>
#include <stdlib.h>
//se incluye el archivo de los metodos de las listas
#include "lista_eventos.h"
#include "cola_servidor.h"
struct node{
	int type;
	float clock;
	float numDeClock;
	struct node* next;
};
//Aqui se van a definir los metodos de la simulacion---------
//inicializa los valores de los clocks y crea la lista de eventos
void  inicializar(void);
//Tiene que avanzar el MC al menor tiempo de la lista de eventos y si el evento es de tipo 1 ejecutar funcion llegada y si es tipo 2 funcion salida
void avanzarTiempo(void);
//Debe meter un evento a la cola del servidor y crear un evento de tipo salida y meterlo a la lista, ademas de actualizar variable estado servirdor y cola
void llegada(void);
//Debe meter un evento de que se arruine la maquina que acaba de salir a la lista de eventos y eliminar el evento de la cola del servidor y actualizar variables
void salida(void);
//Va a imprimir datos
void imprimirDatos(void);
void imprimirEstadisticos(void);


//Aqui se van a definir las variables a utilizar-----
//Lista de eventos, este puntero siempre apunta al menor de la lista
struct node* lista_eventos;
//Cola del servidor, este puntero apunta al ultimo de la cola y hay que crear otros metodos para esta cola esta no saca en orden como la lista de eventos
//Sino que saca por orden de llegada
struct node* cola_servidor_tope;
struct node* cola_servidor_raiz;
//Este nodo se ocupa para eliminar de la lista de eventos y guarda el nodo que se saca de la lista
struct node* eliminar;
struct node* aux;
//Numero de eventos
int LLEGADA = 1, SALIDA = 2;
int OCUPADO =1, DESOCUPADO = 0;
int numberDelayed;
float cl[100],arruinar,reparar,mc,n;
int numCola=0, estadoServidor[20], atendiendo;
//Estos son los valores de los clocks o maquinas, el cl0 es el clock del servidor
//Esto servirira para un numero fijo pero si se pone que se pueda cambiar el numero de maquinas deberia cambiarse a un arreglo
int contadorM, contadorS, i,j;
FILE *outfile;


int main(int argc, char *argv[]) {
	outfile = fopen("inteferencia-maquinas.out", "w");
	inicializar();
	//Aqui debe haber un loop que esto se va a repetir hasta que mc sea 480
	while(mc<n){
		avanzarTiempo();	
	}
	imprimirEstadisticos();
	return 0;
}

//Lo que tiene que hacer este metodo es leer los valores iniciales aqui se los asigne automaticamente y crear la lista de eventos.
void  inicializar(void){
	//esta es la lista de eventos
	numberDelayed = 0;
	lista_eventos = NULL;
	//asigna los valores, si se tomaran en cuenta mas maquinas tendria que llenarse el arreglo
	printf("Ingrese el numero de maquinas: \n");
	scanf("%i", &contadorM);
	printf("Ingrese el numero de servidores: \n");
	scanf("%i", &contadorS);
	i = 0;
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			printf("Ingrese el valor del clock de la maquina %i: \n",(i+1));
			scanf("%f", &cl[i]);
		}else{
			
			printf("Ingrese el valor del clock del servidor %i: \n",(i+1-contadorM));
			scanf("%f", &cl[i]);
		}
		i = i+1;
	}
	printf("Ingrese el tiempo que tardan en fallar las maquinas: \n");
	scanf("%f", &arruinar);
	printf("Ingrese el tiempo que tardan en reparar los servidores: \n");
	scanf("%f", &reparar);
	printf("Ingrese el tiempo de la simulacion: \n");
	scanf("%f", &n);
	//se llena la lista de eventos
	i = 0;
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			lista_eventos = insertNode(lista_eventos,createNode(LLEGADA,cl[i],i));
		}
		i = i+1;
	}
	
	while(i<contadorS){
		estadoServidor[i] = DESOCUPADO;
		i = i+1;
	}
	numCola = 0;
	fprintf(outfile,"\n\tMC = Master Clock\n\tCL = Clocks de las maquinas\n\tServ = Servidor \n\tEst = Estado del servidor\n\tNumCola = Numero en cola\n");
	fprintf(outfile,"	MC	");
	i = 0;
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			fprintf(outfile,"CL%i	",(i+1));
		}else{
			fprintf(outfile,"Serv %i	",(i+1-contadorM));
		}
		i = i+1;
	}
	i = 0;
	while(i<contadorS){
		fprintf(outfile,"Est.%i  ", i+1);
		i = i+1;
	}
	fprintf(outfile,"NumCola\n");
	imprimirDatos();
	
}

void avanzarTiempo(void){
	eliminar = NULL;
	if (lista_eventos != NULL){
		mc = lista_eventos->clock;
		if(lista_eventos->type == LLEGADA){
			lista_eventos = removeNode(lista_eventos, &eliminar);
			llegada();
		}else{
			if(lista_eventos->type == SALIDA){
				lista_eventos = removeNode(lista_eventos, &eliminar);
				salida();
				
			}
		}
		imprimirDatos();
	}
}

void llegada(void){
	/*ay que crear los metodos de la cola en otros archivos asi como hice los de la lista de eventos lo unico que va a cambiar es que meta siempre 
	el que entra al final de la cola y saque el primero.
	poner un if que si la cola que esta definida arriba es null es porque no hay nadie en cola que cambie el estado del servidor a 1 y que programe el 
	cl0 con un evento de salida
	algo asi como
	cl0 = mc + reparar;
	lista_eventos = insertNode(lista_eventos,createNode(SALIDA,cl0,0));
	pero si cola_servidor no es null es que ya hay alguien entonces solo agregar a la cola y no programar la salida y actualizar las variables */
	int desocupados = 0;
	if(cola_servidor_raiz == NULL){
		insertNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,eliminar);
		numberDelayed = numberDelayed + 1;
		lista_eventos = insertNode(lista_eventos,createNode(SALIDA,mc+reparar,contadorM));
		cl[contadorM] = mc+ reparar;
		estadoServidor[0] = OCUPADO;
		atendiendo = 1;
		numCola = 0;
	}else{
		i=0;
		while(i<contadorS){
				if(estadoServidor[i] == DESOCUPADO){
					desocupados =1;
					break;
				}
				i = i+1;
			}
		if(desocupados == 1){
			i=0;
			while(i<contadorS){
					if(estadoServidor[i] == DESOCUPADO){
						insertNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,eliminar);
						numberDelayed = numberDelayed + 1;
						lista_eventos = insertNode(lista_eventos,createNode(SALIDA,mc+reparar,contadorM+i));
						cl[contadorM+i] = mc+ reparar;
						estadoServidor[i] = OCUPADO;
						atendiendo = atendiendo + 1;
						break;
					}
				i = i+1;
			}
		}else{
			insertNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,eliminar);
			numCola = numCola + 1;
		}
	}
	i=0;
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			if(eliminar->numDeClock == i){
			cl[i] = -1;
			}
		}
		i = i+1;
	}
}
void salida(void){
	int server = eliminar->numDeClock, aux;
	eliminar = NULL;
	if(cola_servidor_raiz != NULL){
			removeNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,&eliminar);
			lista_eventos = insertNode(lista_eventos,createNode(LLEGADA,mc+arruinar,eliminar->numDeClock));
			atendiendo = atendiendo -1;
			aux = server-contadorM;
			estadoServidor[aux] = DESOCUPADO;
			cl[server] = -1;
			if(numCola > 0){
				i=0;
				while(i<contadorS){
						if(estadoServidor[i] == DESOCUPADO){
							lista_eventos = insertNode(lista_eventos,createNode(SALIDA,mc+reparar,contadorM+i));
							cl[contadorM+i] = mc+ reparar;
							estadoServidor[i] = OCUPADO;
							atendiendo = atendiendo + 1;
							numberDelayed = numberDelayed + 1;
							break;
						}
					i = i+1;
				}
				numCola = numCola -1;
			}
			i=0;
			while(i<(contadorM+contadorS)){
				if(i<contadorM){
					if(eliminar->numDeClock == i){
					cl[i] = mc+arruinar;
					}
				}
				i = i+1;
			}
	}
}
void imprimirDatos(void){
	int a;
	/* Va a ir imprimiendo las variables clocks, el num en cola y el estado del servidor*/
		fprintf(outfile,"	%.1f\t",mc);
		i = 0;
		while(i<(contadorM+contadorS)){
			if(i<contadorM){
				if(cl[i] < 0){
					if(i == cola_servidor_raiz->numDeClock){
						fprintf(outfile,"ppnf\t");
					}else{
						if(atendiendo>1){
							aux = cola_servidor_raiz;
							j =0;
							while(j<(atendiendo)){
								if(i == aux->numDeClock){
									a = 1;
									break;
								}
								else{
									a = 0;
								}
								aux = aux->next;
							j = j+1;
							}
							if(a == 1){
								fprintf(outfile,"ppnf\t");
							}else{
								fprintf(outfile,"pps\t");
							}
						}else{
							fprintf(outfile,"pps\t");
						}
					}
				}else{
					fprintf(outfile,"%.1f\t",cl[i]);
				}
			}else{
				if(cl[i]<0){
					fprintf(outfile,"-\t	");
				}else{
					if(cl[i]>=1000){
					fprintf(outfile,"%.1f	",cl[i]);
					}else{
					fprintf(outfile,"%.1f\t  	",cl[i]);	
					}	
				}
			}
			i = i+1;
		}
			i = 0;
		while(i<contadorS){
			fprintf(outfile,"  %i\t",estadoServidor[i]);
			i = i+1;
		}
		fprintf(outfile,"\t%i\t\n",numCola);
}
void imprimirEstadisticos(void){
	fprintf(outfile,"\n\tNumber Delayed: %i",numberDelayed);
}
