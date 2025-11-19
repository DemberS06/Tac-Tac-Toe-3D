#include "IA_Tools.h"  // Incluye la estructura IA_t y la función LoadBaseData
#include <vector>      
#include <string>     
#include <utility>     
#include <iostream>   
#include <limits>     
#include <ios>         

using namespace std; // Usa el espacio de nombres std para evitar escribir std::

/**
 * @brief Limpia la pantalla de la consola enviando secuencias de escape ANSI.
 * Intenta limpiar y mover el cursor a la esquina superior izquierda 3 veces.
 * (Funciona en terminales compatibles con ANSI como Linux, macOS, Windows Terminal moderno).
 */
void Clean_Ground() {
    // Envía el código ANSI para limpiar pantalla (2J) y mover cursor a home (1;1H)
    cout << "\033[2J\033[1;1H";
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
    cout << "\033[2J\033[1;1H"; // Repite por si acaso
}

/**
 * @brief Muestra el menú principal y maneja la selección del usuario.
 * Permite cargar o crear una IA, carga los datos de entrenamiento si es necesario,
 * inicia el entrenamiento y luego se llama recursivamente para volver al menú.
 * @param Seeds Referencia a un vector de pares que contiene los datos de entrenamiento.
 * Se carga desde archivo si está vacío.
 */
void menu(vector<pair<vector<long double>, vector<long double>>>& Seeds) {
    // Define las opciones del menú
    vector<string> Menu = {
        "Retomar IA",    // Opción 1: Cargar una IA existente
        "Crear IA",      // Opción 2: Crear una IA nueva
        "Salir"          // Opción 3: Terminar el programa
    };

    // Muestra las opciones del menú al usuario
    cout << "--- Menu Principal ---\n";
    for (int i = 0; i < Menu.size(); i++) {
        // Imprime opción [índice+1] Texto
        cout << "[" << i + 1 << "] " << Menu[i] << "\n";
    }

    int opcion = 0; // Variable para almacenar la opción elegida por el usuario

    // Bucle para obtener una opción válida del usuario
    do {
        cout << "Ingrese la opcion (1-" << Menu.size() << "): ";
        cin >> opcion;
        // Verifica si la entrada fue un número válido y está dentro del rango permitido
        if (cin.fail() || opcion < 1 || opcion > Menu.size()) {
            cout << "Opción inválida.\n";
            cin.clear(); // Limpia los flags de error de cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Descarta la entrada inválida del buffer
            opcion = 0; // Resetea la opción para que el bucle continúe
        }
    } while (opcion == 0); // Repite si la opción no fue válida

    Clean_Ground(); // Limpia la pantalla antes de ejecutar la acción

    // Crea un objeto IA_t (se creará uno nuevo en cada llamada recursiva a menu)
    IA_t IA;
    
    // Ejecuta la acción según la opción seleccionada
    switch (opcion) {
        case 1: // Retomar IA
        cout << "--- Retomar Entrenamiento de IA ---\n";
        IA.Read_IA(); // Llama al método público para cargar la IA desde archivo
        break;
        case 2: // Crear IA
        cout << "--- Crear Nueva IA ---\n";
        IA.Make_IA(); // Llama al método público para crear una nueva IA
        break;
        default: // Caso 3 (Salir) o si la carga de datos falló
        cout << "Saliendo del programa.\n";
        return; // Sale de la función menu (y por tanto de main)
    }
    
    // Verifica si los datos de entrenamiento (Seeds) ya han sido cargados
    if (Seeds.empty()) {
        // Llama a la función (declarada en IA_Tools.h) para cargar los datos
        Seeds = LoadBaseData();
        // Verifica si la carga falló (LoadBaseData devolvió un vector vacío)
        if (Seeds.empty()) {
            cerr << "Error crítico: No se pudieron cargar los datos de entrenamiento. Saliendo.\n";

            // Pausa para que el usuario vea el error
            cout << "Presione Enter para continuar...";
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get(); // Espera Enter

            return;
        } else {
            cout << "Datos cargados exitosamente: " << Seeds.size() << " ejemplos.\n";
        }
    }

    IA.Train(Seeds); // Llama al método público de entrenamiento

    // Pausa para que el usuario vea los resultados del entrenamiento o mensajes
    cout << "Entrenamiento (o intento) finalizado. Presione Enter para volver al menú...";
    // Limpia buffer antes de esperar Enter (importante después de cin >>)
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get(); // Espera Enter
    

    Clean_Ground(); // Limpia la pantalla después de ejecutar la opción

    // Llama recursivamente al menú para mostrarlo de nuevo
    // Nota: Esto crea una nueva instancia de IA_t en cada llamada.
    return menu(Seeds);
}

/**
 * @brief Función principal del programa de entrenamiento.
 * Inicializa el vector de datos y llama al menú principal.
 * @return 0 si el programa termina correctamente.
 */
int main() {
    // Vector para almacenar los datos de entrenamiento (se pasa por referencia a menu)
    vector<pair<vector<long double>, vector<long double>>> Seeds;

    menu(Seeds); // Inicia el menú principal

    return 0; // Retorno estándar para main
}
