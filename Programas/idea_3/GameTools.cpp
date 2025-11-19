// GameTools.cpp

#include "GameTools.h"
#include <vector>
#include <map>
#include <utility>
#include <algorithm> 
#include <limits>

using namespace std;

// --- Definición de check_board_2D ---
/**
 * @brief Verifica si hay una línea ganadora en una matriz 2D (una "cara" 3x3 del tablero).
 * Comprueba filas, columnas y las dos diagonales principales.
 * @param face Referencia constante a un vector 2D representando la cara.
 * @return true si un jugador ha ganado en esta cara, false en caso contrario.
 */
bool check_board_2D(const vector<vector<int>>& face) { // Parámetro const&

    // Revisa las 3 filas
    for (int i = 0; i < 3; ++i) {
        // Comprueba si las 3 celdas de la fila son iguales entre sí y distintas de 0 (vacío)
        if (face[i][0] != 0 && face[i][0] == face[i][1] && face[i][1] == face[i][2]) {
            return true; // Ganador encontrado en la fila i
        }
    }

    // Revisa las 3 columnas
    for (int j = 0; j < 3; ++j) {
        // Comprueba si las 3 celdas de la columna son iguales entre sí y distintas de 0
        if (face[0][j] != 0 && face[0][j] == face[1][j] && face[1][j] == face[2][j]) {
            return true; // Ganador encontrado en la columna j
        }
    }

    // Revisa las diagonales
    // Diagonal principal (\)
    if (face[0][0] != 0 && face[0][0] == face[1][1] && face[1][1] == face[2][2]) {
        return true; // Ganador en la diagonal principal
    }
    // Diagonal secundaria (/)
    if (face[0][2] != 0 && face[0][2] == face[1][1] && face[1][1] == face[2][0]) {
        return true; // Ganador en la diagonal secundaria
    }

    // Si no se encontró ninguna línea ganadora
    return false; // No hay ganador en esta cara
}


// --- Definición de check_board_3D ---
/**
 * @brief Verifica si hay una línea ganadora en el tablero 3D completo.
 * Itera sobre las definiciones de las caras, actualiza la estructura local 'Boards_2D'
 * con los valores del tablero 'Seed', y luego llama a check_board_2D
 * con la estructura actualizada.
 * @param Seed Referencia constante al vector 1D que representa el estado completo del tablero.
 * @return true si se detecta una línea ganadora en alguna cara procesada,
 * false en caso contrario, o si el tamaño de Seed es inválido.
 */
bool check_board_3D(const vector<int>& Seed){ // Parámetro const& Seed
    // Define las 6 plantillas 3x3 de las caras del cubo mediante los índices (0-25).
    // 'Boards_2D' es una variable local (array de vectores de vectores)
    // que se inicializa con estos índices en cada llamada a la función.
    vector<vector<int>> Boards_2D[6] = {
        {{ 0,  1,  2}, { 3,  4,  5}, { 6,  7,  8}}, // Cara 0
        {{ 0,  3,  6}, {21, 24,  9}, {18, 15, 12}}, // Cara 1
        {{ 6,  7,  8}, { 9, 10, 11}, {12, 13, 14}}, // Cara 2
        {{ 8,  5,  2}, {11, 25, 23}, {14, 17, 20}}, // Cara 3
        {{12, 13, 14}, {15, 16, 17}, {18, 19, 20}}, // Cara 4
        {{18, 19, 20}, {21, 22, 23}, { 0,  1,  2}}  // Cara 5
    };
        
    // Itera sobre cada una de las 6 plantillas de cara (i de 0 a 5).
    for(int i = 0; i < 6; i++){ // Usa el número 6 directamente
        // Itera sobre cada fila ('row') dentro de la plantilla de la cara actual (Boards_2D[i]).
        // 'row' es una referencia a un vector<int> dentro de Boards_2D[i].
        for(auto &row : Boards_2D[i]){
            // Itera sobre cada celda ('cell') dentro de la fila actual.
            // 'cell' es una referencia al entero (originalmente un índice) en Boards_2D[i].
            for(auto &cell : row){
                // Usa ese índice para buscar el valor en el tablero 'Seed'.
                cell = Seed[cell]; // Actualiza la estructura local con el valor del tablero.
            }
        }
        // Llama a check_board_2D pasándole la estructura Boards_2D[i] que ahora contiene valores.
        if(check_board_2D(Boards_2D[i])) {
            return true; // Si check_board_2D encuentra un ganador, retorna true.
        }
    } // Fin del bucle for sobre las caras

    // Si se revisaron todas las caras y ninguna tenía un ganador.
    return false; // Retorna false.
}


// --- Implementación del Backtracking (Helper Interno) ---
namespace { // Namespace anónimo para limitar el alcance de la función helper

/**
 * @brief Función recursiva de backtracking para determinar el resultado del juego (Helper).
 * @param deep Profundidad actual de la búsqueda (número de jugada, 1-based).
 * @param board Referencia NO constante al tablero actual (se modifica temporalmente).
 * @param id ID único (long long) del estado actual del tablero. (Pasado por valor).
 * @param Move Referencia al mapa que almacena los movimientos óptimos encontrados.
 * @param Distance Referencia al mapa que almacena los resultados/distancias (memoización).
 * @param Order Referencia constante al vector con la heurística de orden de movimientos (1-based).
 * @param Pot_3 Referencia constante al vector precalculado de potencias de 3.
*/
void backtracking(int deep, vector<int>& board, long long id,
                         MoveMap_t& Move, map<long long, int>& Distance,
                         const vector<int>& Order, const vector<long long>& Pot_3)
{
    // Determina el jugador actual (1 o 2) basado en la profundidad.
    int player = 1 + (deep & 1); // P2 para deep impar (1,3...), P1 para deep par (2,4...)
    vector<int> pos; // Vector para almacenar las posiciones válidas (0-based).
    pos.reserve(26); // Reserva espacio para optimización potencial.

    // Encuentra todas las posiciones vacías (0) siguiendo el orden heurístico.
    for (const auto& u : Order) { // Order contiene índices 1-based.
        // Usa 'v' como en el código original pegado.
        if (board[u - 1] == 0) { // Comprueba si la casilla está vacía.
            pos.push_back(u - 1); // Añade la posición (0-based) a la lista de movimientos posibles.
        }
    }

    // Caso Base: No hay movimientos posibles (tablero lleno o juego bloqueado).
    if (pos.empty()) {
        // Asigna la profundidad del turno anterior como resultado y termina esta rama.
        Distance[id] = deep - 1;
        return; // Usa return estándar.
    }

    // Chequeo de Victoria Inmediata: Revisa si algún movimiento gana directamente.
    for (int u : pos) { // Itera sobre las posiciones válidas (0-based).
        board[u] = player; // Realiza el movimiento temporalmente.
        if (check_board_3D(board)) { // Verifica si este movimiento resulta en victoria.
            board[u] = 0;   // Deshace el movimiento en el tablero.
            Distance[id] = deep; // Guarda la profundidad actual como resultado (positivo = victoria).
            Move[id] = {u};      // Guarda el movimiento ganador.
            return; // Retorna inmediatamente (estrategia: primera victoria encontrada).
        }
        board[u] = 0; // Deshace el movimiento si no fue ganador.
    }

    int max_opponent_distance = 0; // Guarda la máxima distancia (>=0) encontrada en hijos.

    // Exploración Recursiva y Búsqueda de Victoria Forzada.
    // Itera de nuevo sobre las posiciones válidas.
    
    for (int u : pos) { // u es índice 0-based.
        // Asume que u es un índice válido para Pot_3.
        long long new_id = id + player * Pot_3[u]; // Calcula el ID del estado resultante.
        board[u] = player; // Realiza el movimiento.

        // Llama recursivamente si el nuevo estado no ha sido visitado.
        // Usa .count() para chequear (como en el código proporcionado).
        if (!Distance.count(new_id)) {
            // Pasa todos los parámetros necesarios.
            backtracking(deep + 1, board, new_id, Move, Distance, Order, Pot_3);
        }

        // Revisa el resultado del estado hijo (usa .count() como provisto).
        int child_outcome = Distance[new_id]; // Acceso directo.
        // Si el oponente PIERDE desde el estado hijo (resultado negativo)...
        if(child_outcome < 0)return board[u] = 0, Distance[id] = -child_outcome, Move[id].push_back(u), void();
        if(max_opponent_distance >= 0 || child_outcome < 0) max_opponent_distance = max(max_opponent_distance, child_outcome);

        board[u] = 0; // Deshace el movimiento si no llevó a una victoria forzada.
    }

    // Asigna la distancia negada (resultado negativo o cero = derrota/empate).
    Distance[id] = -max_opponent_distance; // Acceso directo.
    Move[id].clear(); // Limpia movimientos previos por si acaso.

    // Guarda TODOS los movimientos que llevan a esa distancia máxima (acceso directo).
    for (int u : pos) { // u es índice 0-based.
        long long new_id = id + player * Pot_3[u];
        // Acceso directo a Distance[new_id].
        if(Distance[new_id] == max_opponent_distance) {
            Move[id].push_back(u); // Acceso directo.
        }
    }
} // Fin backtracking_helper

} // Fin namespace anónimo


// --- Definición de check_seed ---
/**
 * @brief Analiza una configuración inicial del tablero ("semilla") usando backtracking.
 * Configura y lanza la búsqueda recursiva, luego calcula el resultado final.
 * @param Order Referencia constante al vector que define el orden heurístico (1-based).
 * @param board Referencia NO constante al vector del estado inicial del tablero (0, 1, 2).
 * Se pasa no-constante porque el helper backtracking lo modifica temporalmente.
 * @return Un std::pair (alias SeedResult_t) que contiene el mapa de movimientos óptimos
 * calculado y el jugador ganador (1, 2, o 0 para empate/indeterminado).
 * @note La cantidad de estados analizados se puede obtener llamando a .size()
 * en el mapa .first del par devuelto.
 */
SeedResult_t check_seed(const vector<int>& Order, vector<int>& board) { // board no-constante
    // Crear mapas locales para resultados y memorización.
    MoveMap_t Move, Ans;
    map<long long, int> Distance;

    // Calcular potencias de 3.
    vector<long long> Pot_3(26); // Tamaño fijo 26.
    Pot_3[0] = 1;
    for (int i = 1; i < 26; i++) { // Usa literal 26.
        Pot_3[i] = 3 * Pot_3[i - 1];
    }

    // Calcular estado inicial (ID y profundidad).
    long long id = 0;
    int deep = 0;
    
    for (int i = 0; i < 26; i++) {
        // Chequeo de valor como provisto.
        deep += (board[i] != 0); // Incrementa profundidad si no está vacío.
        id += board[i] * Pot_3[i]; // Calcula ID en base 3.
    }

    // Llamar a la función helper de backtracking.
    backtracking(deep, board, id, Move, Distance, Order, Pot_3);

    // Determinar el jugador ganador final basado en el resultado del estado inicial.
    int winner = 0; 

    int outcome = Distance[id]; // Acceso directo (asume que 'id' existe).
    bool is_P1_initial_turn = 1 ^ (deep & 1); // True si P1 mueve en 'deep'.

    if (outcome > 0) { // Gana el jugador que inicia en 'deep'.
        winner = is_P1_initial_turn ? 1 : 2;
    } else { // Pierde el jugador que inicia en 'deep'.
        winner = is_P1_initial_turn ? 2 : 1;
    }

    vector<int> All0(26, 0), s;

    for(auto &[u, v]:Move){
        s=All0;  // Reinicia s a un vector de ceros
        for(auto &w:v)s[w]=Distance[u];  // Asigna a las posiciones óptimas la distancia (número de jugadas)
        Ans[u]=s;
    }

    // Devolver el resultado final: el mapa de movimientos y el ganador.
    return make_pair(Ans, winner); // Usa make_pair.
}
