/*
    Este codigo nos ayuda a saber que responderia la IA en un estado del tablero
*/  // Comentario: Explicación general del propósito del código

#include <bits/stdc++.h>  // Incluye todas las librerías estándar de C++

using namespace std;  // Usa el espacio de nombres estándar

string pathread="Tablero.txt", pathwrite="Respuesta.txt";  // Define las rutas de los archivos de entrada y salida

vector<int> layers;  // Vector que almacena la cantidad de neuronas por capa
vector<vector<vector<long double>>> weights;  // Matriz 3D que contiene los pesos entre las capas
vector<vector<long double>> biases;  // Matriz que contiene los biases para cada capa

long double sigmoid(long double x){
    return 1.0/(1.0+exp(-x));  // Función de activación sigmoide
}
vector<long double> query(vector<long double> v){
    auto ans=v;  // Inicializa la salida con el vector de entrada
    for(int l=0; l<weights.size(); l++){
        vector<long double> res(layers[l+1], 0);  // Crea un vector para almacenar las activaciones de la siguiente capa
        for(int i=0; i<layers[l+1]; i++){
            long double sum=biases[l][i];  // Inicializa la suma con el bias de la neurona
            for(int j=0; j<layers[l]; j++)sum+=weights[l][i][j]*ans[j];  // Suma ponderada: peso * activación previa
            res[i]=sigmoid(sum);  // Aplica la función sigmoide a la suma para obtener la activación
        }
        ans=res;  // Actualiza el vector de activaciones con la salida de la capa actual
    }

    return ans;  // Retorna la salida final de la red neuronal
}

void Init(){
    string s;
    cout<<"Ingrese el nombre del txt con la Red Nuronal (incluir el .txt):\n";  // Solicita el nombre del archivo que contiene la red neuronal
    cin>>s;  // Lee el nombre del archivo
    ifstream info(s);  // Abre el archivo de la red neuronal

    int n;
    info>>n;  // Lee el número de capas de la red
    layers.resize(n); biases.resize(n-1); weights.resize(n-1);  // Redimensiona los vectores según el número de capas
    for(auto &u:layers)info>>u;  // Lee el número de neuronas en cada capa
    
    for(int l=0; l<n-1; l++){
        weights[l].resize(layers[l+1], vector<long double> (layers[l]));  // Redimensiona la matriz de pesos para la conexión l a l+1
        biases[l].resize(layers[l+1]);  // Redimensiona el vector de biases para la capa l+1
    }

    for(auto &mat:weights)for(auto &v:mat)for(auto &u:v)info>>u;  // Lee los valores de los pesos desde el archivo
    for(auto &v:biases)for(auto &u:v)info>>u;  // Lee los valores de los biases desde el archivo

    info.close();  // Cierra el archivo de configuración de la red
}

int main(){
    Init();  // Inicializa la red neuronal leyendo la configuración desde el archivo
    ifstream fin(pathread); ofstream fout(pathwrite);  // Abre los archivos de entrada y salida
    vector<long double> que(26), res;  // Crea un vector de 26 elementos para el estado del tablero y otro para la respuesta
    for(auto &u:que)fin>>u;  // Lee el estado del tablero desde "Tablero.txt"
    for(auto &u:que)cout<<u<<' ';  // Imprime el estado del tablero en consola

    res=query(que);  // Ejecuta la red neuronal con el estado del tablero
    
    long double max_prob=-1; int ans=0;
    for(int i=0; i<res.size(); i++)if(que[i]==0)max_prob=max(max_prob, res[i]);  // Busca la mayor probabilidad en las posiciones vacías
    for(int i=0; i<res.size(); i++)if(res[i]==max_prob && que[i]==0)ans=i+1;  // Determina la posición con la mayor probabilidad (1-indexed)

    fout<<ans;  // Escribe la respuesta en "Respuesta.txt"
    fout.close();  // Cierra el archivo de salida
    fin.close();  // Cierra el archivo de entrada

    for(auto &u:res)cout<<u<<' ';  // Imprime las salidas de la red neuronal en consola
    
    return 0;  // Finaliza el programa
}
