// GameTools.cpp


#include <bits/stdc++.h>
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
 * @brief Función recursiva de backtracking para determinar resultado del juego.
 *        Mantiene dos mapas: winDist (jugadas hasta victoria) y loseDist (jugadas hasta derrota).
 * @param deep      Profundidad actual de la búsqueda (1-based).
 * @param board     Referencia al tablero actual (se modifica temporalmente).
 * @param id        ID único del estado actual (base-3).
 * @param Move      Mapa de movimientos óptimos calculados.
 * @param winDist   Mapa de distancias mínimas hasta victoria (INT_MAX si no puede ganar).
 * @param loseDist  Mapa de distancias máximas hasta derrota (INT_MAX si no está obligado a perder).
 * @param Order     Orden heurístico (1-based) de casillas a explorar.
 * @param Pot_3     Vector de potencias de 3 para recálculo de ID.
 */
void backtracking(int deep,
                  vector<int>& board,
                  long long id,
                  MoveMap_t& Move,
                  map<long long,int>& winDist,
                  map<long long,int>& loseDist,
                  const vector<int>& Order,
                  const vector<long long>& Pot_3)
{
    int player = 1 + (deep & 1);            // Jugador actual: 2 si deep impar
    vector<int> pos; pos.reserve(26);

    // Recoger posiciones vacías según heurística
    for (int u : Order) {
        if (board[u-1] == 0) pos.push_back(u-1);
    }

    // Caso base: tablero lleno -> empate técnico (niego victoria y derrota)
    if (pos.empty()) {
        winDist[id]  = 50;
        loseDist[id] = 50;
        return;
    }

    // Chequeo de victoria inmediata
    for (int u : pos) {
        board[u] = player;
        if (check_board_3D(board)) {
            board[u] = 0;
            winDist[id]  = 1;           // Gana en 1 jugada
            loseDist[id] = 50;     // Nunca pierde
            Move[id] = {u};             // Movimiento ganador
            return;
        }
        board[u] = 0;
    }

    int bestWin  = 50;
    int bestLose = 0;

    // Exploración recursiva
    for (int u : pos) {
        long long nid = id + player * Pot_3[u];
        board[u] = player;
        if (!winDist.count(nid)) {
            backtracking(deep+1, board, nid, Move, winDist, loseDist, Order, Pot_3);
        }
        int childWin  = loseDist[nid];
        int childLose = winDist[nid];

        // Si el oponente puede forzar victoria desde nid -> mi derrota en childLose+1
        if (childLose < 50) {
            bestLose = max(bestLose, 1 + childLose);
        }
        // Si el oponente no puede evitar derrota desde nid -> mi victoria en childWin+1
        if (childWin < 50) {
            bestWin = min(bestWin, 1 + childWin);

            // Para no buscar en todo el árbol
            board[u] = 0;
            winDist[id]  = bestWin;     // Gana en 1 jugada
            loseDist[id] = 50;     // Nunca pierde
            Move[id] = {u};             // Movimiento ganador
            return;
        }
        board[u] = 0;
    }

    // Guardar distancias en el estado actual
    winDist[id]  = bestWin;
    loseDist[id] = (bestWin < 50 ? 50 : bestLose);

    // Seleccionar movimientos óptimos según métrica activa
    if (bestWin < 50) {
        // Priorizar victoria más rápida
        for (int u : pos) {
            long long nid = id + player * Pot_3[u];
            if (loseDist[nid] == bestWin - 1) Move[id].push_back(u);
        }
    } else {
        // Prolongar derrota en lo posible
        for (int u : pos) {
            long long nid = id + player * Pot_3[u];
            if (winDist[nid] == bestLose - 1) Move[id].push_back(u);
        }
    }
} // Fin backtracking_helper

} // Fin namespace anónimo


// --- Definición de check_seed ---
/**
 * @brief Analiza una configuración inicial del tablero ("semilla") usando backtracking con distancias separadas.
 * @param Order Referencia constante al vector que define el orden heurístico (1-based).
 * @param board Referencia NO constante al vector del estado inicial del tablero (0, 1, 2).
 *              Se modifica temporalmente durante la búsqueda.
 * @return Un std::pair (alias SeedResult_t) que contiene:
 *   - .first: Mapa de estados a un par {vector metrics, flag} donde:
 *       > metrics[i]: distancia a victoria (si flag==1) o distancia a derrota (si flag==-1).
 *       > flag: 1 para victoria, -1 para derrota, 0 si indeterminado.
 *   - .second: Jugador ganador (1, 2) o 0 si empate/indeterminado.
 */
SeedResult_t check_seed(const vector<int>& Order, vector<int>& board) {
    MoveMap_t Move;
    map<long long,int> winDist, loseDist;

    // Precalcular potencias de 3
    vector<long long> Pot_3(26,1);
    for (int i = 1; i < 26; i++) Pot_3[i] = Pot_3[i-1] * 3;

    // ID y profundidad inicial
    long long id = 0;
    int deep = 0;
    for (int i = 0; i < 26; i++) {
        deep += (board[i] != 0);
        id    += board[i] * Pot_3[i];
    }

    // Ejecutar backtracking
    backtracking(deep, board, id, Move, winDist, loseDist, Order, Pot_3);

    // Determinar ganador desde el estado inicial
    bool isP1 = !(deep & 1);
    int winner = 0;
    if (winDist[id] < 50)       winner = isP1 ? 1 : 2;
    else if (loseDist[id] < 50) winner = isP1 ? 2 : 1;

    // Construir resultado: para cada estado u, asociar métricas + flag
    SeedAns_t Ans;
    vector<int> All0(26, 0);
    for (auto& [u, moves] : Move) {
        vector<int> metrics = All0;
        int flag = 0; // 1=win, -1=lose, 0=draw
        if (winDist[u]  < 50) flag = 1;
        else if (loseDist[u] < 50) flag = -1;

        if (flag == 1) {
            for (int w : moves) metrics[w] = winDist[u];
        } else if (flag == -1) {
            for (int w : moves) metrics[w] = loseDist[u];
        } else if (flag == 0) {
            for (int w : moves) metrics[w] = 1;
        }
        Ans[u] = make_pair(metrics, flag);
    }

    return make_pair(Ans, winner);
}
