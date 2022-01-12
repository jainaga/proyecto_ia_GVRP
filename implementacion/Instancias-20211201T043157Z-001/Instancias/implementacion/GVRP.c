#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


typedef struct  parametros
{
    char nombre_instancia[6];
    int clientes;
    int estaciones;
    int tiempoMaximo; //en minutos 
    int distanciaMaxima; //en millas
    double velocidad;
    int tiempoServicio; //minutos
    int tiempoRecarga; //minutos
} parametros;

typedef struct nodo
{
    int nodeID;
    char type;
    double longitud;
    double latitud;
    int estado; //1 si esta visitado
} nodo;

typedef struct tupla
{
    int nodeId;
    char type;
} tupla;

typedef struct valoresRuta
{
    float distanciaDeRuta;
    float tiempoDeRuta;
} valoresRuta;

typedef struct hill
{
    int nodeId;
    char type;
    int skip;
} hill;





//globales para funcion void getinfo y guardar los datos de la instancia
parametros* para;
nodo* nodos;
nodo* ultimonodo;
nodo* loops;
valoresRuta* valoresRutas;
tupla* rutas;
int mejoropcionid;
int contcliente;
int* largoRutas;

int indicadorloops = 0;

void getinfo(FILE* file){
    char linea[100];
    //se obtiene la primera linea con los parametros
    fgets(linea,100,file);
    char *sep = strtok(linea, " ");
    //separa por los espacios, empezamos a guardar los parametros

    sscanf(sep,"%s",(para->nombre_instancia));
    //printf("%s\n",(para->nombre_instancia));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->clientes));
    //printf("%d\n",(para->clientes));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->estaciones));
    //printf("%d\n",(para->estaciones));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->tiempoMaximo));
    //printf("%d\n",(para->tiempoMaximo));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->distanciaMaxima));
    //printf("%d\n",(para->distanciaMaxima));
    sep = strtok(NULL, " ");
    sscanf(sep,"%lf",&(para->velocidad));
    //printf("%lf\n",(para->velocidad));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->tiempoServicio));
    //printf("%d\n",(para->tiempoServicio));
    sep = strtok(NULL, " ");
    sscanf(sep,"%d",&(para->tiempoRecarga));
    //printf("%d\n",(para->tiempoRecarga));
    
    //se le la siguiente linea de deposito
    fgets(linea,100,file);
    //se pide la memoria para guardar los nodos
    nodos = (nodo*)malloc(sizeof(nodo)*(para->clientes + para->estaciones));
    
    sep = strtok(linea, " ");
    sscanf(sep,"%d",&(nodos[0].nodeID));    
    sep = strtok(NULL, " ");
    sscanf(sep,"%c",&(nodos[0].type));    
    sep = strtok(NULL, " ");
    sscanf(sep,"%lf",&(nodos[0].longitud));    
    sep = strtok(NULL, " ");
    sscanf(sep,"%lf",&(nodos[0].latitud));    
    //seteamos los estados a cero 
    nodos[0].estado = 0;
    //la primera estacion no la uso por eso se salta
    fgets(linea,100,file);

    for (size_t i = 1; i < (para->clientes + para->estaciones); i++){
        fgets(linea,100,file);
        sep = strtok(linea, " ");
        sscanf(sep,"%d",&(nodos[i].nodeID));    
        sep = strtok(NULL, " ");
        sscanf(sep,"%c",&(nodos[i].type));    
        sep = strtok(NULL, " ");
        sscanf(sep,"%lf",&(nodos[i].longitud));    
        sep = strtok(NULL, " ");
        sscanf(sep,"%lf",&(nodos[i].latitud));
        nodos[i].estado = 0;
        //printf("%d %c %lf %lf\n", (nodos[i].nodeID), (nodos[i].type), (nodos[i].longitud), (nodos[i].latitud));
    }
}

//aqui se implementa el calculo de las distancias en la tierra
double distanciaH(double lat1, double lat2, double long1, double long2){
    int radioTierra = 6371; //en kilometros
    double deltalat = (lat2 - lat1)*(M_PI/180);
    double deltalong = (long2 - long1)*(M_PI/180);
    double a = pow((sin(deltalat/2)),2) + cos(lat1*(M_PI/180)) * cos(lat2*(M_PI/180)) * pow((sin(deltalong/2)),2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    return (radioTierra * c);
}
#define transaKM  1.60934
//implementacion de greedy, almacena las rutas en una variable global 
//no se le pasa nada porque nodos y parametros es global
void greedy(){
    FILE *f = fopen("rutasGreedy.txt", "w");
    ultimonodo =(nodo*)malloc(sizeof(nodo));
    loops =(nodo*)malloc(sizeof(nodo)*(para->estaciones));
    valoresRutas = (valoresRuta*)malloc(sizeof(valoresRuta)*(para->clientes));
    nodo* masCercano = (nodo*)malloc(sizeof(nodo));
    largoRutas = (int*)malloc(sizeof(int)*para->clientes);
    contcliente = 0;
    int contadorRutas = 0;
    float totaldistancia = 0;
    float totaltiempo = 0;
    //arreglo de rutas 
    ///////////////////////////////////////rutas = (tupla*)malloc(sizeof(tupla))   ver esto 
    int indicadorRutas = 0;
    
    while (1)
    {
        //declarar arreglo ruta
        tupla* ruta = (tupla*)malloc(sizeof(tupla)*(para->clientes + para->estaciones));
        float distanciamascorta = 100000; //valor aleatoriamente grande
        masCercano->estado = 2; //default de que no tiene nodo asignado 
        //i empieza en 1 ya que el deposito no es posible visita
         //se asignan valores iniciales de las rutas
        float tiempoActual = 0;
        float distanciaActual = 0;
        double estanque = para->distanciaMaxima * transaKM;
        for (size_t i = 1; i < (para->clientes + para->estaciones); i++)
        {
           
            
            
            //ahora se ve si el nodo es posible candidato de partida
            //en vez de lista estacionesEvitar, se marcara al atributo estado de la estacion si es no visitable
            //por lo que solo hay que ver el estado del nodo
            if (nodos[i].estado == 0){
        
                
                if ((distanciaH(nodos[0].latitud, nodos[i].latitud, nodos[0].longitud, nodos[i].longitud)*2) <= estanque){
                    //aqui aseguramos que el estanque da para ir y volver al nodo
                    //ahora se ve que cumpla con la condicion de tiempo maximo
                    //primero vemos el caso de que sea estacion

                    //vemos la mejor opcion 
                    if (distanciaH(nodos[0].latitud, nodos[i].latitud, nodos[0].longitud, nodos[i].longitud) <= distanciamascorta)//modificar esto 
                        {

                            //ver limites de tiempo para ambos tipos
                            if (nodos[i].type == 'f')
                            {
                                if (tiempoActual + (distanciaH(nodos[0].latitud, nodos[i].latitud, nodos[0].longitud, nodos[i].longitud)/para->velocidad)*2 +
                                    para->tiempoRecarga <= para->tiempoMaximo)
                                {
                                    goto agregarprimero;
                                }
                            }else{
                                if (tiempoActual + (distanciaH(nodos[0].latitud, nodos[i].latitud, nodos[0].longitud, nodos[i].longitud)/para->velocidad)*2 +
                                    para->tiempoServicio <= para->tiempoMaximo)
                                    {
                                        agregarprimero: 
                                           //ver si es opcion mas cercana
                                        distanciamascorta = distanciaH(nodos[0].latitud, nodos[i].latitud, nodos[0].longitud, nodos[i].longitud);
                                        
                                        masCercano->nodeID = nodos[i].nodeID;
                                        masCercano->type = nodos[i].type;
                                        masCercano->latitud = nodos[i].latitud;
                                        masCercano->longitud = nodos[i].longitud;
                                        masCercano->estado = nodos[i].estado;
                                        mejoropcionid = i;

                                    }
                            }

                         
                        }
                    

                    //aqui ya se agrego el primer nodo
                    //seguimos con los demas nodos 
                    //implementacion de funcion auxiliar del escrito 

                    //ver el caso en que no agregue primer nodo si es que  
                }
            }
            
            
        }
        //ver si hay masCercano, si hay se agrega y sigue, si no se termina el greedy
        if (masCercano->estado == 2)
        {
            //no existen mas opciones para empezar 
            fclose(f);
            printf("Cantidad clientes visitados: %d de %d\n", contcliente, para->clientes);
            
            
            //printf("%f %f\n", totaltiempo, totaldistancia);
            
            return;
        }else{
            if (masCercano->type == 'f')
        {
            if (tiempoActual + (distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud)/para->velocidad)*2 +
                para->tiempoRecarga <= para->tiempoMaximo)
            {
                //cumple se agrega como primer nodo despues del deposito
                tiempoActual = tiempoActual + (distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud)/para->velocidad)
                + para->tiempoRecarga;
                

                distanciaActual = distanciaActual + distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud);
                estanque = para->distanciaMaxima * transaKM;
                //agregar a lista ruta junto con deposito 
                ruta[0].nodeId = nodos[0].nodeID;
                ruta[0].type = nodos[0].type;
                ruta[1].nodeId = masCercano->nodeID;
                ruta[1].type = masCercano->type;
                
                

                //esta variable indica en que posicion de ruta agregar las siguintes tuplas
                //int indicadorruta = 2;

                
                //crear y agregar a lista de loops
                //tener en cuenta que en caso de pasar a cliente se libera la lista
                //y de ser estacion ver si la lista existe en caso de que no se crea 
                
                //agregamos la estacion 
                loops[indicadorloops].nodeID = masCercano->nodeID;
                loops[indicadorloops].type = masCercano->type;
                loops[indicadorloops].latitud = masCercano->latitud;
                loops[indicadorloops].longitud = masCercano->longitud;
                loops[indicadorloops].estado = masCercano->estado;
                



                indicadorloops = indicadorloops + 1;
                ultimonodo->nodeID = masCercano->nodeID;
                ultimonodo->type = masCercano->type;
                ultimonodo->latitud = masCercano->latitud;
                ultimonodo->longitud = masCercano->longitud;
                ultimonodo->estado = masCercano->estado;
                
                
            }
            
        }else{//caso en que sea cliente
            if (tiempoActual + (distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud)/para->velocidad)*2 +
                para->tiempoServicio <= para->tiempoMaximo){
                    //cumple se agrega como primer nodo despues del deposito
                tiempoActual = tiempoActual + (distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud)/para->velocidad)
                + para->tiempoServicio;

                distanciaActual = distanciaActual + distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud);
                estanque = estanque - distanciaH(nodos[0].latitud, masCercano->latitud, nodos[0].longitud, masCercano->longitud);
                //agregar a lista ruta junto con deposito 
                ruta[0].nodeId = nodos[0].nodeID;
                ruta[0].type = nodos[0].type;
                ruta[1].nodeId = masCercano->nodeID;
                ruta[1].type = masCercano->type;
                
                

                //esta variable indica en que posicion de ruta agregar las siguintes tuplas
                //int indicadorruta = 2;

                //marcar cliente como visitado 
                masCercano->estado = 1;
                ultimonodo->nodeID = masCercano->nodeID;
                ultimonodo->type = masCercano->type;
                ultimonodo->latitud = masCercano->latitud;
                ultimonodo->longitud = masCercano->longitud;
                ultimonodo->estado = 1;
                nodos[mejoropcionid].estado = 1;
                }
            
        }

        }
        
        

        int indicadorruta = 2;
                    
        while(1){
            int cantNoFactibles = 0;
            //buscamos el nodo mas cercano al ultimo agregado que es nodo[i] = ultimonodo
            masCercano->estado = 2; //dos es su valor por defecto cuando no tiene nodos
            float distanciamascorta = 100000; //valor aleatoriamente grande
            for (size_t j = 1; j < (para->clientes + para->estaciones); j++)
            {
                if (nodos[j].nodeID != ultimonodo->nodeID && nodos[j].type != ultimonodo->type)
                {
                    //significa que los nodos no son el mismo 
                    //ver si es factible, si lo es se ve si se puede agregar a mejor opcion
                    //vemos si son factibles las distancias
                    if (distanciaH(nodos[j].latitud, ultimonodo->latitud, nodos[j].longitud, ultimonodo->longitud) + 
                    distanciaH(nodos[0].latitud, ultimonodo->latitud, nodos[0].longitud, ultimonodo->longitud)  <= estanque)
                    {
                        //fuera de nodos infactibles hasta el momento
                        //vemos si cumple con el tiempo dependiendo si es estacion o cliente
                        if (nodos[j].type == 'f')
                        {
                            if (tiempoActual + (distanciaH(nodos[0].latitud, nodos[j].latitud, nodos[0].longitud, nodos[j].longitud)/para->velocidad) +
                                (distanciaH(ultimonodo->latitud, nodos[j].latitud, ultimonodo->longitud, nodos[j].longitud)/para->velocidad)+
                                para->tiempoRecarga <= para->tiempoMaximo)
                            {
                                
                                
                                //ver queno este el loop
                                int esta = 0;
                                for (size_t x = 0; x < indicadorloops; x++)
                                {
                                    if (nodos[j].nodeID == loops[x].nodeID){
                                        esta = 1;
                                        //esto significa que el nodo si esta en loop por lo que se evita
                                    }
                                }
                                if (esta == 0){
                                        //si entra aca es opcion viable, ver si es la opcion mas corta 
                                    goto mascerca; //salta abajo para ver si es mas cercano, asi solo se escribe una vez
                                }
                                
                                
                            }
                            
                        }else if (nodos[j].type == 'c'){ //es cliente
                            if(nodos[j].estado != 1){
                                
                                if (tiempoActual + (distanciaH(nodos[0].latitud, nodos[j].latitud, nodos[0].longitud, nodos[j].longitud)/para->velocidad) +
                                (distanciaH(ultimonodo->latitud, nodos[j].latitud, ultimonodo->longitud, nodos[j].longitud)/para->velocidad)+
                                para->tiempoServicio <= para->tiempoMaximo)
                                {
                                    
                                    //si entra aca es opcion viable, ver si es la opcion mas corta 
                                    mascerca: //aca se ve si es el mas cercano 
                                    if (distanciaH(nodos[j].latitud, ultimonodo->latitud, nodos[j].longitud, ultimonodo->longitud) <= distanciamascorta)
                                        {
                                            //ver si es opcion mas cercana
                                        distanciamascorta = distanciaH(nodos[j].latitud, ultimonodo->latitud, nodos[j].longitud, ultimonodo->longitud);
                                        
                                        masCercano->nodeID = nodos[j].nodeID;
                                        masCercano->type = nodos[j].type;
                                        masCercano->latitud = nodos[j].latitud;
                                        masCercano->longitud = nodos[j].longitud;
                                        masCercano->estado = nodos[j].estado;
                                        mejoropcionid = j;

                                        


                                        }

                                }
                            }
                            
                        }
                        
                        

                    }
                    


                    
                    
                }
                
            }

            //aqui ya se tiene el nodo mas cercano o no en caso de que ninguno cumpla 
            //caso de tener nodo mas cercano 
            if (masCercano->estado != 2)
            {
                //existe nodo asignado 
                //ver que tipo de nodo es para ver como agregarlo
                
                if (masCercano->type == 'f')
                {
                    tiempoActual = tiempoActual + (distanciaH(masCercano->latitud, ultimonodo->latitud, masCercano->longitud, ultimonodo->longitud)/para->velocidad)
                    + para->tiempoRecarga;
                    
                    

                    distanciaActual = distanciaActual + distanciaH(masCercano->latitud, ultimonodo->latitud, masCercano->longitud, ultimonodo->longitud);
                    estanque = para->distanciaMaxima * transaKM;
                    
                    //agregar a lista ruta junto con deposito 
                    ruta[indicadorruta].nodeId = masCercano->nodeID;
                    ruta[indicadorruta].type = masCercano->type;

                    
                    //crear y agregar a lista de loops
                    //tener en cuenta que en caso de pasar a cliente se libera la lista
                    //y de ser estacion ver si la lista existe en caso de que no se crea 
                    //agregamos la estacion 
                    
                    loops[indicadorloops].nodeID = masCercano->nodeID;
                    loops[indicadorloops].type = masCercano->type;
                    loops[indicadorloops].latitud = masCercano->latitud;
                    loops[indicadorloops].longitud = masCercano->longitud;
                    loops[indicadorloops].estado = masCercano->estado;



                    indicadorloops = indicadorloops + 1;
                    
                    ultimonodo->nodeID = masCercano->nodeID;
                    ultimonodo->type = masCercano->type;
                    ultimonodo->latitud = masCercano->latitud;
                    ultimonodo->longitud = masCercano->longitud;
                    ultimonodo->estado = masCercano->estado;
            
                    //tener ojo aca si es que se paso la referencia y en caso de cambiar mas cercano cambie el valor de ultimo nodo
                }else if (masCercano->estado != 1){
                    //cliente
                    tiempoActual = tiempoActual + (distanciaH(masCercano->latitud, ultimonodo->latitud, masCercano->longitud, ultimonodo->longitud)/para->velocidad)
                    + para->tiempoServicio;
                    
                

                    distanciaActual = distanciaActual + distanciaH(masCercano->latitud, ultimonodo->latitud, masCercano->longitud, ultimonodo->longitud);
                    estanque = estanque - distanciaH(masCercano->latitud, ultimonodo->latitud, masCercano->longitud, ultimonodo->longitud);
                
                    //agregar a lista ruta junto con deposito 
                    ruta[indicadorruta].nodeId = masCercano->nodeID;
                    ruta[indicadorruta].type = masCercano->type;

                    //esta variable indica en que posicion de ruta agregar las siguintes tuplas
                    //int indicadorruta = 2;

                    //marcar cliente como visitado 
                    masCercano->estado = 1;
                    ultimonodo->nodeID = masCercano->nodeID;
                    ultimonodo->type = masCercano->type;
                    ultimonodo->latitud = masCercano->latitud;
                    ultimonodo->longitud = masCercano->longitud;
                    ultimonodo->estado = 1;
                    nodos[mejoropcionid].estado = 1;

                    
                    //ver lo de la referencia de los nodos
                    
                    //se reinicia el contador de loops
                    indicadorloops = 0;
                
                }

                indicadorruta = indicadorruta + 1;
                //se aumenta ya que se agrego un nodo
                

            }
            if (masCercano->estado == 2){
                //caso de no tener nodo mas cercano
            //en este caso el estado de masCercano es 2
            //como no hay mas cercano se agrega el deposito para finalizar ruta
            //se agrega la ruta a rutas 
            //se sale del while 
                indicadorloops = 0;
                
                ruta[indicadorruta].nodeId = nodos[0].nodeID;
                ruta[indicadorruta].type = nodos[0].type;
                tiempoActual = tiempoActual + (distanciaH(nodos[0].latitud, ultimonodo->latitud, nodos[0].longitud, ultimonodo->longitud)/para->velocidad);
                distanciaActual = distanciaActual + distanciaH(nodos[0].latitud, ultimonodo->latitud, nodos[0].longitud, ultimonodo->longitud);

                
                //hasta aqui se obtiene una ruta

                //caso de rutas de puras estaciones 
                int haycliente = 0;
                for (size_t q = 0; q < indicadorruta+1; q++)
                {
                    if (ruta[q].type == 'c')
                    {
                        haycliente = 1;
                    }
                        
                }
                //si hay cliente es 1 se guardan los valores
                //guardar ruta y valores
                //valores en malloc de estos donde la posicion calza con posicion de ruta 
                if (haycliente ==  1)
                {
                    //ruta valida
                    //guardamos valores
                    valoresRutas[indicadorRutas].distanciaDeRuta = distanciaActual;
                    valoresRutas[indicadorRutas].tiempoDeRuta = tiempoActual;

                    totaldistancia = totaldistancia + distanciaActual;
                    totaltiempo = totaltiempo + tiempoActual;
                
                    //se guarda ruta en un txt 
                    for (size_t q = 0; q < indicadorruta+1; q++)
                    {
                        if (ruta[q].type == 'c')
                        {
                            contcliente = contcliente +1;
                        }
                        
                        fprintf(f, "%d-%c,", ruta[q].nodeId,ruta[q].type);
                    }
                    fprintf(f, "%f,%f\n", tiempoActual,distanciaActual);

                    //agregamos valores a largoRutas
                    largoRutas[contadorRutas] = indicadorruta+1; //este es el largo de la ruta
                    contadorRutas = contadorRutas + 1;

                }else if (haycliente == 0)
                {
                    //puras estaciones, marcar primera con estado 1
                    for (size_t t = 0; t < (para->clientes+para->estaciones); t++)
                    {
                        if (nodos[t].nodeID == ruta[1].nodeId && nodos[t].type == ruta[1].type) //el tipo es estacion f
                        {
                            
                            //marcar estacion como visitada
                            nodos[t].estado = 1; //estacion visitada
                            
                        }
                        
                    }
                    
                }
                
                
                

                //si no se marca la primer nodo despues de deposito como visitado para no verlo, esto hay que cambiarlo al agregar el primer nodo para que agarre esta limitante
                
                //si no son puras estaciones se guarda la ruta y los valores de distancia y tiempo 

                //return; //esto es para probbar
                break;
                //aqui se sale del while que agrega nodos a la ruta para seguir con las demas rutas 
            }
            
        }
        
    }
    
}


void hillFI(){
    //leer archivo txt por linea hasta EOF
    FILE *archivo = fopen("rutasGreedy.txt","r");
    float totaldistanciah = 0;
    float totaltiempoh = 0;
    char linea[1000];
    char *sep;
    for (size_t i = 0; i < (para->clientes); i++){
        char* aux = fgets(linea,1000,archivo);
        if (aux == NULL)
        {

            //printf("%f %f\n", totaltiempoh, totaldistanciah);
            return; //ya se llego al final del archivo
        }


        hill* tuplasruta = (hill*)malloc(sizeof(hill)*largoRutas[i]);
        //separar la linea y guardarla en estructura hill
        sep = strtok(linea, ",");
        //separa la linea por comas

        //for para guardar nodos en hill
        for (size_t t = 0; t < largoRutas[i]; t++) //este for es por el largo de la ruta sin los valores de tiempo y distancia, estos estan en valoresRutas
        {
            sscanf(sep,"%d-%c",&(tuplasruta[t].nodeId), &(tuplasruta[t].type));
            sep = strtok(NULL, ",");
            
        }
        float valorantiguodist;
        float valorantiguotiem;
        sscanf(sep,"%f",&(valorantiguotiem));
        sep = strtok(NULL, ",");
        sscanf(sep,"%f",&(valorantiguodist));
        valoresRutas[i].distanciaDeRuta = valorantiguodist;
        valoresRutas[i].tiempoDeRuta = valorantiguotiem;

        //ya se almacena la ruta
        //ahora hill en esa ruta, una vez no se pueda mejorar se printea por pantalla y se sigue con la otra

        //puede que el while no sea necesario /////////////////////////////////
        //for para encontrar las posibles estaciones a sacar
        
        int cambio = 0 ;
        
        
        
        
        for (size_t m = 0; m < largoRutas[i]; m++)
        {
            float tiemponuevo = 0;
            float distancianueva = 0;
            if (tuplasruta[m].type == 'f')
            {
                //aqui ya tenemos estacion a verificar
                tuplasruta[m].skip = 1; //se marca para ser saltada al recorrer la ruta
                //ver si se puede hacer la ruta
                //ultimo nodo se determina como el deposito y el for parte de 1
                ultimonodo->nodeID = nodos[0].nodeID;
                ultimonodo->type = nodos[0].type;
                ultimonodo->latitud = nodos[0].latitud;
                ultimonodo->longitud = nodos[0].longitud;
                double estanque = para->distanciaMaxima * transaKM;
                for (size_t g = 1; g < largoRutas[i]; g++)
                {
                    //se recorre la ruta y vemos si es factible con el salto
                    //empieza de la primera visita 
                    //vemos si es skip o no, si lo es no pasa nada, si no se hace el calculo
                    if (tuplasruta[g].skip != 1) //no hay skip
                    {
                        //se hace el calculo con el anterior y se ve si cumple con estanque
                        //buscamos el nodo actual de la ruta en nodos para saver su posicion 
                        for (size_t d = 0; d < (para->clientes+para->estaciones); d++)
                        {
                            if (nodos[d].nodeID == tuplasruta[g].nodeId && nodos[d].type == tuplasruta[g].type)
                            {
                                
                                //encontramos el nodo, sabemos su posicion 
                                //hacemos los calculos con el ultimonodo
                                //vemos si da el estanque, si no da se vuelve a activar
                                if (distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud) <= estanque)
                                {
                                    //se cumple la distancia se actualizan valores dependiendo de que es el nodo
                                    distancianueva = distancianueva + distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud);
                                    if (tuplasruta[g].type == 'f')
                                    {
                                        /* printf("ultimo nodo %d %c %lf %lf\n", ultimonodo->nodeID = nodos[d].nodeID,
                                        ultimonodo->type = nodos[d].type,
                                        ultimonodo->latitud = nodos[d].latitud,
                                        ultimonodo->longitud = nodos[d].longitud); */
                                        tiemponuevo = tiemponuevo + (distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud)/para->velocidad) + para->tiempoRecarga;
                                        estanque = para->distanciaMaxima * transaKM;
                                    }if (tuplasruta[g].type == 'c')
                                    {
                                        tiemponuevo = tiemponuevo + (distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud)/para->velocidad) + para->tiempoServicio;
                                        estanque = estanque - distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud);
                                    }if (tuplasruta[g].type == 'd')
                                    {
                                        tiemponuevo = tiemponuevo + (distanciaH(ultimonodo->latitud,nodos[d].latitud,ultimonodo->longitud,nodos[d].longitud)/para->velocidad);
                                    }
                                    

                                    //guardamos como ultimonodo el nodo
                                    ultimonodo->nodeID = nodos[d].nodeID;
                                    ultimonodo->type = nodos[d].type;
                                    ultimonodo->latitud = nodos[d].latitud;
                                    ultimonodo->longitud = nodos[d].longitud;
                                    /* printf("ultimo nodo %d %c %lf %lf\n", ultimonodo->nodeID = nodos[d].nodeID,
                                    ultimonodo->type = nodos[d].type,
                                    ultimonodo->latitud = nodos[d].latitud,
                                    ultimonodo->longitud = nodos[d].longitud); */
                                    
                                    
                                }else{
                                    //no cumple el estanque 
                                    tuplasruta[m].skip = 0;
                                    goto sigamos;

                                }
                                
                            }
                            
                        }
                        
                    }
                    
                    //aqui termina la primera revision de ruta 
                    //como termina y fue factible se guardan los nuevos valores de distancia y tiempo
                    cambio = 0;
                    if (tuplasruta[g].type == 'd')//porque llego al final 
                    {
                        
                        valoresRutas[i].distanciaDeRuta = distancianueva;
                        valoresRutas[i].tiempoDeRuta = tiemponuevo;

                    }
                    
                    
                    
                }
                
                
            }

            //aqui se sigue dentro del for de encontrar estaciones
            //esta etiqueta es para salir de los for que ya no interesan ver si la ruta no sirve
            sigamos:
            //si quiero seguir aqui
            
            cambio = 1; //esto solo esta para que me sirva el label
            
            
         
        }
        
        /* if (cambio == 1)
        {
            valoresRutas[i].distanciaDeRuta = valorantiguodist;
            valoresRutas[i].tiempoDeRuta = valorantiguotiem;
        } */
        
        
            
        
        //aqui termino la revision de ruta, se printea
        for (size_t w = 0; w < largoRutas[i]; w++)
        {
            if (tuplasruta[w].skip != 1)
            {
                printf("(%d-%c),",tuplasruta[w].nodeId, tuplasruta[w].type);
            }
            
            
        }
        totaldistanciah = totaldistanciah + (valoresRutas[i].distanciaDeRuta);
        totaltiempoh = totaltiempoh + (valoresRutas[i].tiempoDeRuta);
        printf("%f,%f\n", valoresRutas[i].tiempoDeRuta, valoresRutas[i].distanciaDeRuta);
     


        
        
    }

    fclose(archivo);

}



int main(){
    clock_t begin = clock();
    para = (parametros*)malloc(sizeof(parametros));
    FILE *file = fopen("../AB207.dat","r");
    getinfo(file);
    greedy();
    hillFI();
    fclose(file);
    free(para);
    clock_t end = clock();
    double time_spent = (double)(end - begin)/CLOCKS_PER_SEC;
    printf("%f\n", time_spent);
    return 0;
}