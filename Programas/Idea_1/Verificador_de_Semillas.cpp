/*
    Este código nos ayuda a Saber si un tablero es 
    buena semilla para el Generador de la Base de Datos
*/
#include <bits/stdc++.h>  // Incluye todas las bibliotecas estándar

using namespace std;  // Utiliza el espacio de nombres std

long long Pot_3[30];                // La memorización de las potencias de 3
map<long long, int> Distance;       // Almacena la cantidad de jugadas necesarias para finalizar el juego en cada estado
map<long long, vector<int>> Move;   // Guarda los movimientos (profundidad) óptimos para cada estado
vector<int> Order(26);              // El orden en que se revisan las casillas según una estrategia dada

bool checkmat(vector<vector<int>> &v){// Función para saber si en una cara ya ha ganado alguien
    for(int i=0; i<3; i++){// Revisa filas
        if(v[i][0]!=0 && v[i][0]==v[i][1] && v[i][1]==v[i][2])return 1;  // Si todas las celdas de una fila son iguales y no son 0, hay ganador
    }
    
    for(int j=0; j<3; j++){// Revisa las columnas
        if(v[0][j]!=0 && v[0][j]==v[1][j] && v[1][j]==v[2][j])return 1;  // Si todas las celdas de una columna son iguales y no son 0, hay ganador
    }
    // Revisa las diagonales
    if(v[0][0]!=0 && v[0][0]==v[1][1] && v[1][1]==v[2][2])return 1;  // Diagonal principal
    if(v[0][2]!=0 && v[0][2]==v[1][1] && v[1][1]==v[2][0])return 1;  // Diagonal secundaria
    return 0;  // No se encontró un ganador en esta cara
}

bool checkvec(vector<int> &v){// Función para descomponer el tablero en las caras del cubo
    vector<vector<int>> mat[6]={// Matrices que representan el ID de la casilla de cada cara del cubo
        // Face 1 (arriba) - submatriz 0
        {{ 0,  1,  2},
         { 3,  4,  5},
         { 6,  7,  8}},
        // Face 2 (izquierda) - submatriz 1
        {{ 0,  3,  6},
         {21, 24,  9},
         {18, 15, 12}},
        // Face 3 (frontal) - submatriz 2
        {{ 6,  7,  8},
         { 9, 10, 11},
         {12, 13, 14}},
        // Face 4 (derecha) - submatriz 3
        {{ 8,  5,  2},
         {11, 25, 23},
         {14, 17, 20}},
        // Face 5 (abajo) - submatriz 4
        {{12, 13, 14 },
         {15, 16, 17 },
         {18, 19, 20 }},
        // Face 6 (atrás) - submatriz 5
        {{18, 19, 20},
         {21, 22, 23},
         { 0,  1,  2}}
    };

    for(int i=0; i<6; i++){// Recorre cada cara del cubo
        for(auto &a:mat[i])for(auto &b:a)b=v[b];  // Reemplaza los índices por los valores del vector v
        if(checkmat(mat[i]))return 1;  // Si alguna cara muestra línea ganadora, retorna verdadero
    }

    return 0;  // Si ninguna cara tiene ganador, retorna falso
}

void backtracking(int p, vector<int> v, long long id){// Revisa quién gana para un tablero dado utilizando backtracking
    int player=2-(p&1); // Determina el jugador: si la profundidad (p) es impar, le toca el jugador 1; de lo contrario, el jugador 2
    vector<int> pos;    // Vector para almacenar las posiciones válidas de juego

    for(auto &u:Order){// Recorre las casillas en el orden definido por la estrategia
        if(v[u-1]==0)pos.push_back(u-1);  // Si la casilla está vacía (0), se añade a las posiciones válidas
    }

    if(pos.empty())return Distance[id]=p, void();  // Si no hay movimientos válidos, se asigna al jugador 2 (empate favorable para este) y se registra la profundidad p

    for(auto &u:pos){// Se revisa si alguno de los movimientos lleva a la victoria inmediatamente
        v[u]=player;  // Realiza el movimiento asignando la casilla al jugador actual
        if(checkvec(v))return Distance[id]=p, Move[id].push_back(u), void();  // Si el movimiento es ganador, se registra y se retorna
        v[u]=0;  // Revierte el movimiento si no es ganador
    }

    for(auto &u:pos){
        long long new_id=id+player*Pot_3[u];  // Calcula un nuevo ID para el estado resultante utilizando la potencia de 3 en la posición u
        v[u]=player;  // Realiza el movimiento
        if(Distance[new_id]==0)backtracking(p+1, v, new_id);  // Si el nuevo estado no ha sido visitado, se continúa el backtracking recursivamente
        if(Distance[new_id]<0)return Distance[id]=-Distance[new_id], Move[id].push_back(u), void();  // Si el movimiento conduce a victoria, se registra y retorna
        v[u]=0;  // Revierte el movimiento
    }

    // Si ningún movimiento conduce a una victoria, se alarga el juego lo máximo posible
    int max_distance=0;
    for(auto u:pos){// Calcula la máxima cantidad de jugadas posibles (distancia) para los movimientos disponibles
        long long new_id=id+player*Pot_3[u];
        max_distance=max(max_distance, Distance[new_id]);  // Se obtiene la mayor distancia alcanzada
    }
    
    Distance[id]=-max_distance;  // Se asigna esa máxima distancia al estado actual
    
    for(auto &u:pos){// Registra los movimientos que optimizan la distancia (mejor prolongan el juego)
        long long new_id=id+player*Pot_3[u];
        if(max_distance==Distance[new_id])Move[id].push_back(u);
    }
    return;
}

int main(){
    Pot_3[0]=1; for(int i=1; i<30; i++)Pot_3[i]=Pot_3[i-1]*3; // Inicializa las potencias de 3 para representar el tablero en base 3
    long long id=0, deep=1;
    vector<int> v(26, 0);  // Inicializa el tablero con 26 casillas en 0 (vacío)

    cout<<"Ingrese el orden en el que se revisaran las casillas: ";
    for(auto &u:Order)cin>>u;
    cout<<"Ingrese el tablero inicial: ";  // Solicita el estado inicial del tablero
    for(auto &u:v)cin>>u;  // Lee los valores para cada casilla del tablero
    for(auto &u:v)deep+=u>0;  // Calcula la profundidad inicial sumando 1 por cada casilla ocupada (>0)
    
    for(int i=0; i<v.size(); i++)id+=v[i]*Pot_3[i];  // Construye un ID único del tablero, usando una codificación en base 3
    backtracking(deep, v, id);  // Inicia el proceso de backtracking para determinar el resultado del juego
    
    cout<<"Gana el jugador "<<((((deep&1)*2-1)*Distance[id]>0)?1:2)<<"\nY se jugaron "<<Distance.size()<<" jugadas distintas\n";  // Muestra el jugador ganador y cuántos estados se analizaron
    cin>>deep;  // Pausa para mantener abierta la consola y revisar los resultados
    
    return 0;  // Finaliza el programa
}
