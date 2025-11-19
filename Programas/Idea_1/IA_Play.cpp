/*
    Este codigo enfrenta varias IA´s en tableros preestablecidos
*/
#include <bits/stdc++.h>

using namespace std;

long double sigmoid(long double x){
    return 1.0/(1.0+exp(-x));  // Función sigmoide: transforma la suma ponderada a un valor entre 0 y 1
}

bool checkmat(vector<vector<long double>> &v){//Funcion para saber si en una cara ya ha ganado alguien
    for(int i=0; i<3; i++){//Revisa filas
        if(v[i][0]!=0 && v[i][0]==v[i][1] && v[i][1]==v[i][2])return 1;  // Si una fila tiene tres valores iguales y no son 0, se ganó
    }
    
    for(int j=0; j<3; j++){//Revisa las columnas
        if(v[0][j]!=0 && v[0][j]==v[1][j] && v[1][j]==v[2][j])return 1;  // Si una columna tiene tres valores iguales y no son 0, se ganó
    }
    //Revisa las diagonales
    if(v[0][0]!=0 && v[0][0]==v[1][1] && v[1][1]==v[2][2])return 1;      // Diagonal principal
    if(v[0][2]!=0 && v[0][2]==v[1][1] && v[1][1]==v[2][0])return 1;      // Diagonal secundaria
    return 0;
}

bool checkvec(vector<long double> &v){//Funcion para descomponer el tablero en las caras del cubo 
    vector<vector<long double>> mat[6]={//ID de la casilla de cada cara
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

    for(int i=0; i<6; i++){//Llama la funcion para revisar cada cara
        for(auto &a:mat[i])for(auto &b:a)b=v[b];  // Reemplaza cada índice con el valor real en el vector "v"
        if(checkmat(mat[i]))return 1;  // Si en alguna cara se cumple la condición de victoria, retorna verdadero
    }

    return 0;
}

void Imprimir(vector<long double> tab){
    for(auto u:tab)cout<<u<<' '; cout<<"\n";  // Imprime el estado actual del tablero
    for(int i=1; i<=26; i++)cout<<i%10<<" "; cout<<"\n";  // Imprime las posiciones (módulo 10 para mantener un dígito)
    return;
}

struct IA{
    string name;  // Nombre del archivo que contiene la red neuronal de la IA

    int win1=0, win2=0, draw=0;  // Contadores de resultados: victorias como jugador 1, jugador 2 y empates

    vector<int> layers;  // Arquitectura de la red (cantidad de neuronas por capa)
    vector<vector<vector<long double>>> weights;  // Pesos de la red neuronal
    vector<vector<long double>> biases;  // Biases de la red neuronal
    
    vector<long double> query(vector<long double> v){
        auto ans=v;  // Inicia el proceso con la entrada
        for(int l=0; l<weights.size(); l++){
            vector<long double> res(layers[l+1], 0);  // Prepara un vector para la salida de la capa l+1
            for(int i=0; i<layers[l+1]; i++){
                long double sum=biases[l][i];  // Suma el bias
                for(int j=0; j<layers[l]; j++)sum+=weights[l][i][j]*ans[j];  // Suma ponderada de las activaciones
                res[i]=sigmoid(sum);  // Aplica la función sigmoide
            }
            ans=res;  // Actualiza la respuesta con la salida de la capa actual
        }
    
        return ans;  // Retorna la salida final
    }
    
    void Init(){
        cout<<"Ingrese el nombre del txt con la Red Nuronal (incluir el .txt):\n";
        cin>>name;  // Lee el nombre del archivo que contiene la red neuronal
        ifstream info(name);
    
        int n;
        info>>n;  // Lee el número de capas
        layers.resize(n); biases.resize(n-1); weights.resize(n-1);
        for(auto &u:layers)info>>u;  // Lee la cantidad de neuronas de cada capa
        
        for(int l=0; l<n-1; l++){
            weights[l].resize(layers[l+1], vector<long double> (layers[l]));  // Redimensiona la matriz de pesos para la capa l
            biases[l].resize(layers[l+1]);  // Redimensiona el vector de biases para la capa l+1
        }
    
        for(auto &mat:weights)for(auto &v:mat)for(auto &u:v)info>>u;  // Lee los pesos
        for(auto &v:biases)for(auto &u:v)info>>u;  // Lee los biases
    
        info.close();
    }
};

IA enemy;  // Instancia de la IA contrincante

int IA_RES(vector<long double> v){
    vector<long double> w=enemy.query(v);  // La IA procesa el tablero (vector "v") y obtiene sus salidas

    long double max_val=-1;
    for(int i=0; i<26; i++)if(v[i]==0)max_val=max(w[i], max_val);  // Determina el valor máximo entre las salidas en posiciones vacías
    for(int i=0; i<26; i++)if(w[i]==max_val && v[i]==0)return i;  // Retorna la posición del mayor valor en un espacio vacío

    return 0;
}

void Player1(){
    vector<long double> tab(26, 0);  // Inicializa el tablero vacío (26 posiciones)

    for(int i=0; i<13; i++){
        int p;
        Imprimir(tab);  // Muestra el estado actual del tablero
        do{
            cout<<"Ingrese una posicion: ";
            cin>>p;
        }while(p<1||p>26 || tab[p-1]);  // Repite hasta que se ingrese una posición válida
        tab[p-1]=1;  // El jugador marca la posición con el valor 1

        if(checkvec(tab)){
            cout<<"YOU WIN\n";
            Imprimir(tab);
            cin>>p;
            return;
        }

        p=IA_RES(tab);  // La IA elige su movimiento
        tab[p]=2;  // La IA marca la posición con el valor 2
        if(checkvec(tab)){
            cout<<"YOU LOSE\n";
            Imprimir(tab);
            cin>>p;
            return;
        }
    }

    return;
}

void Player2(){
    vector<long double> tab(26, 0);  // Inicializa el tablero vacío

    for(int i=0; i<13; i++){
        int p;
        p=IA_RES(tab);  // La IA decide su movimiento
        tab[p]=1;  // La IA marca la posición con el valor 1
        if(checkvec(tab)){
            cout<<"YOU LOSE\n";
            Imprimir(tab);
            cin>>p;
            return;
        }
        
        Imprimir(tab);
        do{
            cout<<"Ingrese una posicion: ";
            cin>>p;
        }while(p<1||p>26 || tab[p-1]);  // Verifica que la posición ingresada sea válida
        tab[p-1]=2;  // El jugador marca la posición con el valor 2
        if(checkvec(tab)){
            cout<<"YOU WIN\n";
            Imprimir(tab);
            cin>>p;
            return;
        }
    }

    return;
}

void Menu(){
    string menu[] = {
        "Salir",
        "Inicializar IA",
        "Jugar P1",
        "Jugar P2"
    };

    for(int i=0; i<4; i++){
        cout<<"["<<i+1<<"]"<<menu[i]<<"\n";  // Muestra las opciones del menú
    }

    int option;
    
    do{
        cout<<"Ingrese una opcion: ";
        cin>>option;
    }while(option<1 || option>4);  // Solicita una opción válida

    IA new_IA;

    switch(option){
        case 1:
            return;  // Sale del menú
        break;
        case 2:
            new_IA.Init();  // Inicializa una nueva IA
            swap(new_IA, enemy);  // Reemplaza la IA actual con la nueva
        break;
        case 3:
            Player1();  // Jugar como Player1
        break;
        case 4:
            Player2();  // Jugar como Player2
        break;
    }

    return Menu();  // Regresa al menú para otra acción
}

int main(){
    
    Menu();  // Inicia el menú principal
    return 0;
}
