#include <stdio.h>
#include <stdlib.h>
/*se incluye el archivo de los metodos de la lista y de la cola*/
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

/*Aqui se van a definir las variables a utilizar-----*/
//Lista de eventos, este puntero siempre apunta al menor de la lista
struct node* lista_eventos;
//Cola del servidor, este puntero apunta al ultimo de la cola y hay que crear otros metodos para esta cola esta no saca en orden como la lista de eventos
//Sino que saca por orden de llegada
struct node* cola_servidor_tope;
struct node* cola_servidor_raiz;
//Este nodo se ocupa para eliminar de la lista de eventos y guarda el nodo que se saca de la lista
struct node* eliminar;
struct node* aux;
//Constantes a utilizar
int LLEGADA = 1, SALIDA = 2;
int OCUPADO =1, DESOCUPADO = 0;
/*Variables que se van a utilizar en la impresion y para controlar la simulación-----*/
int numberDelayed;
float cl[100],arruinar,reparar,mc,n;
int numCola=0, estadoServidor[20], atendiendo;
/*Variables para guardar el numero de Maquinas, servidores y contadores auxiliares-----*/
int contadorM=0, contadorS=0, i,j;
FILE *outfile;


int main(int argc, char *argv[]) {
	outfile = fopen("inteferencia-maquinas.out", "w");
	inicializar();
	//Ciclo que controla el tiempo de la ejecucion
	while(mc<n){
		avanzarTiempo();	
	}
	imprimirEstadisticos();
	return 0;
}

/*Lee los valores iniciales ingresados por consola y crea la lista de eventos.-----*/
void  inicializar(void){
	//Se inicializan las variables a 0
	numberDelayed = 0;
	numCola = 0;
	lista_eventos = NULL;
	mc = 0;
	//Lee el numero de maquinas y de servidores en las variables contador
	while(contadorM<=0){
		printf("Ingrese un numero de maquinas: \n");
		scanf("%i", &contadorM);	
	}
	while(contadorS<=0){
	printf("Ingrese un numero de servidores: \n");
	scanf("%i", &contadorS);
	}
	i = 0;
	//Llena el vector cl con las maquinas primero y despues los servidores
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			printf("Ingrese el valor del clock de la maquina %i en minutos: \n",(i+1));
			scanf("%f", &cl[i]);
		}else{
			
			printf("Ingrese el valor del clock del servidor %i en minutos: \n",(i+1-contadorM));
			scanf("%f", &cl[i]);
		}
		i = i+1;
	}
	//Lee los valores de tiempo de fallo de maquinas y tiempo de reperacion de servidores
	while(arruinar<=0){
		printf("Ingrese el tiempo que tardan en fallar las maquinas en minutos: \n");
		scanf("%f", &arruinar);
	}
	while(reparar<=0){
		printf("Ingrese el tiempo que tardan en reparar los servidores en minutos: \n");
		scanf("%f", &reparar);
	}
	//Lee el valor del tiempo de la simulacion en minutos
	while(n<=0){
		printf("Ingrese el tiempo de la simulacion en min: \n");
		scanf("%f", &n);
	}
	//se llena la lista de eventos
	i = 0;
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			lista_eventos = insertNode(lista_eventos,createNode(LLEGADA,cl[i],i));
		}
		i = i+1;
	}
	//Los servidores se inicializan en desocupados
	while(i<contadorS){
		estadoServidor[i] = DESOCUPADO;
		i = i+1;
	}
	//Se imprimien las cabeceras de la tabla
	fprintf(outfile,"\tSimulación de modelo interferencia de Máquinas.");
	fprintf(outfile,"\n\tAutores:\n\t\tChicas Duarte, Herbert Daniel - CD17008\n\t\tMiguel Chinchilla, Benneth Daniel - MC15048\n\t\tMenjivar Sandoval, Delmy Stefany - MS12016\n\t\tPortillo Ventura, Kevin Enrique - PV17028\n\t\tVásquez Mancia, William Enrique - VM19003\n");
	fprintf(outfile,"\n\tMC = Master Clock\n\tCL = Clocks de las maquinas\n\tServ = Servidor i\n\tEst = Estado del servidor i\n\tNumCola = Numero en cola del sistema\n\tNumSistema = Numero total en el sistema\n\n");
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
	fprintf(outfile,"NumCola\tNumSistema\n");
	//Se imprimen los valores inciales
	imprimirDatos();
	
}
/*Avanza el master clock hasta el menor valor de clock de la lista de eventos.-----*/
void avanzarTiempo(void){
	eliminar = NULL;
	if (lista_eventos != NULL){
		mc = lista_eventos->clock;
		//Si el evento es de tipo llegada ejecuta el metodo llegada y si es salida ejecuta el metodo salida
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

/*Ingresa el evento en la cola del servidor, si hay servidores desocupados empieza a ser atendido y programa el evento de salida de esa maquina.-----*/
void llegada(void){
	//Variable auxiliar para verificar si hay servidores desocupados
	int desocupados = 0;
	//Verifica si la cola esta vacia
	if(cola_servidor_raiz == NULL){
		//Inserta el evento en la cola, empieza a ser atendido y programa su salida
		insertNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,eliminar);
		numberDelayed = numberDelayed + 1;
		lista_eventos = insertNode(lista_eventos,createNode(SALIDA,mc+reparar,contadorM));
		//Actualiza el valor del servidor y cambia su estado, ademas de las otras variables
		cl[contadorM] = mc+ reparar;
		estadoServidor[0] = OCUPADO;
		atendiendo = 1;
		numCola = 0;
	}else{
		//Verifica si hay servidores desocupados
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
					//Busca el servidor desocupado
					if(estadoServidor[i] == DESOCUPADO){
						//Inserta el evento en cola pero empieza a ser atendido, se programa su salida y se actualizan varoles del servidor
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
			//Si hay eventos en cola y los servidores estan en cola solo se agrega y se aumenta el numero en cola
			insertNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,eliminar);
			numCola = numCola + 1;
		}
	}
	i=0;
	//Cuando una maquina esta arruinada se le asigna un valor de -1 para manejar la salida.
	while(i<(contadorM+contadorS)){
		if(i<contadorM){
			if(eliminar->numDeClock == i){
			cl[i] = -1;
			}
		}
		i = i+1;
	}
}
/*Saca el evento de la cola del servidor, programa la siguiente llegada y si hay servidores desocupados y otras maquinas en cola empieza a atender al siguiente.-----*/
void salida(void){
	int server = eliminar->numDeClock, aux;
	eliminar = NULL;
	//Verifica si la cola no esta vacia
	if(cola_servidor_raiz != NULL){
		//Remueve el nodo de la cola y programa su siguiente llegada
			removeNodeServidor(&cola_servidor_raiz,&cola_servidor_tope,&eliminar);
			lista_eventos = insertNode(lista_eventos,createNode(LLEGADA,mc+arruinar,eliminar->numDeClock));
			atendiendo = atendiendo -1;
			aux = server-contadorM;
			//Se cambia el estado del servidor a desocupado y se le asigna un valor de -1 para manejar la salida
			estadoServidor[aux] = DESOCUPADO;
			cl[server] = -1;
			//Verifica si hay eventos en cola
			if(numCola > 0){
				i=0;
				while(i<contadorS){
					//Verifica si hay servidores desocupados
						if(estadoServidor[i] == DESOCUPADO){
							//Empieza a atenderlos y programa su salida
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
			//Actualiza el valor del clock de la maquina que acaba  de salir del servidor
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
/*Imprime los datos de los clocks de maquinas y serviores en forma de tabla en un documento txt externo-----*/
void imprimirDatos(void){
	if(mc<=n){
	int a;
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
		fprintf(outfile,"\t%i\t\t%i\t\n",numCola,numCola+atendiendo);
	}
}
/*Imprime los datos de los estadisticos en un documento txt externo-----*/
void imprimirEstadisticos(void){
	fprintf(outfile,"\n\t-----Estadisticos -----");
	fprintf(outfile,"\n\tNumber Delayed: %i",numberDelayed);
}
