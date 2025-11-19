#include <bits/stdc++.h>  // Incluye todas las librerías estándar

using namespace std;  // Uso del espacio de nombres estándar

string pathread="Base_de_Datos.txt", pathwrite="_";  // Ruta del archivo de lectura y cadena inicial para la ruta de escritura
ifstream fin(pathread);  // Stream para leer el archivo "Base_de_Datos.txt"

vector<int> layers;  // Vector para almacenar la cantidad de neuronas por capa
vector<vector<vector<long double>>> weights;  // Matriz 3D con los pesos entre las neuronas de diferentes capas
vector<vector<long double>> biases;  // Matriz con los biases correspondientes de cada capa
long double learningRate=0.01;  // Tasa de aprendizaje inicial
int epochs;  // Número de ciclos ya completados (epochs)

long double RandomAccess(){  // Función que retorna un número aleatorio entre -1 y 1
    return ((long double)rand()/RAND_MAX)*2-1;  // Genera un valor aleatorio normalizado y escalado a [-1,1]
}

long double sigmoid(long double x){  // Función de activación sigmoide
    return 1.0/(1.0+exp(-x));  // Cálculo de la función sigmoide
}
long double dsigmoid(long double x){  // Derivada de la sigmoide asumiendo que x es la salida de la misma
    return x*(1.0-x);  // Fórmula para la derivada de la sigmoide
}

void train(vector<long double> input, vector<long double> target){  // Función que entrena la red mediante backpropagation
    int numLayers=layers.size();  // Obtiene el número total de capas
    vector<vector<long double>> activations, zs;  // Vectores para almacenar activaciones y valores "z" (suma ponderada)
    activations.push_back(input);  // La primera capa es la de entrada
    
    vector<long double> current=input;  // Inicializa la propagación con la entrada
    for(int l=0; l<weights.size(); l++) {  // Itera por cada capa (exceptuando la de entrada)
        vector<long double> z(layers[l+1], 0.0), next(layers[l+1], 0.0);  // Vectores para almacenar la suma ponderada y la activación de la siguiente capa
        for(int i=0; i<layers[l+1]; i++) {  // Recorre cada neurona de la capa l+1
            long double sum=0.0;  // Inicializa la suma para la neurona i
            for(int j=0; j<layers[l]; j++)sum+=weights[l][i][j]*current[j];  // Calcula la suma ponderada de la neurona i
            
            sum+=biases[l][i];  // Agrega el bias correspondiente
            z[i]=sum;  // Guarda el valor "z" para la neurona
            next[i]=sigmoid(sum);  // Calcula la activación aplicando la función sigmoide
        }
        zs.push_back(z);  // Almacena el vector "z" de la capa actual
        activations.push_back(next);  // Almacena la activación de la capa actual
        current=next;  // Actualiza para la siguiente iteración
    }
    
    vector<vector<double>> deltas(weights.size());  // Vector que contendrá el error (delta) para cada capa
    
    deltas.back().resize(layers.back());  // Ajusta el tamaño del vector de errores en la capa de salida
    for(int i=0; i<layers.back(); i++){
        double a=activations.back()[i];  // Obtiene la activación de la capa de salida para la neurona i
        double error=target[i]-a;  // Calcula el error como diferencia entre target y la activación
        deltas.back()[i]=error*dsigmoid(a);  // Calcula el delta ajustado por la derivada de la sigmoide
    }
    
    for(int l=weights.size()-2; l>=0; l--){  // Propaga el error hacia las capas anteriores (backpropagation)
        deltas[l].resize(layers[l+1]);  // Redimensiona el vector de errores para la capa l+1
        for(int i=0; i<layers[l+1]; i++){
            double error=0.0;  // Inicializa el error acumulado para la neurona i
            for(int k=0; k<layers[l+2]; k++)error+=weights[l+1][k][i]*deltas[l+1][k];  // Suma del error ponderado de la siguiente capa
            
            double a=activations[l+1][i];  // Obtiene la activación de la neurona en la capa actual
            deltas[l][i]=error*dsigmoid(a);  // Calcula el delta para la neurona i
        }
    }

    for(int l=0; l<weights.size(); l++) {  // Actualiza los pesos y biases para cada capa
        for(int i=0; i<layers[l+1]; i++) {
            for(int j=0; j<layers[l]; j++)
                weights[l][i][j]+=learningRate*deltas[l][i]*activations[l][j];  // Actualiza cada peso según el error y la activación previa
            
            biases[l][i]+=learningRate*deltas[l][i];  // Actualiza el bias de la neurona
        }
    }
}

void Init(){  // Función para inicializar la red a partir de un archivo previamente guardado
    string s;
    cout<<"Ingrese el nombre del ultimo archivo guardado (con todo y el .txt)\n";  // Solicita el nombre del archivo guardado
    cin>>s;
    cout<<"Ingrese la cantidad de ciclos concluidos: ";  // Solicita al usuario los ciclos ya entrenados
    cin>>epochs;
    ifstream info(s);  // Abre el archivo de configuración

    int n;
    info>>n;  // Lee la cantidad de capas de la red
    layers.resize(n); biases.resize(n-1); weights.resize(n-1);  // Redimensiona los vectores según n
    for(auto &u:layers)info>>u;  // Lee el número de neuronas por capa
    
    for(int l=0; l<n-1; l++){
        weights[l].resize(layers[l+1], vector<long double> (layers[l]));  // Reserva espacio para los pesos de la conexión entre las capas
        biases[l].resize(layers[l+1]);  // Reserva espacio para los biases de la capa
    }

    for(auto &mat:weights)for(auto &v:mat)for(auto &u:v)info>>u;  // Lee los pesos guardados del archivo
    for(auto &v:biases)for(auto &u:v)info>>u;  // Lee los biases guardados del archivo

    info.close();  // Cierra el archivo de configuración

    reverse(layers.begin(), layers.end());  // Invierte el vector de capas para construir la parte del nombre de salida

    for(auto h:layers){
        while(h)pathwrite+=char(h%10+'0'), h/=10;  // Convierte cada número de la capa a dígitos y los agrega a la cadena pathwrite
        pathwrite+='_';  // Agrega un guion bajo como separador
    }pathwrite.pop_back();  // Elimina el último guion bajo sobrante
    reverse(pathwrite.begin(), pathwrite.end());  // Invierte el string para obtener el nombre correcto
    reverse(layers.begin(), layers.end());  // Restaura el orden original en layers

    return;
}

void Save(int h){  // Función para guardar el estado actual de la red en un archivo
    string s;
    while(h)s=char(h%10+'0')+s, h/=10;  // Convierte el número de ciclos (h) a string

    ofstream info(pathwrite+s+"Epochs.txt");  // Abre el archivo para escribir utilizando el nombre generado

    info<<layers.size()<<' ';  // Escribe la cantidad de capas
    for(auto&u:layers)info<<u<<' ';  // Escribe el número de neuronas por capa

    for(auto &mat:weights)for(auto &v:mat)for(auto &u:v)info<<fixed<<setprecision(8)<<u<<' ';  // Escribe todos los pesos con 8 decimales de precisión
    for(auto &v:biases)for(auto &u:v)info<<fixed<<setprecision(8)<<u<<' ';  // Escribe todos los biases con 8 decimales de precisión

    info.close();  // Cierra el archivo

    return;
}

int main(){
    cin.tie(0);  // Desvincula cin de cout para mejorar el rendimiento en I/O
    Init();  // Inicializa la red a partir del archivo guardado

    int n, ciclos=2000;
    fin>>n;  // Lee la cantidad de patrones (datos) que se usarán para el entrenamiento

    vector<vector<long double>> input(n, vector<long double> (26)), output=input;  // Crea vectores para los datos de entrada y salida (26 elementos cada uno)
    cout<<"Iniciando lectura:\n";
    for(int i=0; i<n; i++){
        if(i%(n/100)==0)cout<<i*100/n<<"%\n";  // Muestra el progreso de lectura en porcentaje
        for(auto &u:input[i])fin>>u;  // Lee cada valor del vector de entrada
        for(auto &u:output[i])fin>>u;  // Lee cada valor del vector de salida
    }

    fin.close();  // Cierra el archivo de datos

    for(int h=epochs; h<ciclos; h++){  // Desde los ciclos ya entrenados hasta el total deseado
        if(h==1000)learningRate/=5;  // Al llegar al ciclo 1000, reduce la tasa de aprendizaje a 1/5
        for(int i=0; i<n; i++){
            train(input[i], output[i]);  // Entrena la red con cada patrón de entrada y salida
        }
        cout<<h+1<<" Ciclos terminados de "<<ciclos<<" Ciclos\n";  // Muestra el progreso del entrenamiento
        if(h%50==49)Save(h+1);  // Guarda el estado cada 50 ciclos
    }

    Save(ciclos);  // Guarda el estado final después de completar todos los ciclos

    cin>>n;  // Pausa la ejecución para permitir revisar el resultado (útil en consola)

    return 0;
}
