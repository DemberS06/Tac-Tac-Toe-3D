# 🎮 Tac-Tac-Toe 3D — Motor de Juego + Generador de Bases de Datos + IA

Este proyecto implementa un sistema completo para analizar, evaluar y jugar **3 en línea en un cubo tridimensional**.  
Incluye herramientas de backtracking, generación de bases de datos optimizadas, verificadores de estados y experimentos con redes neuronales en C++.

---

## 🌟 Características principales

### 🔹 1. **Motor del Juego 3D**
- Representación del tablero en 3 dimensiones.
- Detección de líneas ganadoras en todas las direcciones (ejes, diagonales internas y diagonales espaciales).
- Codificación compacta en **base 3** para identificar estados únicos.

### 🔹 2. **Backtracking avanzado**
- Algoritmo completo para determinar:
  - El ganador desde cualquier estado dado.
  - Distancia mínima a la victoria/derrota.
  - Jugadas óptimas.
- Heurísticas de poda para acelerar la búsqueda en tableros grandes.

### 🔹 3. **Generación masiva de bases de datos**
- Producción de archivos gigantes de estados:
  - Estructurados
  - No estructurados
  - Comprimidos
- Organización por niveles (*G3, G4, … G10, G11, W1, etc.*).

> ⚠️ *Los archivos de bases de datos grandes no se incluyen en el repositorio para evitar tamaños excesivos.*

### 🔹 4. **IA basada en redes neuronales**
- Entrenamiento con archivos generados.
- Arquitectura flexible con capas configurables.
- Entrenamiento multihilo con gradientes protegidos por mutex.
- Exportación e importación de pesos.

### 🔹 5. **Apps integradas**
En la carpeta `/Apps/` se incluyen programas como:
- Generación de datos
- Testing
- Verificadores de semillas
- IA Trainer
- IA Tester

---

## 📂 Estructura del repositorio
├── Apps/
│ ├── Generacion_de_Datos/ # Scripts que producen las bases de datos (muy pesadas)
│ ├── Testing/ # Programas de prueba
│ └── Play/ # Herramientas para jugar o visualizar
│
├── Programas/ # Código del motor del juego e IA
│ ├── GameTools.cpp/.h # Backtracking y análisis del juego
│ ├── IA_Tools.cpp/.h # Red neuronal
│ └── Base_de_Datos.cpp # Lógica para generar/verificar estados
│
└── Redes_Neuronales/ # Pesos guardados de las IAs

