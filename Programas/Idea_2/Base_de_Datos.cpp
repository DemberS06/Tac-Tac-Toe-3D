/*
    Este codigo nos ayuda crear una Base de Datos en base de Estrategias y Semillas dadas
    pero para optimizar la velocidad y calidad de búsqueda la Base de Datos no esta bien
    Estructurada
*/
#include "GameTools.h"   // Incluye las declaraciones de nuestras funciones de juego (check_board_2D, check_board_3D, check_seed)
#include <vector>
#include <map>
#include <utility>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <limits>

using namespace std; // Usa el espacio de nombres std para evitar escribir std::

/**
 * @brief Limpia la pantalla de la consola enviando secuencias de escape ANSI.
 * Intenta limpiar y mover el cursor a la esquina superior izquierda 3 veces.
 * (Funciona en terminales compatibles con ANSI como Linux, macOS, Windows Terminal moderno).
 */
void Clean_Ground() {
    cout << "\033[2J\033[1;1H"; // Código ANSI para limpiar pantalla y mover cursor a 1,1
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
}

/**
 * @brief Solicita al usuario ingresar el orden (1-26) en que se revisarán las casillas.
 * Valida que se ingresen 26 números únicos entre 1 y 26.
 * @return Un vector<int> con el orden válido ingresado por el usuario.
 */
vector<int> GetOrder() {
    vector<int> Order(26); // Vector para almacenar el orden

    // Bucle do-while para repetir la solicitud hasta que la entrada sea válida
    do {
        cout << "Ingrese el orden en el que se revisan las casillas del cubo (1-26, separados por espacio): ";

        set<int> elements; // Usa un set para verificar fácilmente la unicidad y el rango

        // Lee los 26 números del orden
        for (auto& u : Order) {
            cin >> u;
            // Verifica si el número está fuera del rango permitido (1-26)
            if (u < 1 || u > 26) {
                Order[0] = -1; // Marca la entrada como inválida
            }
            elements.insert(u); // Inserta en el set (duplicados se ignoran)
        }

        // Verifica si se ingresaron exactamente 26 números únicos
        if (elements.size() != 26) {
            Order[0] = -1;
        }

        if(Order[0] == -1){
            cout << "\nError: Ingrese 26 numeros unicos entre 1 y 26.\n";
            // Limpia el estado de error de cin y descarta la entrada inválida
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } 

    } while (Order[0] == -1); // Repite mientras la señal de error esté activa

    return Order; // Devuelve el vector con el orden válido
}

/**
 * @brief Solicita al usuario ingresar un estado inicial del tablero (semilla).
 * Valida que sean 26 números entre 0 y 2, que la diferencia entre
 * fichas de P1 (1) y P2 (2) no sea mayor a 1, y que el tablero
 * inicial no sea ya un estado ganador.
 * @return Un vector<int> con la semilla válida ingresada por el usuario.
 */
vector<int> GetSeed() {
    vector<int> Seed(26); // Vector para almacenar la semilla

    // Bucle do-while para repetir hasta obtener una semilla válida
    do {
        cout << "Ingrese la seed (26 numeros 0, 1 o 2 separados por espacio): ";
        int dif = 0; // Contador para la diferencia entre P1 y P2

        // Lee los 26 valores de la semilla
        for (auto& u : Seed) {
            cin >> u;
            // Verifica que el valor esté en el rango [0, 2]
            if (u < 0 || u > 2) {
                Seed[0] = -1;
            }
            // Actualiza el contador de diferencia
            if (u == 1) dif++;
            if (u == 2) dif--;
        }

        // Verifica que la diferencia de piezas sea válida (0 o 1)
        if (dif > 1 || dif < 0) {
            Seed[0] = -1;
        }

        // Si la entrada hasta ahora es válida, verifica si ya hay un ganador
        if (Seed[0] != -1) {
            if (check_board_3D(Seed)) { // Llama a la función de GameTools.h
                cout << "\nError: El tablero ingresado ya tiene un ganador.\n";
                Seed[0] = -1;
            }
        }
        // Si la entrada no fue válida, imprime mensaje y prepara para repetir
        else{
            cout << "\nError: Seed invalida. Verifique los valores (0-2), la diferencia de piezas (<=1) y que no haya ganador inicial.\n";
            // Limpia el estado de error de cin y descarta la entrada inválida
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            Seed[0] = -1; // Usa Seed[0] como señal de error interna
        }

    } while (Seed[0] == -1); // Repite mientras la señal de error esté activa

    return Seed; // Devuelve la semilla válida
}

/**
 * @brief Genera la base de datos "no estructurada" a partir de múltiples órdenes y semillas.
 * Para cada orden, pide N semillas, analiza cada semilla usando check_seed,
 * y guarda los resultados (ID de tablero y movimientos óptimos) en un archivo.
 */
void Make_Data() {
    // Abre (o crea) el archivo para escribir la base de datos no estructurada
    ofstream fout("Base_de_Datos_NO_Estructurada.txt");
    if (!fout.is_open()) { // Verifica si el archivo se abrió correctamente
        cerr << "Error: No se pudo abrir el archivo Base_de_Datos_NO_Estructurada.txt" << endl;
        return;
    }

    // Variables locales
    long long id; // Para reconstruir ID de tablero al guardar
    int i, N, K, deep; // Contadores y variables de entrada
    vector<int> Order, Seed; // Vectores para orden y semilla actual
    vector<pair<vector<int>, int>> Seeds; // Almacena las semillas y su profundidad inicial para un orden dado

    SeedResult_t Ans; // Variable para guardar el resultado de check_seed ({MoveMap, winner})

    cout << "Ingrese la cantidad de Ordenes que se van a revisar: ";
    cin >> K; // Lee cuántos órdenes (estrategias) se procesarán
    fout << K << "\n"; // Escribe la cantidad de órdenes en el archivo

    // Bucle principal: procesa cada orden
    while (K--) {
        Order = GetOrder(); // Obtiene un orden válido del usuario

        cout << "Ingrese la cantidad de semillas para este orden: ";
        cin >> N; // Lee cuántas semillas se usarán con este orden
        fout << N << "\n"; // Escribe la cantidad de semillas en el archivo

        Seeds.clear(); // Limpia el vector de semillas de la iteración anterior
        while (N--) {
            deep = 1; // Profundidad inicial (1 + piezas)
            Seed = GetSeed(); // Obtiene una semilla válida del usuario
            // Calcula la profundidad inicial del tablero
            for (const auto& u : Seed) deep += (u > 0);
            // Guarda la semilla y su profundidad
            Seeds.push_back({Seed, deep});
        }

        // Bucle para procesar cada semilla obtenida con el orden actual
        for (auto& [board, turno] : Seeds) { // Itera sobre las semillas guardadas
            Ans = check_seed(Order, board);
            for(auto u:board)cout<<u<<' ';cout<<"\n";
            cout << "  El ganador determinado es: " << Ans.second
                 << " | Se analizaron " << Ans.first.size() << " tableros distintos.\n";

            // Guardamos la información generada (mapa de movimientos) en el archivo
            fout << Ans.first.size() << "\n"; // Escribe cuántos estados se guardarán

            // Itera sobre el mapa de movimientos devuelto por check_seed
            // Ans.first es MoveMap_t (map<long long, vector<int>>)
            // 'u' es el ID del tablero (long long)
            // 'MovesForState' es el vector<int> de movimientos óptimos para ese estado 'u'
            for (const auto& [u, MovesForState] : Ans.first) {
                // Imprime el ID del tablero 'u' en formato base 3 (26 dígitos)
                id = u; // Copia el ID para no modificar la clave del mapa
                for (i = 0; i < 26; i++) {
                    fout << id % 3 << ' ';
                    id /= 3;
                }
                fout << "\n";

                // Imprime el vector de movimientos óptimos para este estado
                if (MovesForState.empty()) { // Si no hay movimientos óptimos registrados
                    for(i=0; i<26; ++i) fout << "0 "; // Imprime ceros como placeholder
                } else {
                    // Imprime el vector de movimientos tal cual está en el mapa
                    for (const auto& w : MovesForState) fout << w << ' ';
                }
                fout << "\n";
            }
        }
    }

    cout << "Proceso completado. Presione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer
    cin.get(); // Espera Enter

    fout.close(); // Cierra el archivo de salida
}

/**
 * @brief Reestructura la base de datos "no estructurada" en una versión más útil.
 * Lee el archivo intermedio, combina resultados de diferentes estrategias/semillas
 * para cada estado del tablero, y escribe la base de datos final.
 * La lógica de combinación parece priorizar movimientos ganadores y luego
 * combinar/filtrar otros movimientos basados en alguna heurística.
 */
void Struct_Data() {
    // Abre los archivos de entrada y salida
    ifstream rin("Base_de_Datos_NO_Estructurada.txt");
    ofstream rout("Base_de_Datos.txt");
    if (!rin.is_open()) {
        cerr << "Error: No se pudo abrir Base_de_Datos_NO_Estructurada.txt" << endl;
        return;
    }
    if (!rout.is_open()) {
        cerr << "Error: No se pudo abrir Base_de_Datos.txt para escritura" << endl;
        rin.close();
        return;
    }

    // Variables locales
    long long id, pot3, player_val; // IDs, potencias, valor de jugador leído
    int i, N, M, K, best, optimal_score; // Contadores y valor 'best' (parece indicar si hay mov. ganador)
    MoveMap_t Move; // Mapa para almacenar los movimientos combinados para cada ID de tablero
                    // Clave: ID del tablero. Valor: vector<int> representando ¿profundidad/score? para cada posible casilla (0-25).
    vector<int> Distance(26); // Vector temporal para leer los movimientos/distancias de una línea del archivo

    rin >> K; // Lee la cantidad de órdenes (estrategias) procesadas
    while (K--) {
        rin >> N; // Lee la cantidad de semillas para esta orden
        while (N--) {
            rin >> M; // Lee la cantidad de tableros generados por esta semilla/orden
            while (M--) {
                // Lee el ID del tablero en formato base 3
                id = 0;
                pot3 = 1; // Potencia de 3 actual
                for (i = 0; i < 26; i++) {
                    rin >> player_val; // Lee el valor de la casilla (0, 1, 2)
                    id += pot3 * player_val; // Construye el ID base 10
                    pot3 *= 3; // Actualiza potencia
                }

                // Prepara el vector en el mapa 'Move' para este ID si no existe
                vector<int>& Value = Move[id]; // Obtiene referencia al vector asociado a este ID
                if (Value.empty()) { // Si es la primera vez que vemos este ID...
                    Value.resize(26, 0); // ...lo redimensiona a 26 y lo inicializa con ceros.
                }

                // Lee el vector de movimientos/distancias asociado a este tablero en el archivo
                best = 0; // Reinicia 'best' (parece rastrear el valor máximo leído)
                for (auto& u : Distance) { // Lee 26 valores en el vector temporal 'Distance'
                    rin >> u;
                    best = max(best, u); // Encuentra el valor máximo en esta línea
                }
                // También considera el valor máximo ya almacenado para este ID en Value
                for (const auto& u : Value) { // Usa const auto& porque solo lee
                    best = max(best, u);
                }

                // Combina los datos leídos (Distance) con los ya almacenados (Value)
                for (i = 0; i < 26; i++) {
                    // La lógica de combinación:
                    // Si 'best' es 0 (ningún movimiento ganador encontrado aún), prioriza minimizar el valor.
                    if (!best) {
                        // Si Value[i] toma el menor entre el viejo y el nuevo.
                        Value[i] = min(Value[i], Distance[i]); // Combina con mínimo
                    }
                    // Si 'best' > 0 (algún movimiento ganador encontrado), aplica otra lógica.
                    else {
                        // Si el valor almacenado era <= 0 y el nuevo leído es > 0, actualiza con el máximo.
                        // (Parece priorizar un movimiento positivo sobre uno no positivo).
                        if (Value[i] <= 0 && Distance[i] > 0) { // Corregido >0 (era !=0)
                            Value[i] = Distance[i];
                        }
                        // En otros casos, actualiza con el mínimo.
                        // (Intenta encontrar el movimiento ganador más rápido)
                        else {
                            // Solo actualiza si el nuevo valor es estrictamente menor,
                            // o si el valor actual es 0 (inicial).
                            if (Value[i] == 0 || (Distance[i] > 0 && Distance[i] < Value[i])) {
                                Value[i] = Distance[i];
                            }
                        }
                    }
                }
            }
        }
    }

    // Filtra los tableros "basura" (aquellos sin movimientos útiles registrados)
    vector<long long> trash; // Vector para guardar IDs a eliminar
    for (auto& [u, v] : Move) { // Itera sobre el mapa combinado
        id = 0; // Usa 'id' como contador de movimientos no cero
        for (const auto& w : v) { // Itera sobre el vector de valores para este tablero
            id += (w != 0); // Cuenta cuántos valores son distintos de cero
        }
        if (!id) { // Si todos los valores son cero...
            trash.push_back(u); // ...marca este ID para eliminar.
        }
    }
    // Elimina los IDs marcados del mapa 'Move'
    for (const auto& u : trash) {
        Move.erase(u);
    }

    // Escribe la cabecera de la base de datos estructurada (cantidad de tableros)
    rout << Move.size() << "\n";

    // Escribe cada entrada de la base de datos estructurada
    for (auto& [u, v] : Move) { // Itera sobre el mapa filtrado
        // Imprime el ID del tablero 'u' en formato base 3
        id = u;
        for (i = 0; i < 26; i++, id /= 3) {
            rout << id % 3 << ' ';
        }
        rout << "\n"; // Fin de línea para el ID

        // Determina el "mejor" valor en el vector 'v' asociado
        best = 0; // Reinicia best
        // Encuentra el valor máximo en el vector 'v'
        for(const auto& w : v) best = max(best, w);

        // Encuentra el valor mínimo positivo o el valor máximo negativo/cero
        for(const auto& w : v) {
            // Si best (máximo global) <= 0 O el valor actual w es > 0...
            if(best <= 0 || w > 0) {
                // ...entonces busca el mínimo entre los valores positivos y el 'best' actual.
                best = min(best, w);
            }
        }
        // Imprime el vector de salida transformado basado en 'best'
        if (best > 0) { // Si el movimiento óptimo es ganador (positivo)
            for (const auto& w : v) {
                if (w == best) rout << "1 ";    // Movimiento óptimo ganador
                else if (w > 0) rout << "0.3 "; // Otro movimiento ganador (menos óptimo)
                else if (w == 0) rout << "0 ";   // Movimiento no considerado/disponible
                else rout << "-1 ";               // Movimiento perdedor
            }
        } else { // Si el movimiento óptimo es perdedor o empate (best <= 0)
            for (const auto& w : v) {
                // Marca con 1 solo los movimientos que igualan al 'best' (el mejor entre los malos)
                rout << (w == best) << ' ';
            }
        }
         rout << "\n"; // Fin de línea para el vector transformado
    }

    // Cierra los archivos
    rin.close();
    rout.close();
    cout << "Base de datos reestructurada en Base_de_Datos.txt\n";
    cout << "Presione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer si es necesario
    cin.get(); // Espera Enter
}

/**
 * @brief Permite al usuario validar una única semilla y orden.
 * Obtiene la semilla y el orden, llama a check_seed y muestra el resultado.
 */
void Valid_Seed() {
    cout << "--- Validar una Semilla ---\n";
    // Obtiene el orden y la semilla del usuario
    vector<int> Order = GetOrder();
    vector<int> Seed = GetSeed();

    // Llama a check_seed para analizar la semilla con el orden dado
    // Nota: check_seed espera vector<int>& board (no const), así que pasamos Seed.
    SeedResult_t Ans = check_seed(Order, Seed);

    // Muestra los resultados principales
    cout << "\n--- Resultados del Analisis ---\n";
    cout << "El ganador determinado es: " << Ans.second << "\n";
    cout << "Se analizaron " << Ans.first.size() << " tableros distintos.\n";

    // Pausa para que el usuario vea los resultados
    cout << "Presione Enter para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer si es necesario
    cin.get(); // Espera Enter
}

/**
 * @brief Muestra el menú principal y maneja la selección del usuario.
 * Llama a las funciones correspondientes o sale del programa.
 */
void menu() {
    // Opciones del menú
    vector<string> Menu = {
        "Verificar una Semilla",
        "Crear la Base de Datos",
        "Restructurar la Base de Datos",
        "Salir"
    };

    // Muestra las opciones del menú
    cout << "--- Menu Principal ---\n";
    for (int i = 0; i < Menu.size(); i++) {
        // Imprime opción [índice+1] Texto
        cout << "[" << i + 1 << "] " << Menu[i] << "\n";
    }

    int opcion = 0; // Variable para guardar la opción del usuario

    // Bucle para obtener una opción válida
    do {
        cout << "Ingrese la opcion (1-" << Menu.size() << "): ";
        cin >> opcion;
        // Verifica si la entrada fue un número y está en el rango
        if (cin.fail() || opcion < 1 || opcion > Menu.size()) {
            cout << "Opción invalida.\n";
            cin.clear(); // Limpia flag de error
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada
            opcion = 0; // Resetea opción para repetir bucle
        }
    } while (opcion == 0); // Repite si la opción no fue válida

    Clean_Ground(); // Limpia la pantalla antes de ejecutar la opción

    // Ejecuta la acción según la opción seleccionada
    switch (opcion) {
    case 1:
        Valid_Seed(); // Llama a la función para validar semilla
        break;
    case 2:
        Make_Data(); // Llama a la función para crear la BD no estructurada
        break;
    case 3:
        Struct_Data(); // Llama a la función para reestructurar la BD
        break;
    default: // Caso 4 (Salir) o cualquier otro valor inesperado
        cout << "Saliendo del programa.\n";
        return; // Sale de la función menu (y por tanto de main)
    }

    Clean_Ground(); // Limpia la pantalla después de ejecutar la opción

    // Llama recursivamente al menú para mostrarlo de nuevo
    return menu();
}

/**
 * @brief Función principal del programa.
 * Llama al menú principal para iniciar la interacción.
 * @return 0 si el programa termina correctamente.
 */
int main() {
    menu(); // Inicia el menú principal

    return 0; // Retorno estándar para main
}