#include "GameTools.h" 
#include "IA_Tools.h"  
#include <vector>      
#include <string>      
#include <utility>     
#include <iostream>    
#include <fstream>     
#include <limits>      
#include <ios>         
#include <algorithm>   

using namespace std; 

/**
 * @brief Limpia la pantalla de la consola enviando secuencias de escape ANSI.
 * Intenta limpiar y mover el cursor a la esquina superior izquierda 3 veces.
 * (Funciona en terminales compatibles con ANSI).
 */
void Clean_Ground() {
    cout << "\033[2J\033[1;1H"; // Código ANSI para limpiar pantalla y mover cursor a 1,1
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
}

/**
 * @brief Imprime el estado actual del tablero y los números de posición.
 * @param tab Vector<int> que representa el tablero (0-25). Se pasa por copia.
 */
void Imprimir(vector<int> tab){ // Recibe copia del tablero
    // Imprime los valores de cada casilla del tablero
    for(auto u:tab) cout << u << ' ';
    cout << "\n";
    // Imprime los números de posición (1-26), mostrando solo el último dígito
    for(int i=1; i<=26; i++) cout << i % 10 << " ";
    cout << "\n";
}

/**
 * @brief Simula una partida donde el humano juega como Jugador 1 contra la IA.
 * @param IA Referencia al objeto IA_t contra el que se jugará.
 */
void Player1(IA_t& IA){
    vector<int> tab(26, 0); // Inicializa un tablero vacío (26 casillas a 0)
    int p; // Variable para guardar la posición ingresada por el humano o elegida por la IA

    // El juego dura como máximo 13 turnos por jugador (26 casillas / 2 jugadores)
    for(int i = 0; i < 13; i++){
        Imprimir(tab); // Muestra el tablero actual

        // Bucle para obtener una jugada válida del humano
        do{
            cout << "Ingrese una posicion (1-26): ";
            cin >> p;
            // Validación de entrada: debe ser número, estar en rango y la casilla debe estar vacía
            if (cin.fail() || p < 1 || p > 26 || tab[p - 1] != 0) {
                cout << "Posición inválida o ya ocupada. Intente de nuevo.\n";
                cin.clear(); // Limpia flags de error
                cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta entrada
                p = 0; // Poner p inválido para repetir bucle
            }
        } while(p == 0); // Repite si la entrada fue inválida

        tab[p - 1] = 1; // Marca la jugada del humano (Jugador 1) en el tablero (ajusta índice a 0-based)

        // Verifica si el humano ganó después de su jugada
        if(check_board_3D(tab)){ // Llama a la función de GameTools
            cout << "--- ¡GANASTE! ---\n";
            Imprimir(tab); // Muestra el tablero final
            cout << "Presione Enter para continuar...";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get(); // Pausa
            return; // Termina la función
        }

        // Turno de la IA (Jugador 2)
        cout << "Turno de la IA...\n";
        p = IA.query(tab); // Llama al método query de la IA para obtener su movimiento (0-based)

        tab[p] = 2; // Marca la jugada de la IA (Jugador 2) en el tablero

        // Verifica si la IA ganó después de su jugada
        if(check_board_3D(tab)){ // Llama a la función de GameTools
            cout << "--- ¡PERDISTE! ---\n";
            Imprimir(tab); // Muestra el tablero final
            cout << "Presione Enter para continuar...";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get(); // Pausa
            return; // Termina la función
        }
    }
    // Si el bucle termina (13 turnos por jugador) sin un ganador, es empate.
    cout << "--- ¡EMPATE! ---\n";
    Imprimir(tab);
    cout << "Presione Enter para continuar...";
    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
}

/**
 * @brief Simula una partida donde el humano juega como Jugador 2 contra la IA.
 * @param IA Referencia al objeto IA_t contra el que se jugará.
 */
void Player2(IA_t& IA){
    vector<int> tab(26, 0); // Inicializa un tablero vacío
    int p; // Variable para guardar la posición

    // El juego dura como máximo 13 turnos por jugador
    for(int i = 0; i < 13; i++){

        // Turno de la IA (Jugador 1)
        cout << "Turno de la IA...\n";
        p = IA.query(tab); // La IA decide su movimiento (0-based)

        tab[p] = 1; // La IA marca la posición con el valor 1

        // Verifica si la IA ganó
        if(check_board_3D(tab)){
            cout << "--- ¡PERDISTE! ---\n";
            Imprimir(tab);
            cout << "Presione Enter para continuar...";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
            return;
        }

        // Turno del Humano (Jugador 2)
        Imprimir(tab); // Muestra el tablero
        do{
            cout << "Ingrese una posicion (1-26): ";
            cin >> p;
            // Validación de entrada
            if (cin.fail() || p < 1 || p > 26 || tab[p - 1] != 0) {
                 cout << "Posición inválida o ya ocupada. Intente de nuevo.\n";
                 cin.clear();
                 cin.ignore(numeric_limits<streamsize>::max(), '\n');
                 p = 0;
            }
        } while(p == 0);

        tab[p - 1] = 2; // El jugador marca la posición con el valor 2

        // Verifica si el humano ganó
        if(check_board_3D(tab)){
            cout << "--- ¡GANASTE! ---\n";
            Imprimir(tab);
            cout << "Presione Enter para continuar...";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
            return;
        }
    }
    // Si el bucle termina sin ganador, es empate.
    cout << "--- ¡EMPATE! ---\n";
    Imprimir(tab);
    cout << "Presione Enter para continuar...";
    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); cin.get();
}

/**
 * @brief Muestra el menú para jugar contra la IA y maneja la selección.
 * @param IA Referencia al objeto IA_t que se usará para jugar. Se puede recargar.
 */
void menu_Play(IA_t& IA){
    // Define las opciones del submenú de juego
    string menu[] = {
        "Jugar P1",         // Opción 1
        "Jugar P2",         // Opción 2
        "Inicializar IA",   // Opción 3 (Cargar otra IA)
        "Salir"             // Opción 4 (Volver al menú principal)
    };

    // Muestra las opciones
    cout << "--- Jugar Contra IA ---\n";
    for(int i = 0; i < 4; i++){ // Asume 4 opciones
        cout << "[" << i + 1 << "] " << menu[i] << "\n";
    }

    int option; // Variable para la opción del usuario

    // Bucle para obtener una opción válida
    do{
        cout << "Ingrese una opcion: ";
        cin >> option;
        if (cin.fail() || option < 1 || option > 4) {
            cout << "Opción inválida.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); option = 0;
        }
    } while(option == 0);

    Clean_Ground(); // Limpia pantalla

    // Ejecuta la acción seleccionada
    switch(option){
    case 1:
        Player1(IA); // Llama a la función para jugar como P1
        break;
    case 2:
        Player2(IA); // Llama a la función para jugar como P2
        break;
    case 3:
        // Carga una nueva IA en el objeto IA actual (la que se pasó por referencia)
        IA.Read_IA(); // Llama al método Read_IA del objeto IA
        break;
    default: // Opción 4 (Salir)
        return; // Sale de esta función (vuelve al menú principal)
    }

    // Pausa después de jugar o cargar IA
    cout << "Presione Enter para continuar...";
    cin.clear(); // Limpia flags por si acaso
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();

    Clean_Ground(); // Limpia pantalla

    // Llama recursivamente a este menú para mostrarlo de nuevo
    return menu_Play(IA);
}

/**
 * @brief Simula una partida entre dos IAs (índices x e y en el vector Players).
 * @param x Índice de la IA que juega primero en esta llamada.
 * @param y Índice de la IA que juega segundo en esta llamada.
 * @param deep Profundidad actual del juego (número de turno, empieza en 1).
 * @param board Referencia al tablero (se modifica directamente).
 * @param Players Referencia al vector que contiene los objetos IA_t.
 * @return Índice del jugador ganador (x o y), o -1 si es empate.
 */
int Fight(int x, int y, int deep, vector<int>& board, vector<IA_T>& Players){
    // La IA 'x' elige su movimiento
    int ans = Players[x]->query(board);

    // Marca el movimiento en el tablero. Jugador = 2-(deep&1) -> 1 si deep es impar, 2 si deep es par
    board[ans] = 2 - (deep & 1);

    // Verifica si la IA 'x' ganó con este movimiento
    if(check_board_3D(board)) return x; // Retorna el índice de la IA ganadora

    // Verifica si el tablero está lleno (empate)
    if(deep == 26) return -1; // Si se alcanzó la profundidad máxima (26 jugadas), es empate

    // Si no hay ganador y no es empate, pasa el turno a la otra IA
    // Llama recursivamente intercambiando x e y, incrementando la profundidad
    return Fight(y, x, deep + 1, board, Players);
}

void Read_Seeds(vector<vector<int>>& Seeds){
    // Carga las semillas desde el archivo "Seeds.txt"
    ifstream fin("Seeds.txt");
    if (!fin.is_open()) { // Chequeo de apertura
        cerr << "Error: No se pudo abrir el archivo 'Seeds.txt'." << endl;
        return;
        }
    int tam; // Variable para leer el número de semillas
    fin >> tam; // Lee el número de semillas
    
    Seeds.resize(tam); // Ajusta el tamaño del vector de semillas
    cout << "Leyendo " << tam << " semillas desde Seeds.txt..." << endl;
    for(auto &v : Seeds){ // Itera para leer cada semilla
        v.resize(26); // Asegura tamaño 26 para cada semilla
        for(auto &u : v) { // Lee los 26 valores de la semilla
            fin >> u;
        }
    }
    fin.close(); // Cierra el archivo de semillas
    cout << "Semillas cargadas." << endl;
}

void Read_IAs(vector<IA_T>& Players){
    int tam; // Variable para leer el número de semillas/IAs
    
    // Obtiene el número de IAs a enfrentar
    cout << "Ingrese la cantidad de IAs a enfrentar: ";
    cin >> tam;
    if (cin.fail() || tam <= 0) {
        cerr << "Error: Cantidad inválida de IAs." << endl;
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); return;
    }

    // Carga cada IA desde su archivo correspondiente
    for(int i=0; i<tam; i++) {
        cout << "\n--- Cargando IA " << i+1 << " ---\n";
        auto new_IA = make_unique<IA_t>(); 
        new_IA->Read_IA();

        Players.push_back(std::move(new_IA));
    }
}

void printStadistics(vector<IA_T>& Players){
    cout << "--- Enfrentamientos Terminados ---\n";

    // Muestra los resultados finales
    cout << "\n--- Resultados Finales ---\n";
    // Abre archivo para resultados (opcional, también imprime en consola)
    ofstream fout_res("Answer.txt");
    for(auto &u : Players){ // Itera sobre las IAs
        // Imprime en consola
        cout << u->name_access << " W1:" << u->win1 << " W2:" << u->win2 << " D:" << u->draw << "\n";
        // Escribe en archivo si se pudo abrir
        if (fout_res.is_open()) {
            fout_res << u->name_access << ' ' << u->win1 << ' ' << u->win2 << ' ' << u->draw << "\n";
        }
        //u.draw=u.win1=u.win2=0;
    }
    if (fout_res.is_open()) fout_res.close(); // Cierra archivo de resultados
}
void Stadistics(vector<IA_T>& Players, vector<vector<int>>& Seeds){
    // Ejecuta los enfrentamientos (todos contra todos)
    cout << "\n--- Iniciando Enfrentamientos ---\n";
    int ans; // Variable para guardar el resultado de cada partida (índice ganador o -1)
    for(int i = 0; i < Players.size(); i++){ // IA i
        if(Players[i]->vis)continue;
        for(int j = 0; j < Players.size(); j++){ // IA j
            if(j == i || Players[j]->vis) continue; // Una IA no juega contra sí misma

            cout << "Enfrentando IA " << i+1 << " vs IA " << j+1 << "...\n";
            // Itera sobre cada tablero inicial (semilla)
            for(auto u : Seeds){ // 'u' es una copia de la semilla original
                // Llama a Fight: IA j empieza (índice 'j' va primero), IA i es la segunda.
                // El jugador que empieza (deep=1) marcará con 1 (2-(1&1)=1).
                ans = Fight(j, i, 2, u, Players);

                // Actualiza contadores según el resultado 'ans'
                if(ans == i) Players[i]->win1++;         // Ganó IA i (que jugó segunda, como P1 en 'Fight')
                else if(ans == j) Players[j]->win2++;    // Ganó IA j (que jugó primera, como P2 en 'Fight')
                else Players[j]->draw++;                 // Empate (-1), se le asigna a j
            }
        }
    }
}

void Tournament(vector<IA_T>& Players, vector<vector<int>>& Seeds){
    for(int i=1; i<Players.size(); i++){
        Stadistics(Players, Seeds);
        int lower=100000000, x=0;
        for(int j=0; j<Players.size(); j++){
            if(Players[j]->vis)continue;
            if(Players[j]->win1<lower)lower=Players[j]->win1, x=j;
        }

        auto &u=Players[x];
        cout<< "La IA "<<u->name_access<<" ha sido eliminada\n";
        cout<< "W1:" << u->win1 << " W2:" << u->win2 << " D:" << u->draw << "\n";
        u->vis=1;

        for(int j=0; j<Players.size(); j++){
            Players[j]->draw=Players[j]->win1=Players[j]->win2=0;
        }
    }

    int best1;
    
    for(int i=0; i<Players.size(); i++){
        auto &u=Players[i];
        if(!u->vis){
            best1=i;
            cout<<"El mejor jugador 1 es: "<<u->name_access<<"\n";
        }
        u->vis=0;
    }
    
    int best2, bestwin=-1;

    for(int i=0; i<Players.size(); i++){
        int win=0;
        for(auto &u:Seeds){
            int ans = Fight(i, best1, 2, u, Players);
            if(ans != i) win++;         
        }
        if(win>bestwin)bestwin=win, best2=i;
    }
    cout<<"El mejor jugador 2 es: "<<Players[best2]->name_access<<"\n";
}
/**
 * @brief Ejecuta el torneo entre múltiples IAs cargadas, usando semillas predefinidas.
 * Carga las IAs, carga las semillas, las enfrenta todas contra todas y muestra resultados.
 */
void Testing(){
    vector<IA_T> Players; // Vector para almacenar las IAs cargadas
    vector<vector<int>> Seeds; // Vector para almacenar los tableros iniciales

    Read_Seeds(Seeds);
    Read_IAs(Players);

    vector<string> Menu = {
        "Ver estadísticas",     // Opción 1
        "Obtener la mejor IA",  // Opción 2
        "Salir"                 // Opción 3
    };

    // Muestra las opciones
    cout << "--- Menu Principal (IA Tester) ---\n";
    for (int i = 0; i < Menu.size(); i++) {
        cout << "[" << i + 1 << "] " << Menu[i] << "\n";
    }

    int opcion = 0; // Opción del usuario

    // Bucle para obtener opción válida
    do {
        cout << "Ingrese la opcion (1-" << Menu.size() << "): ";
        cin >> opcion;
        if (cin.fail() || opcion < 1 || opcion > Menu.size()) {
            cout << "Opción inválida.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); opcion = 0;
        }
    } while (opcion == 0);

    Clean_Ground(); // Limpia pantalla

    // Ejecuta la acción seleccionada
    switch (opcion) {
    case 1:
        Stadistics(Players, Seeds); // Llama a la función para enfrentar múltiples IAs
        printStadistics(Players);
        break;
    case 2:
        Tournament(Players, Seeds);
        break;
    default: // Caso 3 (Salir)
        cout << "Saliendo del programa.\n";
        return; // Sale de menu()
    }

    // Pausa final
    cout << "\nPresione Enter para continuar...";
    cin.clear(); // Limpia flags
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Limpia buffer
    cin.get(); // Espera Enter
}

/**
 * @brief Muestra el menú principal del programa Tester.
 * Permite elegir entre enfrentar IAs, jugar contra una IA o salir.
 */
void menu() {
    // Opciones del menú principal
    vector<string> Menu = {
        "Enfrentar varias IAs", // Opción 1
        "Jugar contra una IA",  // Opción 2
        "Salir"                 // Opción 3
    };

    // Muestra las opciones
    cout << "--- Menu Principal (IA Tester) ---\n";
    for (int i = 0; i < Menu.size(); i++) {
        cout << "[" << i + 1 << "] " << Menu[i] << "\n";
    }

    int opcion = 0; // Opción del usuario

    // Bucle para obtener opción válida
    do {
        cout << "Ingrese la opcion (1-" << Menu.size() << "): ";
        cin >> opcion;
        if (cin.fail() || opcion < 1 || opcion > Menu.size()) {
            cout << "Opción inválida.\n";
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); opcion = 0;
        }
    } while (opcion == 0);

    Clean_Ground(); // Limpia pantalla

    // Ejecuta la acción seleccionada
    switch (opcion) {
    case 1:
        Testing(); // Llama a la función para enfrentar múltiples IAs
        break;
    case 2:
        { // Bloque para crear IA localmente para menu_Play
            IA_t IA_para_jugar; // Crea objeto IA
            cout << "Cargando IA para jugar...\n";
            IA_para_jugar.Read_IA(); // Pide al usuario que cargue la IA
            menu_Play(IA_para_jugar); // Llama al submenú de juego
        }
        break;
    default: // Caso 3 (Salir)
        cout << "Saliendo del programa.\n";
        return; // Sale de menu()
    }

    Clean_Ground(); // Limpia pantalla

    // Llama recursivamente al menú principal para volver a mostrarlo
    return menu();
}

/**
 * @brief Función principal del programa IA Tester.
 * @return 0 si el programa termina correctamente.
 */
int main() {
    menu(); // Inicia el menú principal

    return 0; // Fin del programa
}
