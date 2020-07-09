#define cant_operadores 2
#include "ColaEstatica.h"
#include <avr/wdt.h>
#include <math.h>
#define SEGUNDOS 10

typedef struct {
  int led;
  int ledstate;
  int leds[4];
  double tiempo;
  Tcola cola;
  double llamada;
  int plibre_led;
} Toperador;
/*registro Toperador que va a tener todas las variables de cada uno de los operadores, led es del numero de led que le corresponde en la 
placa arduino, ledstate indica el estado del led, actuaria como un "booleano": 1 para HIGH (prendido), 0 para LOW (apagado)*/

/*definimos aca las funciones que despues vamos a usar en el algoritmo*/
void ponerllamada (Toperador *op, double duracion);
void ElegirOperador(Toperador operadores[cant_operadores], int* destino);
void InicializarOperadores(Toperador operadores[cant_operadores]);
void actualizar(Toperador operadores[cant_operadores]);



/*Definimos un arreglo de cant_operadores que en este caso es igual a dos del tipo Toperador (es decir, que va a ser un registro)*/
Toperador operadores[cant_operadores];

const int boton = 4;
double reloj = 0;
int destino;
/*Con destino nos referimos al operador al cual va a ser destinada la llamada: 0 para operador 1 y 1 para operador 2 (los arreglos empiezan en posicion 0 en C)*/
double duracion=0;
int i;
int j;


double buttonState = 0;
double lastButtonState = 0; 
double startPressed = 0;    
double endPressed = 0;
double millisact;
/*Buttonstate estado del boton actualmente LOW o HIGH, LastButtonState el ultimo estado del boton, StartPressed almacena el momento en el cuál comenzo a presionarse el botón*/      


void setup() {
/*Se le asignan a cada operador uno de los dos leds amarillos que van a estar en los pines 2 y 3*/
operadores[0].led = 2;
operadores[1].led = 3;

/*Inicializacion de los leds de las colas que son cuatro leds rojas para cada uno*/
int cont=5;
for(i=0; i<=1; i++){
  for(j=0; j<=3; j++){
    operadores[i].leds[j]=cont;
    cont++;
}}
/*Se declara el boton como pin de salida*/
pinMode(boton, INPUT);
InicializarOperadores(operadores);

  
}


void loop() {

/*Milisegundos actuales*/
millisact = millis();
/*Si los milisegundos actuales desde que comienza el loop es mayor al tiempo de reloj que habia hasta el momento, al restarlo me deberia dar mayor o igual a 1 segundo*/
if (millisact - reloj >= 1000) {
   actualizar(operadores); /**/
   reloj = millisact; /*actualizo el tiempo de reloj para la proxima vez que se ejecute el loop*/
   for (i=0; i<=cant_operadores-1; i++){
    /*Si hay llamadas en espera tiempo es mayor a 0, la led del operador correspondiente a ese tiempo de espera tiene que titilar*/
    if(operadores[i].tiempo > 0){
      if(operadores[i].ledstate == LOW){
        operadores[i].ledstate = HIGH;
      }else{
        operadores[i].ledstate = LOW;
      }
      digitalWrite(operadores[i].led, operadores[i].ledstate);/*cambio el estado del led*/
    }
   }

}


/*Me aseguro que las dos colas no esten llenas al mismo tiempo y entonces ahí acepto otra llamada recibida a través de la presión del botón*/
if ((!cllena(&operadores[0].cola)) || (!cllena(&operadores[1].cola))){
  buttonState = digitalRead(boton); /*Leo el estado del boton actualmente y lo almaceno en buttonState*/
/*si el estado del botón actualmente es distinto al estado del botón anterior entonces pregunto si el estado del botón es igual a HIGH (comenzó a ser presionado)y 
 se almacena el tiempo en el que se empezó a presionar o igual a LOW(se dejo de presionar) y se almacena el tiempo en el que se finalizo la presión del botón y con
 todo eso se calcula la duración total de la presión del botón, es decir, la duración total de la nueva llamada*/
  if (buttonState != lastButtonState) {
      if (buttonState == HIGH) {
          startPressed = millis();
      }else{
       endPressed = millis();
       duracion = (endPressed - startPressed)/1000 * SEGUNDOS;
      }
  }
/*Para evitar errores en las restas de tiempo tratamos de asegurarnos que nos de valores enteros, caso contrario convertir ese tiempo en un entero calculando la parte entera*/
  if (duracion>0){
    if (duracion < 1){
      duracion=0;
    }else{
      duracion=ceil(duracion);
    }
    if (duracion > 0){
      ElegirOperador(operadores, &destino);
      ponerllamada(&operadores[destino], duracion);
      duracion=0; /*vuelvo a inicializar la duración para la proxima llamada que se reciba*/
   }
  }
  


  lastButtonState = buttonState; /*Almaceno el estado actual del botón en lastButtonState actualizando el valor de este*/

}
}



void ElegirOperador(Toperador operadores[cant_operadores], int* destino){
  /*Si el segundo operador tiene la cola llena el destino es igual a cero, es decir, ira al primer operador*/
  if(cllena(&operadores[1].cola)){
    *destino = 0;
  }else{ /*Sino pregunto si el primer operador tiene la cola llena, caso en el cual destino igual 1, ira al segundo operador*/
    if(cllena(&operadores[0].cola)){
      *destino = 1;
    }else{    /*Si ninguno de los operadores tiene la cola llena, verifico cuál de los dos tiene menos tiempo de espera y ése será el que recibirá la llamada*/  
      if (operadores[0].tiempo <= operadores[1].tiempo){
        *destino = 0;
      }else{
        *destino = 1;
      }
    }
  }
  
}


void actualizar(Toperador operadores[cant_operadores]){

 for(int i=0; i<cant_operadores; i++){
  /*si el tiempo de espera del operador es mayor a cero, resto un segundo al tiempo de espera y al mismo tiempo al tiempo de llamada actual*/
  if (operadores[i].tiempo > 0)
    operadores[i].tiempo--;
    operadores[i].llamada--;
    /*si la llamada actual queda en cero o menos al restarle 1 segundo, asegurandome que en la cola haya llamadas en 
    espera, saco una llamada de la cola y la almaceno en la variable llamada del operador en cuestión  y a su vez le resto uno al plibre_led indicando cual va a ser el nuevo 
    primer led disponible para ser ocupado y apago la luz del led en cuestion para indicar que ahora hay un espacio nuevo en la cola*/
    if (operadores[i].llamada <= 0){
      if (!cvacia(&operadores[i].cola)){
        csacar(&operadores[i].cola, &operadores[i].llamada);
        operadores[i].plibre_led--;
        digitalWrite(operadores[i].leds[operadores[i].plibre_led], LOW);
        }else{
        /*El operador está libre, la cola esta vacía, el led amarillo deja de titilar y se mantiene encendido*/
        digitalWrite(operadores[i].led, HIGH);
      }
    }
  }
}



void ponerllamada (Toperador *op, double duracion){
  /*si el tiempo de espera total del operador es igual a 0 quiere decir que el operador esta libre, la llamada es atendida instantaneamente por el operador y, por lo tanto, no se encola*/
  if (op->tiempo == 0){
    op->llamada = duracion;
  }else{ /*Si hay tiempo de espera, mientras la cola no este llena, indico que el primer led que estaba disponible (primera posicion de la cola disponible sin ocupar) se prenda e incremento plibre_led en 1 
    indicando cual es el proximo led a encenderse*/
    if (!cllena(&op->cola)){
      cponer(&op->cola, duracion);
      digitalWrite(op->leds[op->plibre_led], HIGH);
      op->plibre_led++;
    }
  }
  op->tiempo = op->tiempo + duracion; /*Aumento el tiempo de espera del operador*/
}


/*inicializo los operadores creando la pila, poniendo en 0 el tiempo de llamada de espera y la llamada actual, Plibre_led primera posición de la cola libre seteo el led amarillo como de 
salida y ledstate HIGH indicando que comienza prendido al iniciar el programa indicando que los operadores al principio estan libres y seteo los cuatro leds rojos de la 
cola de espera como leds de salida*/
void InicializarOperadores(Toperador operadores[cant_operadores]){
  for (int i=0; i<=cant_operadores - 1; i++){
    ccrear(&operadores[i].cola);
    operadores[i].tiempo = 0;
    operadores[i].llamada = 0;
    operadores[i].plibre_led = 0;
    pinMode(operadores[i].led, OUTPUT);
    operadores[i].ledstate = HIGH;
    digitalWrite(operadores[i].led, operadores[i].ledstate);
    for (int j=0; j<=3; j++){
     pinMode(operadores[i].leds[j], OUTPUT);
    }
  }
}
