// IATools.h
#ifndef IATOOLS_H
#define IATOOLS_H

#include <vector>
#include <string>
#include <utility>
#include <mutex>   
#include <thread>

// --- Estructura Principal para la IA ---

struct IA_t {
public: // --- MIEMBROS PÚBLICOS ---
    // --- Nombres de Archivos ---
    std::string name_access; // Nombre/ruta para leer una IA existente

    // --- Estadísticas de Juego ---
    int win1 = 0; // Victorias como jugador 1
    int win2 = 0; // Victorias como jugador 2
    int draw = 0; // Empates
    bool vis = 0;  // 

    // --- Métodos Públicos ---

    // Método de Consulta/Predicción (solo el de alto nivel)
    int query(const std::vector<int>& board) const; // Elige el mejor movimiento

    // Métodos de Configuración/Inicialización (los principales)
    void Read_IA();     // Carga la IA (estructura y pesos) desde archivo
    void Make_IA();     // Inicializa una nueva IA con pesos aleatorios

    // Método Principal de Entrenamiento
    void Train(std::vector<std::pair<std::vector<long double>, std::vector<long double>>>& Seeds); // Orquesta el ciclo de entrenamiento completo

private: // --- MIEMBROS PRIVADOS ---

    // --- Nombres de Archivos ---
    std::string name_save;   // Nombre base para guardar la IA entrenada

    // --- Estructura de la Red ---
    std::vector<int> layers; // Número de neuronas en cada capa (incl. entrada/salida)

    // --- Parámetros de la Red (Pesos y Biases) ---
    std::vector<std::vector<std::vector<long double>>> weights; // [capa_origen][neurona_destino][neurona_origen]
    std::vector<std::vector<long double>> biases;               // [capa_origen][neurona_destino]

    // --- Acumuladores para Gradientes (Mini-Batch) ---
    // Guardan la suma de los gradientes calculados para un mini-lote
    std::vector<std::vector<std::vector<long double>>> GradientWeights;
    std::vector<std::vector<long double>> GradientBiases;

    // --- Parámetros de Entrenamiento ---
    int Epochs = -1;                    // Contador de épocas completadas (-1 indica no inicializado/leído)
    long double Learning_Rate;    // Tasa de aprendizaje
    long double LR = 0.2;
    int MiniBatchSize = 1;             // Tamaño del mini-lote 
    int Safe = 50;
    int Last_Epoch = 2000;
    std::mutex gradient_mutex;          // Para proteger acumuladores de gradientes en entorno multihilo

    // --- Métodos Privados ---

    // Métodos de Consulta/Predicción (detalle interno)
    std::vector<long double> GetProbability(const std::vector<int>& board) const; // Calcula salidas de la red

    // Métodos de Configuración/Inicialización (auxiliares)
    void GetNameAccess(); // Pide al usuario nombre de archivo para cargar
    bool GetNameSave();   // Genera nombre de archivo para guardar basado en 'layers'
    void GetLayers();     // Pide al usuario la estructura de capas
    void GetEpochs();     // Pide al usuario las épocas ya completadas (al cargar)

    // Método para Guardar (interno)
    void Save(const std::string& preffix, const std::string& suffix); // Guarda la IA actual

    // Métodos de Entrenamiento (pasos internos del ciclo Mini-Batch)
    void GetTrain(const std::vector<long double>& input, const std::vector<long double>& output); // Calcula y acumula gradientes
    void Update(const int& Size); // Actualiza pesos/biases usando gradientes acumulados y tamaño del lote
};

using IA_T = std::unique_ptr<IA_t>;

// --- DECLARACIÓN DE FUNCIÓN UTILITARIA ---
/**
 * @brief Carga los datos de entrenamiento (pares input/output) desde "Base_de_Datos.txt".
 * @return Un vector de pares, donde cada par contiene un vector de entrada y un vector de salida.
 */
std::vector<std::pair<std::vector<long double>, std::vector<long double>>> LoadBaseData();

#endif // IATOOLS_H