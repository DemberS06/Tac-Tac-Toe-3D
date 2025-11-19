#include "IA_Tools.h"   
#include <vector>
#include <string>
#include <cmath>       
#include <iostream>    
#include <fstream>     
#include <iomanip>     
#include <numeric>     
#include <algorithm>   
#include <random>      
#include <limits>
#include <thread>  
#include <mutex>   
#include <functional> 

using namespace std;

// --- IMPLEMENTACIÓN DE LoadBaseData ---
/**
 * @brief Carga los datos de entrenamiento (pares input/output) desde "Base_de_Datos.txt".
 * @return Un vector de pares, donde cada par contiene un vector de entrada y un vector de salida.
 */
vector<pair<vector<long double>, vector<long double>>> LoadBaseData(){
    ifstream fin("Base_de_Datos.txt");
    // Añadir chequeo de apertura de archivo
    if (!fin.is_open()) {
        cerr << "Error: No se pudo abrir Base_de_Datos.txt para lectura." << endl;
        return {}; // Devuelve vector vacío
    }

    int Size;
    fin >> Size;
    // Añadir chequeo de lectura y valor de Size
    if (fin.fail() || Size <= 0) {
        cerr << "Error: Tamaño inválido o no encontrado en Base_de_Datos.txt." << endl;
        fin.close();
        return {}; // Devuelve vector vacío
    }

    vector<pair<vector<long double>, vector<long double>>> Seeds(Size, make_pair(vector<long double>(26), vector<long double>(26)));
    cout << "Iniciando lectura de " << Size << " ejemplos:\n";
    for(int i = 0; i < Size; i++){
        // Cálculo de progreso más robusto
        if (Size >= 100 && i % (Size / 100) == 0) {
             cout << static_cast<int>(static_cast<double>(i) * 100.0 / Size) << "%\n";
        } else if (i == Size - 1) { // Asegura mostrar 100% al final
             cout << "100%\n";
        }

        // Lee vector de entrada
        for(auto &u : Seeds[i].first) {
            fin >> u;
            if (fin.fail()) { // Chequeo de error de lectura
                 cerr << "Error leyendo datos de entrada en ejemplo " << i << endl;
                 fin.close();
                 return {}; // Devuelve vacío
            }
        }
        // Lee vector de salida
        for(auto &u : Seeds[i].second) {
            fin >> u;
             if (fin.fail()) { // Chequeo de error de lectura
                 cerr << "Error leyendo datos de salida en ejemplo " << i << endl;
                 fin.close();
                 return {}; // Devuelve vacío
            }
        }
    }

    fin.close();
    cout << "Lectura de datos completada.\n";
    return Seeds;
}

// --- Funciones Auxiliares (Internas a este archivo) ---
namespace { // Namespace anónimo para limitar alcance

/**
 * @brief Genera un número aleatorio 'long double' entre -1.0 y 1.0.
 * Usa el motor Mersenne Twister para mejor aleatoriedad que rand().
 * @return Un valor aleatorio long double.
 */
long double RandomAccess() {
    // Generador estático para que no se reinicie en cada llamada
    static std::mt19937_64 rng(std::random_device{}());
    // Distribución uniforme en el rango [-1.0, 1.0]
    static std::uniform_real_distribution<long double> dist(-1.0L, 1.0L);
    return dist(rng);
}

/**
 * @brief Calcula la función de activación sigmoide.
 * @param x Valor de entrada.
 * @return Valor de la sigmoide (entre 0 y 1).
 */
long double sigmoid(long double x) {
    // Usar expl para mayor precisión con long double
    return 1.0L / (1.0L + expl(-x));
}

/**
 * @brief Calcula la derivada de la función sigmoide.
 * @param sigmoid_output El valor YA calculado por la función sigmoide (sigmoid(x)).
 * @return El valor de la derivada en ese punto.
 */
long double dsigmoid(long double sigmoid_output) {
    return sigmoid_output * (1.0L - sigmoid_output);
}

} // Fin namespace anónimo

// --- Implementación de Métodos PRIVADOS de IA_t ---

/**
 * @brief Solicita al usuario la ruta del archivo para cargar una Red Neuronal.
 * Valida que el archivo exista y sea accesible. (Método Privado)
 */
void IA_t::GetNameAccess() {
    // Bucle hasta que se ingrese un nombre de archivo válido
    do {
        cout << "Ingrese la direccion del archivo donde está guardada la Red Neuronal:\n"
             << "Ejemplo: IA_26_50_26.txt\n";
        cin >> name_access;

        ifstream fin(name_access); // Intenta abrir el archivo para lectura

        if (!fin.is_open()) { // Si no se pudo abrir...
            cerr << "Error: No se pudo abrir el archivo '" << name_access << "'. Verifique la ruta y permisos." << endl;
            name_access.clear(); // Limpia el nombre para repetir el bucle
            // Limpiar buffer de cin por si acaso
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else {
            fin.close(); // Cierra el archivo si se pudo abrir (solo verificamos existencia)
        }
    } while (name_access.empty()); // Repite si el nombre quedó vacío (por error)
}

/**
 * @brief Genera el nombre base para guardar la IA basado en su estructura de capas. (Método Privado)
 * @return true si hubo un error (IA no inicializada), false si se generó el nombre.
 */
bool IA_t::GetNameSave() {
    // Verifica si la estructura de capas ha sido definida
    if (layers.empty()) {
        return true; // Indica error
    }

    name_save.clear(); // Limpia cualquier nombre anterior
    // Construye el nombre concatenando tamaños de capa separados por '_'
    for (const auto& u : layers) {
        name_save += to_string(u) + '_';
    }
    name_save.pop_back(); // Elimina el último '_' sobrante
    return false; // Indica éxito
}

/**
 * @brief Solicita al usuario la estructura de capas de la red neuronal. (Método Privado)
 * Valida que haya al menos 3 capas y que la primera y última sean de tamaño 26.
 */
void IA_t::GetLayers() {
    int laysize = 0;

    // Pide el número de capas hasta que sea >= 3
    do {
        cout << "Ingrese la cantidad de layers (minimo 3): ";
        cin >> laysize;
        if (cin.fail() || laysize < 3) {
            cout << "Error: Se requiere un minimo de 3 layers (entrada, oculta(s), salida).\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            laysize = 0; // Resetea para repetir
        }
    } while (laysize == 0);

    // Pide los tamaños de capa hasta que sean válidos
    do {
        layers.resize(laysize); // Ajusta el tamaño del vector
        cout << "Ingrese la cantidad de neuronas por Layer (ej: 26 50 26): ";
        bool input_ok = true;
        for (auto& u : layers) {
            cin >> u;
            if (cin.fail() || u <= 0) { // Valida que sean enteros positivos
                input_ok = false;
                break;
            }
        }

        if (!input_ok) {
            cout << "Error: Ingrese números enteros positivos para los tamaños de capa.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            layers.clear(); // Limpia para repetir
            continue; // Vuelve al inicio del do-while
        }

        // Valida que la primera y última capa tengan tamaño 26
        if (layers.front() != 26 || layers.back() != 26) {
            cout << "Error: La primera y ultima layer deben ser de tamaño 26.\n";
            layers.clear(); // Limpia para repetir
        }

    } while (layers.empty()); // Repite si el vector quedó vacío por error
}

/**
 * @brief Solicita al usuario el número de épocas ya completadas (al cargar un modelo). (Método Privado)
 * Valida que el número sea no negativo.
 */
void IA_t::GetEpochs() {
    do {
        cout << "Ingrese la cantidad de Epochs ya concluidos (>=0): ";
        cin >> Epochs;
        if (cin.fail() || Epochs < 0) {
            cout << "Error: La cantidad de Epochs debe ser no negativa.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            Epochs = -1; // Resetea para repetir
        }
    } while (Epochs < 0);
}

/**
 * @brief Calcula las probabilidades de salida de la red para un tablero dado. (Método Privado)
 * Realiza la propagación hacia adelante (forward pass).
 * @param board Referencia constante al estado del tablero (vector<int> 0, 1, 2).
 * @return Vector<long double> con las activaciones de la capa de salida.
 */
vector<long double> IA_t::GetProbability(const vector<int>& board) const {
    // Convierte la entrada int a long double
    vector<long double> current_activations(layers.front());
    for (size_t i = 0; i < board.size(); i++) {
        current_activations[i] = board[i];
    }

    // Propagación hacia adelante
    for (size_t l = 0; l < weights.size(); l++) { // Itera sobre las capas de conexión (0 a numLayers-2)

        vector<long double> next_activations(layers[l + 1]); // Salida de la capa l+1
        for (size_t i = 0; i < layers[l + 1]; i++) { // Neurona de destino 'i' en capa l+1
            long double sum = biases[l][i]; // Inicia suma con el bias
            // Verifica consistencia de tamaños (defensivo)
            if (layers[l] != weights[l][i].size() || layers[l] != current_activations.size()){
                 cerr << "Error: Inconsistencia en tamaños de pesos/activaciones en GetProbability (capa " << l << ", neurona " << i << ")." << endl;
                 return vector<long double>();
            }
            for (size_t j = 0; j < layers[l]; j++) { // Neurona de origen 'j' en capa l
                sum += weights[l][i][j] * current_activations[j]; // Suma ponderada
            }
            next_activations[i] = sigmoid(sum); // Aplica función de activación (definida en namespace anónimo)
        }
        current_activations = std::move(next_activations); // Actualiza para la siguiente capa (usa move para eficiencia)
    }

    return current_activations; // Devuelve las activaciones de la última capa
}

/**
 * @brief Guarda el estado actual de la red (estructura, pesos, biases) en un archivo. (Método Privado)
 * @param preffix Prefijo para el nombre del archivo.
 * @param suffix Sufijo para el nombre del archivo (antes de .txt).
 */
void IA_t::Save(const string& preffix, const string& suffix) {
    string filename = preffix + name_save + suffix + ".txt"; // Construye nombre completo
    cout << "Guardando IA en: " << filename << endl;
    ofstream fout(filename); // Abre archivo para escritura

    // Guarda estructura
    fout << layers.size(); // Guarda número de capas
    for (const auto& u : layers) {
        fout << ' ' << u; // Guarda tamaño de cada capa
    }
    fout << '\n'; // Nueva línea

    // Configura precisión para guardar pesos/biases
    fout << fixed << setprecision(8);

    // Guarda pesos
    for (const auto& mat : weights) {
        for (const auto& v : mat) {
            for (const auto& u : v) {
                fout << u << ' ';
            }
        }
        fout << '\n';
    }

    // Guarda biases
    for (const auto& v : biases) {
        for (const auto& u : v) {
            fout << u << ' ';
        }
        fout << '\n';
    }

    fout.close(); // Cierra el archivo
    
    cout << "Guardado completado." << endl;
}


/**
 * @brief Realiza el forward y backward pass para y ACUMULA los gradientes. (Método Privado)
 * @param input Vector de entrada para este ejemplo.
 * @param target Vector objetivo (salida deseada) para este ejemplo.
 */
void IA_t::GetTrain(const vector<long double>& input, const vector<long double>& target) {
    // --- Forward Pass ---
    vector<vector<long double>> activations; // Almacena activaciones de todas las capas [capa][neurona]
    vector<vector<long double>> zs;          // Almacena sumas ponderadas (z) de capas ocultas/salida [capa-1][neurona]
    activations.push_back(input); // Capa 0 = entrada

    vector<long double> current = input;
    for (size_t l = 0; l < weights.size(); l++) { // Itera conexiones l -> l+1
        vector<long double> z(layers[l + 1]);
        vector<long double> next(layers[l + 1]);
        for (size_t i = 0; i < layers[l + 1]; i++) { // Neurona destino i
            long double sum = biases[l][i];
            for (size_t j = 0; j < layers[l]; j++) { // Neurona origen j
                sum += weights[l][i][j] * current[j];
            }
            z[i] = sum;
            next[i] = sigmoid(sum); // Usa helper sigmoid
        }
        zs.push_back(z);             // Guarda z para la capa l+1
        activations.push_back(next); // Guarda activación para la capa l+1
        current = next;              // Actualiza para la siguiente iteración
    }

    // --- Backward Pass ---
    vector<vector<long double>> deltas(weights.size()); // Almacena errores delta [capa-1][neurona]

    // Calcula delta para la capa de salida
    deltas.back().resize(layers.back());
    for (size_t i = 0; i < layers.back(); i++) {
        long double a = activations.back()[i];
        long double error = target[i] - a;
        deltas.back()[i] = error * dsigmoid(a); // Usa helper dsigmoid
    }

    // Propaga delta hacia capas anteriores
    for (int l = weights.size() - 2; l >= 0; l--) { // Itera desde penúltima capa de conexión hacia atrás
        deltas[l].resize(layers[l + 1]);
        for (size_t i = 0; i < layers[l + 1]; i++) { // Neurona i en capa l+1
            long double error_sum = 0.0L;
            // Suma ponderada de deltas de la capa siguiente (l+2)
            for (size_t k = 0; k < layers[l + 2]; k++) {
                error_sum += weights[l + 1][k][i] * deltas[l + 1][k];
            }
            long double a = activations[l + 1][i]; // Activación de la neurona actual
            deltas[l][i] = error_sum * dsigmoid(a); // Usa helper dsigmoid
        }
    }

    // --- Acumulación de Gradientes SEGURA ---
    { // Bloque para limitar el alcance del lock_guard
        // *** Bloquea el mutex antes de acceder a los acumuladores compartidos ***
        std::lock_guard<std::mutex> lock(gradient_mutex);

        // Suma los gradientes calculados a los acumuladores compartidos
        if (GradientWeights.size() != weights.size() || GradientBiases.size() != biases.size()) { /* Error */ return; }
        for (size_t l = 0; l < weights.size(); ++l) {
             if (GradientWeights[l].size() != weights[l].size() || GradientBiases[l].size() != biases[l].size()) continue;
            for (size_t i = 0; i < layers[l + 1]; ++i) {
                if (GradientWeights[l][i].size() != weights[l][i].size()) continue;
                for (size_t j = 0; j < layers[l]; ++j) {
                    GradientWeights[l][i][j] += deltas[l][i] * activations[l][j];
                }
                GradientBiases[l][i] += deltas[l][i];
            }
        }
    } // El lock_guard se destruye aquí, liberando el mutex automáticamente
}

/**
 * @brief Actualiza los pesos y biases de la red usando los gradientes acumulados. (Método Privado)
 * @param Size El número real de ejemplos procesados en el último mini-lote.
 */
void IA_t::Update(const int& Size) {
    // Actualiza pesos y biases
    for (size_t l = 0; l < weights.size(); ++l) {
        // Se asume que los tamaños coinciden
        for (size_t i = 0; i < layers[l + 1]; ++i) {
            // Se asume que los tamaños coinciden
            for (size_t j = 0; j < layers[l]; ++j) {
                // Actualiza peso: w = w + learning_rate * (gradiente_acumulado / tamaño_lote)
                weights[l][i][j] += Learning_Rate * (GradientWeights[l][i][j] / Size);
            }
            // Actualiza bias: b = b + learning_rate * (gradiente_acumulado / tamaño_lote)
            biases[l][i] += Learning_Rate * (GradientBiases[l][i] / Size);
        }
    }
}

// --- Implementación de Métodos PÚBLICOS de IA_t ---

/**
 * @brief Elige el mejor movimiento para un tablero dado según la salida de la red. (Método Público)
 * @param board Referencia constante al estado del tablero (vector<int> 0, 1, 2).
 * @return El índice (0-based) de la casilla vacía con la mayor probabilidad de salida.
 * Retorna 0 si no hay movimientos válidos o hay error.
 */
int IA_t::query(const vector<int>& board) const {
    // Obtiene las probabilidades de la red llamando al método privado
    vector<long double> Probability = GetProbability(board);

    long double max_val = -1.0L; // Inicializa con valor bajo
    int best_move = -1;          // Inicializa con valor inválido

    // Encuentra la casilla vacía (board[i]==0) con la máxima probabilidad
    for (int i = 0; i < 26; i++) { // Asume tamaño 26
        if (board[i] == 0) { // Si la casilla está vacía
            if (Probability[i] > max_val) {
                max_val = Probability[i];
                best_move = i; // Guarda el índice de la mejor casilla encontrada hasta ahora
            }
        }
    }

    // Si no se encontró ninguna casilla vacía, best_move seguirá siendo -1
    return best_move; // Retorna el índice 0-based del mejor movimiento
}

/**
 * @brief Carga la estructura y parámetros de la red desde un archivo. (Método Público)
 * Utiliza los métodos privados GetNameAccess y GetEpochs.
 */
void IA_t::Read_IA() {
    // Obtiene el nombre del archivo si no se ha establecido
    GetNameAccess(); // Llama al método privado
    
    cout << "Cargando IA desde: " << name_access << endl;
    ifstream fin(name_access); // Abre el archivo

    // Lee la estructura de la red
    int n;
    fin >> n; // Lee número de capas
    
    layers.clear();
    biases.clear();
    weights.clear();
    GradientWeights.clear();
    GradientBiases.clear();

    layers.resize(n);
    biases.resize(n - 1);
    weights.resize(n - 1);
    GradientWeights.resize(n - 1);
    GradientBiases.resize(n - 1);

    for (int& u : layers) { // Lee tamaño de cada capa
        fin >> u;
    }

    // Redimensiona pesos y biases según la estructura leída
    for (int l = 0; l < n - 1; ++l) {
        weights[l].resize(layers[l + 1], vector<long double>(layers[l]));
        biases[l].resize(layers[l + 1]);
        GradientWeights[l].resize(layers[l + 1], vector<long double>(layers[l]));
        GradientBiases[l].resize(layers[l + 1]);
    }

    // Lee los pesos
    cout << "Leyendo pesos..." << endl;
    for (auto& mat : weights) {
        for (auto& v : mat) {
            for (auto& u : v) {
                fin >> u;
            }
        }
    }

    // Lee los biases
    cout << "Leyendo biases..." << endl;
    for (auto& v : biases) {
        for (auto& u : v) {
            fin >> u;
        }
    }

    fin.close(); // Cierra el archivo
    cout << "IA cargada exitosamente." << endl;
}

/**
 * @brief Inicializa una nueva red neuronal con estructura definida y pesos/biases aleatorios. (Método Público)
 * Utiliza los métodos privados GetLayers y GetNameSave.
 */
void IA_t::Make_IA() {
    // Obtiene la estructura de capas si no está definida
    if (layers.empty()) {
        GetLayers(); // Llama al método privado
    }
    cout << "Creando nueva IA con estructura: ";
    for(size_t i=0; i<layers.size(); i++) cout << layers[i] << (i == layers.size()-1 ? "\n" : "_");

    int n = layers.size();
    // Redimensiona vectores de pesos, biases y acumuladores de gradientes
    biases.resize(n - 1);
    weights.resize(n - 1);
    GradientWeights.resize(n - 1);
    GradientBiases.resize(n - 1);

    // Inicializa pesos y biases aleatoriamente
    for (int l = 0; l < n - 1; ++l) {
        weights[l].resize(layers[l + 1], vector<long double>(layers[l]));
        biases[l].resize(layers[l + 1]);
        GradientWeights[l].resize(layers[l + 1], vector<long double>(layers[l]));
        GradientBiases[l].resize(layers[l + 1]);

        for (auto& v : weights[l]) {
            for (auto& u : v) {
                u = RandomAccess(); // Llama a la función auxiliar interna
            }
        }
        for (auto& u : biases[l]) {
            u = RandomAccess(); // Llama a la función auxiliar interna
        }
    }

    Epochs = 0; // Establece épocas a 0 para una nueva IA
    cout << "Nueva IA creada e inicializada aleatoriamente. \n";
}

/**
 * @brief Orquesta el ciclo completo de entrenamiento por épocas y mini-lotes. (Método Público)
 * Utiliza los métodos privados GetTrain, Update y Save.
 * @param Seeds Vector de pares {input, output} que representa el conjunto de entrenamiento.
 */
void IA_t::Train(vector<pair<vector<long double>, vector<long double>>>& Seeds) {
    //Verifica que la Red Neuronal ha sido inicializada por algun metodo
    if(name_save.empty() && GetNameSave()){
        cerr<< "IA no Inicializada\n";
        return;
    }

    // Pide las épocas completadas para este archivo
    if(Epochs<0)GetEpochs(); // Llama al método privado
    
    cout << "Iniciando entrenamiento desde epoca " << Epochs << "...\n";

    // --- Configuración de Paralelismo ---
    // Determina el número de hilos a usar (ej. los disponibles menos uno para el hilo principal)
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads > 1) num_threads--;     // Deja un núcleo libre si hay más de uno
    if (num_threads == 0) num_threads = 1;  // Usa al menos un hilo
    cout<<"La cantidad de hilos es: "<<num_threads<<"\n";

    std::random_device rd;
    std::mt19937 g(rd());

    // Función lambda para poner a cero los acumuladores de gradientes
    auto Restart = [&]() {
        for (auto& mat : GradientWeights) {
            for (auto& v : mat) {
                for(auto &u : v) u = 0.0L;
            }
        }
        for (auto& v : GradientBiases) {
            for(auto &u : v) u = 0.0L;
        }
    };

    // Guarda estado inicial si es la época 0
    if (Epochs == 0) {
        Save("", "_0Epochs"); // Llama al método privado Save
    }

    // Bucle principal de entrenamiento (hasta 2000 épocas)
    while (Epochs < Last_Epoch) {
        Epochs++; // Incrementa contador de época

        // Ajusta la tasa de aprendizaje
        Learning_Rate = LR / (long double)(int((Epochs + Safe-1) / Safe));

        // Baraja el conjunto de datos para esta época
        shuffle(Seeds.begin(), Seeds.end(), g);

        // --- Bucle de Mini-Lotes ---
        for (size_t i = 0; i < Seeds.size(); i += MiniBatchSize) {
            Restart(); // Pone a cero los gradientes ANTES de procesar el lote

            // Determina el rango de índices para este mini-lote
            size_t start_index = i;
            size_t end_index = min(i + MiniBatchSize, Seeds.size()); // Cuidado con pasarse del final
            int current_actual_batch_size = end_index - start_index; // Tamaño real del lote

            // --- Paralelización del Cálculo de Gradientes ---
            vector<std::thread> threads; // Vector para guardar los hilos
            size_t examples_per_thread = current_actual_batch_size / num_threads;
            size_t extra_examples = current_actual_batch_size % num_threads;

            size_t current_start = start_index; // Índice de inicio para el sub-lote del hilo

            for (unsigned int t = 0; t < num_threads; t++) {
                size_t current_end = current_start + examples_per_thread + (t < extra_examples ? 1 : 0);
                // Asegura que no se pase del final del lote real
                current_end = min(current_end, end_index);

                // Lanza un hilo si hay ejemplos que procesar
                if (current_start < current_end) {
                    threads.emplace_back([&, current_start, current_end]() {
                        // Cada hilo procesa su rango de índices
                        for (size_t k = current_start; k < current_end; k++) {
                            // Llama a GetTrain
                            GetTrain(Seeds[k].first, Seeds[k].second);
                        }   
                    });
                }
                current_start = current_end; // Actualiza inicio para el siguiente hilo
            }

            // Espera a que todos los hilos del lote terminen
            for (auto& th : threads) {
                if (th.joinable()) {
                    th.join();
                }
            }
            // --- Fin Paralelización ---  

            Update(current_actual_batch_size);
        }

        cout << "Epoca " << Epochs << " completada. Tasa de Aprendizaje: " << Learning_Rate << "\n";

        // Guarda el modelo periódicamente (cada 50 épocas)
        // Condición original del usuario
        if (Epochs % Safe == 0) { // Guarda en época 50, 100, 150...
            Save("", "_" + to_string(Epochs) + "Epochs"); // Llama al método privado Save
        }
    } // Fin del bucle de épocas

    // Guarda el modelo final después de completar todas las épocas
    Save("IA_", ""); // Llama al método privado Save
    cout << "Entrenamiento completado." << endl;
}
