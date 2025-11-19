// GameTools.h
#ifndef GAMETOOLS_H
#define GAMETOOLS_H

#include <vector>  // Para std::vector
#include <map>     // Para std::map
#include <utility> // Para std::pair

// --- Alias de Tipos ---
// (Estos alias se usan en la declaración de check_seed)

/** @brief Alias para el mapa que almacena movimientos óptimos.
 * Clave: ID del estado del tablero (long long).
 * Valor: Vector de índices (0-based) de movimientos óptimos (int). */
using MoveMap_t = std::map<long long, std::vector<int>>;

/**
 * @brief Alias para la respuesta detallada de una semilla.
 *        Mapea cada estado (ID) a un par donde:
 *          - first: vector<int> de métricas (distancias) por movimiento óptimo.
 *          - second: flag (1=win, 2=lose, 0=indeterminado) para ese estado.
 */
using SeedAns_t = std::map<long long, std::pair<std::vector<int>, int>>;

/**
 * @brief Alias para el resultado completo del análisis de una semilla.
 *        first: SeedAns_t, el mapa completo de estados a métricas y flags.
 *        second: int con el jugador ganador (1 o 2) o 0 si empate/indeterminado.
 */
using SeedResult_t = std::pair<SeedAns_t, int>;

// --- Declaraciones de Funciones ---
// (Solo se declaran las funciones que deben ser visibles/llamables
// desde otros archivos .cpp que incluyan este header)

/**
 * @brief Verifica si hay una línea ganadora en una matriz 2D (una "cara" 3x3 del tablero).
 * Comprueba filas, columnas y las dos diagonales principales.
 * @param face Referencia constante a un vector 2D representando la cara.
 * @return true si un jugador ha ganado en esta cara, false en caso contrario.
 */
bool check_board_2D(const std::vector<std::vector<int>>& face);

/**
 * @brief Verifica si hay una línea ganadora en el tablero 3D completo.
 * Itera sobre las definiciones de las caras, actualiza la estructura local 'Boards_2D'
 * con los valores del tablero 'Seed', y luego llama a check_board_2D
 * con la estructura actualizada.
 * @param Seed Referencia constante al vector 1D que representa el estado completo del tablero.
 * @return true si se detecta una línea ganadora en alguna cara procesada,
 * false en caso contrario, o si el tamaño de Seed es inválido.
 */
bool check_board_3D(const std::vector<int>& Seed);

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
SeedResult_t check_seed(const std::vector<int>& Order, std::vector<int>& board);

// NOTA: La función 'backtracking' NO se declara aquí porque está
//       definida dentro de un namespace anónimo en GameTools.cpp,
//       lo que la hace interna a ese archivo y no parte de la interfaz pública.

#endif // GAMETOOLS_H