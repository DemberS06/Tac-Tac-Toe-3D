#include <bits/stdc++.h>           // Incluye todas las librerías estándar

using namespace std;              // Usamos el espacio de nombres estándar

string pathread="Base_de_Datos.txt", pathwrite="_"; // Rutas de lectura y generación de nombre para escritura
ifstream fin(pathread);           // Stream para leer desde "Base_de_Datos.txt"

vector<int> layers;               // Vector que guarda la cantidad de neuronas en cada capa
vector<vector<vector<long double>>> weights; // Matriz 3D para los pesos entre capas
vector<vector<long double>> biases;          // Matriz para los biases de cada capa
long double learningRate=0.1;     // Tasa de aprendizaje de la red

long double RandomAccess(){       // Función que retorna un número aleatorio entre -1 y 1
    return ((long double)rand()/RAND_MAX)*2-1;  // Casteo a long double, normaliza a [0,1] y escala a [-1,1]
}

long double sigmoid(long double x){  // Función sigmoide como función de activación
    return 1.0/(1.0+exp(-x));         // Calcula la sigmoide
}
long double dsigmoid(long double x){ // Derivada de la función sigmoide (si x ya es la salida de sigmoid)
    return x*(1.0-x);                // Fórmula de la derivada de la sigmoide en función de la salida
}

void train(vector<long double> input, vector<long double> target){  // Función para entrenar la red con backpropagation
    int numLayers=layers.size();    // Número total de capas en la red
    vector<vector<long double>> activations, zs;  // Vector para almacenar las activaciones y los valores "z" de cada capa
    activations.push_back(input);   // La primera capa es la de entrada (input)
    
    vector<long double> current=input;  // Inicia la propagación con la entrada
    for(int l=0; l<weights.size(); l++) {  // Itera por cada capa de la red (excluyendo la capa de entrada)
        vector<long double> z(layers[l+1], 0.0), next(layers[l+1], 0.0); // Vectores para valores "z" y siguientes activaciones
        for(int i=0; i<layers[l+1]; i++) {    // Para cada neurona en la capa l+1
            long double sum=0.0;               // Suma ponderada para la neurona i
            for(int j=0; j<layers[l]; j++)       // Itera por cada neurona en la capa l
                sum+=weights[l][i][j]*current[j]; // Suma producto del peso y la activación de la neurona anterior
            
            sum+=biases[l][i];   // Agrega el bias correspondiente a la neurona
            z[i]=sum;            // Guarda la suma ponderada en vector z
            next[i]=sigmoid(sum); // Calcula la activación aplicando la función sigmoide
        }
        zs.push_back(z);         // Guarda el vector z de la capa actual
        activations.push_back(next); // Guarda las activaciones de la capa
        current=next;            // Actualiza las activaciones para la siguiente iteración
    }
    
    vector<vector<double>> deltas(weights.size());  // Vector para almacenar los errores (delta) de cada capa
    
    deltas.back().resize(layers.back());  // Inicializa el error de la capa de salida
    for(int i=0; i<layers.back(); i++){
        double a=activations.back()[i];  // Activación de la neurona i en la capa de salida
        double error=target[i]-a;          // Error en la salida: diferencia entre target y la activación real
        deltas.back()[i]=error*dsigmoid(a); // Multiplica el error por la derivada de la sigmoide
    }
    
    for(int l=weights.size()-2; l>=0; l--){  // Propagación hacia atrás del error, desde la penúltima capa hasta la primera oculta
        deltas[l].resize(layers[l+1]);         // Reserva espacio para los errores en la capa l+1
        for(int i=0; i<layers[l+1]; i++){
            double error=0.0;                  // Inicializa el error acumulado para la neurona i
            for(int k=0; k<layers[l+2]; k++)    // Suma ponderada de los errores de la capa siguiente
                error+=weights[l+1][k][i]*deltas[l+1][k];
            
            double a=activations[l+1][i];      // Activación de la neurona i
            deltas[l][i]=error*dsigmoid(a);      // Calcula el delta para la neurona i
        }
    }
  
    for(int l=0; l<weights.size(); l++) {  // Actualiza los pesos y biases para cada capa
        for(int i=0; i<layers[l+1]; i++) {
            for(int j=0; j<layers[l]; j++)
                weights[l][i][j]+=learningRate*deltas[l][i]*activations[l][j]; // Actualiza el peso en base al delta y activación anterior
            
            biases[l][i]+=learningRate*deltas[l][i];  // Actualiza el bias en base al delta
        }
    }
}
  
void Init(vector<int> v){  // Función para inicializar la red neuronal con la arquitectura deseada
    layers=v; 
    int n=layers.size();  
    biases.resize(n-1);   // Crea espacio para los biases de cada capa oculta y salida
    weights.resize(n-1);  // Crea espacio para los pesos entre cada capa
  
    for(int l=0; l<n-1; l++){
        weights[l].resize(layers[l+1], vector<long double> (layers[l])); // Inicializa la matriz de pesos para la capa l a l+1
        biases[l].resize(layers[l+1]);   // Inicializa el vector de biases para la capa l+1
    }
  
    for(auto &mat:weights)         // Inicializa aleatoriamente todos los pesos
        for(auto &v:mat)
            for(auto &u:v)
                u=RandomAccess();
    for(auto &v:biases)            // Inicializa aleatoriamente todos los biases
        for(auto &u:v)
            u=RandomAccess();
  
    reverse(v.begin(), v.end());   // Invierte el vector de capas para construir el nombre de salida
    for(auto h:v){
        while(h)
            pathwrite+=char(h%10+'0'), h/=10; // Convierte cada capa a dígitos y los añade al nombre
        pathwrite+='_'; // Separa los números con un guión bajo
    }
    pathwrite.pop_back();         // Elimina el último guión bajo extra
    reverse(pathwrite.begin(), pathwrite.end());  // Invierte el string para obtener el nombre final
  
    return;
}
  
void Save(int h){  // Función para guardar el estado del modelo en un archivo
    string s;
    while(h)
        s=char(h%10+'0')+s, h/=10;  // Convierte la cantidad de ciclos a string
  
    ofstream info(pathwrite+s+"Epochs.txt");  // Crea y abre el archivo con el nombre generado
  
    info<<layers.size()<<' ';      // Escribe la cantidad de capas
    for(auto&u:layers)
        info<<u<<' ';             // Escribe la cantidad de neuronas por capa
  
    for(auto &mat:weights)         // Escribe todos los pesos con precisión de 8 decimales
        for(auto &v:mat)
            for(auto &u:v)
                info<<fixed<<setprecision(8)<<u<<' ';
    for(auto &v:biases)            // Escribe todos los biases con la misma precisión
        for(auto &u:v)
            info<<fixed<<setprecision(8)<<u<<' ';
  
    info.close();                 // Cierra el archivo
  
    return;
}
  
int main(){  // Función principal del programa
    cin.tie(0);
    cout<<"Ingrese la cantidade de layers: ";
    int laysize;
    cin>>laysize;
    vector<int> name(laysize);
    cout<<"Ingrese la cantidad de neuronas por Layer:\n";
    for(auto &u:name)
        cin>>u;
  
    Init(name);  // Inicializa la red con la configuración indicada
  
    int n, ciclos=2000;
    fin>>n;
  
    vector<vector<long double>> input(n, vector<long double> (26)), output=input; // Crea vectores para almacenar los datos de entrada y salida
    cout<<"Iniciando lectura:\n";
    for(int i=0; i<n; i++){
        if(i%(n/100)==0)
            cout<<i*100/n<<"%\n";   // Muestra el progreso en porcentaje
        for(auto &u:input[i])
            fin>>u;               // Lee cada valor de la entrada
        for(auto &u:output[i])
            fin>>u;               // Lee cada valor correspondiente de la salida
    }
  
    fin.close();                 // Cierra el archivo de lectura
  
    for(int h=0; h<ciclos; h++){
        for(int i=0; i<n; i++){
            train(input[i], output[i]);  // Entrena la red con cada patrón de entrada y salida
        }
        cout<<h+1<<" Ciclos terminados de "<<ciclos<<" Ciclos\n"; // Informa el progreso del entrenamiento
        if(h%50==49)
            Save(h+1);         // Guarda el modelo cada 50 ciclos
    }
  
    Save(ciclos);  // Guarda el modelo final después de completar todos los ciclos
  
    cin>>n;      // Pausa al final (útil en consola)
  
    return 0;
}
