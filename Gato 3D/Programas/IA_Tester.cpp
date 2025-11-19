/*
    Este codigo nos ayuda a enfrentar varias IA´s en tableros preestablecidos
*/
#include <bits/stdc++.h>

using namespace std;

long double sigmoid(long double x){
    return 1.0/(1.0+exp(-x));  // Función sigmoide
}

bool checkmat(vector<vector<int>> &v){//Funcion para saber si en una cara ya ha ganado alguien
    for(int i=0; i<3; i++){//Revisa filas
        if(v[i][0]!=0 && v[i][0]==v[i][1] && v[i][1]==v[i][2])return 1;  // Si alguna fila es homogénea (y no 0) retorna verdadero
    }
    
    for(int j=0; j<3; j++){//Revisa las columnas
        if(v[0][j]!=0 && v[0][j]==v[1][j] && v[1][j]==v[2][j])return 1;  // Si alguna columna es homogénea (y no 0) retorna verdadero
    }
    //Revisa las diagonales
    if(v[0][0]!=0 && v[0][0]==v[1][1] && v[1][1]==v[2][2])return 1;      // Diagonal principal
    if(v[0][2]!=0 && v[0][2]==v[1][1] && v[1][1]==v[2][0])return 1;      // Diagonal secundaria
    return 0;
}

bool checkvec(vector<int> &v){//Funcion para descomponer el tablero en las caras del cubo 
    vector<vector<int>> mat[6]={//ID de la casilla de cada cara
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
        for(auto &a:mat[i])for(auto &b:a)b=v[b];  // Sustituye cada índice con el valor correspondiente del tablero
        if(checkmat(mat[i]))return 1;  // Si en alguna cara hay ganador, retorna verdadero
    }

    return 0;
}

struct IA{
    string name;  // Nombre del archivo con la red neuronal de la IA

    int win1=0, win2=0, draw=0;  // Contadores: victorias como jugador 1, como jugador 2 y empates

    vector<int> layers;  // Arquitectura de la red neuronal (número de neuronas por capa)
    vector<vector<vector<long double>>> weights;  // Pesos de la red neuronal
    vector<vector<long double>> biases;  // Biases de la red neuronal
    
    vector<long double> query(vector<long double> v){
        auto ans=v;  // Inicia la propagación con la entrada v
        for(int l=0; l<weights.size(); l++){
            vector<long double> res(layers[l+1], 0);  // Vector para almacenar la salida de la capa l+1
            for(int i=0; i<layers[l+1]; i++){
                long double sum=biases[l][i];  // Suma iniciada con el bias correspondiente
                for(int j=0; j<layers[l]; j++)sum+=weights[l][i][j]*ans[j];  // Suma ponderada con la activación anterior
                res[i]=sigmoid(sum);  // Aplica la función sigmoide a la suma
            }
            ans=res;  // La salida de la capa actual se vuelve la entrada de la siguiente
        }
    
        return ans;  // Retorna la salida final de la red
    }
    
    void Init(){
        cout<<"Ingrese el nombre del txt con la Red Nuronal (incluir el .txt):\n";
        cin>>name;  // Lee el nombre del archivo con la red neuronal
        ifstream info(name);
    
        int n;
        info>>n;  // Lee el número de capas
        layers.resize(n); biases.resize(n-1); weights.resize(n-1);
        for(auto &u:layers)info>>u;  // Lee la cantidad de neuronas por capa
        
        for(int l=0; l<n-1; l++){
            weights[l].resize(layers[l+1], vector<long double> (layers[l]));  // Redimensiona la matriz de pesos para cada capa
            biases[l].resize(layers[l+1]);  // Redimensiona el vector de biases para cada capa (excepto la de entrada)
        }
    
        for(auto &mat:weights)for(auto &v:mat)for(auto &u:v)info>>u;  // Lee todos los pesos de la red
        for(auto &v:biases)for(auto &u:v)info>>u;  // Lee todos los biases de la red
    
        info.close();
    }
};

vector<IA> Players;  // Vector que almacena a las IA enfrentadas
vector<vector<int>> Seeds;  // Vector de tableros preestablecidos (semillas)

int Fight(int x, int y, int p, vector<int> &v){
    vector<long double> que;
    for(int i=0; i<v.size(); i++)que.push_back(v[i]);  // Convierte el tablero (vector de int) a vector de long double
    
    que=Players[x].query(que);  // La IA x procesa el tablero y obtiene sus salidas

    long double max_prob=-1; int ans=-1;
    for(int i=0; i<que.size(); i++)if(v[i]==0)max_prob=max(max_prob, que[i]);  // Busca la mayor probabilidad en posiciones vacías
    for(int i=0; i<que.size(); i++)if(que[i]==max_prob && v[i]==0)ans=i;  // Selecciona la posición con mayor probabilidad

    if(ans==-1)return -1;

    v[ans]=p;  // Asigna el movimiento (marca la posición en el tablero)

    if(checkvec(v))return x;  // Si tras el movimiento se obtiene una victoria, retorna x (la IA que movió)
    return Fight(y, x, 3-p, v);  // Si no, invierte roles y continúa la pelea recursivamente
}

void Start(){
    int ans;
    for(int i=0; i<Players.size(); i++){
        for(int j=0; j<Players.size(); j++){
            if(j==i)continue;
            for(auto u:Seeds){
                ans=Fight(j, i, 2, u);  // La IA j enfrenta a la IA i utilizando el tablero u, comenzando con el jugador 2
                if(ans==i)Players[i].win1++;  // Si la victoria es para la IA i, incrementa su contador de victorias como jugador 1
                else if(ans==j)Players[j].win2++;  // Si la victoria es para la IA j, incrementa su contador de victorias como jugador 2
                else Players[j].draw++;  // En otro caso, se cuenta como empate para la IA j
            }
        }
    }
}

int main(){
    ifstream fin("Seeds.txt");
    int tam;
    fin>>tam;
    Seeds.resize(tam);
    for(auto &v:Seeds){
        v.resize(26);
        for(auto &u:v)fin>>u;  // Lee cada semilla (tablero preestablecido) del archivo "Seeds.txt"
    }
    fin.close();
    
    cout<<"Ingrese la cantidad de IA,s: ";
    cin>>tam;

    Players.resize(tam);

    for(int i=0; i<tam; i++)Players[i].Init();  // Inicializa cada IA leyendo su red neuronal desde archivo
    
    Start();  // Comienza el enfrentamiento entre las IA

    ofstream fout("Answer.txt");

    for(auto &u:Players)fout<<u.name<<' '<<u.win1<<' '<<u.win2<<' '<<u.draw<<"\n";  // Escribe los resultados de cada IA en "Answer.txt"
    fout.close();
    
    return 0;
}
