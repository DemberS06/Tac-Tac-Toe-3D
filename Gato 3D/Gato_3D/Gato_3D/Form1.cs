using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Globalization;

//Cambia "Gato_3D" a el nombre de tu archivo antes de correr
namespace Gato_3D
{
    public partial class Form1 : Form
    {
        List<int> lay = new List<int>();//arquitectura de la red neuronal, el numero de neuronas de cada capa
        List<List<List<double>>> weights = new List<List<List<double>>>();//almacena los pesos de las conexiones entre las neuronas de la red neuronal
        List<List<double>> biases = new List<List<double>>();//sesgos de las neuronas
        private Random rnd = new Random();
        private bool redCargada = false;
        /*CLASSES-----------------------------------------------------------------------------------
        Polygon data contiene el color y vértices de un polígono, usado en los cubos y el hexágono
        Casilla es cada uno de los 9 cuadrados dentro de una cara del cubo, contiene id, color y si
            ya fue presionado (El color solo es visual, no se utilizará en el código para verificar ganadores)
        Face son las caras del cubo(6), contiene una matriz de 9 casillas, id y links a las caras de
            izquirda y derecha (para las caras laterales, las caras de arriba y abajo no utilizarán links)*/
        class PolygonData
        {
            public Point[] Points { get; set; }
            public Brush FillColor { get; set; }

            public PolygonData(Point[] points, Brush color)
            {
                Points = points;
                FillColor = color;
            }
        }
        class casilla
        {
            /*Representa una unica casilla en una de las caras del cubo, contiene su id 0-26, excluyendo el 12? centro*/
            public int id { get; set; }
            public Brush color { get; set; }
            public bool enabled { get; set; } = true;//saber si ya ha sido presionada, su edo
            public casilla(int setId, Brush setColor)
            {
                id = setId;
                color = setColor;
            }
        }
        class face
        {
            public casilla[,] board { get; set; }
            public face[] links { get; set; } = new face[2]; //0=izquierda 1=derecha 2=arriba 3=abajo
            public int faceid;

            public face(casilla[,] setBoard, face[] setLinks, int setFaceid)
            {
                board = setBoard;
                links = setLinks;//conectar las cara de la iz con der
                faceid = setFaceid;//identificar cara
            }
        }
        public Form1()
        {
            InitializeComponent();
            this.DoubleBuffered = true;
        }
        /*GENERADOR DEL JUEGO--------------------------------------------------------------
        El tamaño de la ventana cambiará según el tamaño de los elementos automáticamente
           (encontrados en la sección de variables dentro de Form1_Load)
        Genera todo, elementos visuales como los cubos, el hexagono y el controlador
            y otros elementos internos o no visibles--------------------------------------*/

        //0-topleft 1-bottomleft 2-topright 3-bottomright 4-top- 5-bottom
        PolygonData[,,] cubefaces = new PolygonData[6, 3, 3]; //Polígonos de los cubos 3D
        PolygonData[] hexatris = new PolygonData[7];        //Polígonos del Hexágono / Cubo transparente
        face[] faces = new face[6];                         //Las caras del cubo y su información
        face[] layers = new face[3];                        //Las 3 caras que alternarán para jugar (arriba(0), lateral(1) y abajo(2))
        int activeLayer = 1;                                //La cara en estado activo de las 3 caras que alternan
        int[] seed = new int[27];                           //La semilla de los movimientos del juego (usador por la ia y para verificar ganadores)
        int activePlayer = 1;                               //El jugador actual, cambiará entre 1 y 2
        int moves = 0;                                      //Los movimientos jugados, para decir en cuánto se ganó e indicar un empate
        Button[,] boardBtns = new Button[3, 3];              //Los 9 botones en el controlador que representan casillas
        Button[] controlBtns = new Button[4];               //Los 4 botones para navegar el cubo
        Dictionary<int, List<casilla>> cubits = new Dictionary<int, List<casilla>>(); //Aqui se guardan todas las referencias de casillas con ids idénticas
        Brush[] playerColors = { Brushes.CadetBlue, Brushes.Peru }; //Los colores respectivos de cada jugador
        private void Form1_Load(object sender, EventArgs e)
        {
            //VARIABLES====================================================================
            //Tamaño de los elementos:
            int cubes_poly_height = 40;         //cubos 3D
            int hexagon_tri_height = 80;        //hexágono / cubo transparente
            int controller_side_lenght = 400;   //contenedor del controlador

            Point[] points, subset;
            int h, originx, originy, tempx, tempy;
            //=============================================================================

            //Ajustar la configuración del form
            this.BackColor = Color.DimGray;
            Width = 8 * cubes_poly_height + 3 * hexagon_tri_height + controller_side_lenght;
            Height = 16 * cubes_poly_height;

            //3D CUBES------------------------------------------------------------------------------------------
            h = cubes_poly_height; originx = Width - h * 9 / 2; originy = h;
            points = new Point[4];
            for (int y = 0; y < 3; y++)
            { //x
                for (int x = 0; x < 3; x++)
                { //y
                    for (int k = 0; k < 2; k++)
                    { //3 funciones usadas por las 6 caras
                        //Top & bottom faces
                        tempx = originx + (x - y) * h; tempy = originy + (x + y) * h / 2 + k * 10 * h;
                        points[0] = new Point(tempx, tempy);
                        points[1] = new Point(tempx - h, tempy + h / 2);
                        points[2] = new Point(tempx, tempy + h);
                        points[3] = new Point(tempx + h, tempy + h / 2);
                        cubefaces[k + 4, x, y] = new PolygonData((Point[])points.Clone(), Brushes.Beige);

                        //Topleft & Bottomright faces
                        tempx = originx + h * ((k == 0) ? x - 3 : x); tempy = Convert.ToInt32(originy + h * (((k == 0) ? 1.5f : 7) + y + x / 2f));
                        points[0] = new Point(tempx, tempy);
                        points[1] = new Point(tempx, tempy + h);
                        points[2] = new Point(tempx + h, tempy + h * 3 / 2);
                        points[3] = new Point(tempx + h, tempy + h / 2);
                        cubefaces[k * 3, x, y] = new PolygonData((Point[])points.Clone(), Brushes.Beige);

                        //Topright & Bottomleft faces
                        tempx = originx + h * ((k == 0) ? x : x - 3); tempy = Convert.ToInt32(originy + h * (((k == 0) ? 3 : 8.5f) + y - x / 2f));
                        points[0] = new Point(tempx, tempy);
                        points[1] = new Point(tempx, tempy + h);
                        points[2] = new Point(tempx + h, tempy + h / 2);
                        points[3] = new Point(tempx + h, tempy - h / 2);
                        cubefaces[2 - k, x, y] = new PolygonData((Point[])points.Clone(), Brushes.Beige);
                    }
                }
            }

            //HEXAGON / TRANSPARENT CUBE-----------------------------------------------------------------------
            h = hexagon_tri_height; originx = h + hexagon_tri_height / 2; originy = Height / 2;
            points = new Point[8];
            subset = new Point[3];
            points[0] = new Point(originx, originy);
            points[1] = new Point(originx + h, originy - h / 2);
            points[2] = new Point(originx, originy - h);
            points[3] = new Point(originx - h, originy - h / 2);
            points[4] = new Point(originx - h, originy + h / 2);
            points[5] = new Point(originx, originy + h);
            points[6] = new Point(originx + h, originy + h / 2);
            points[7] = points[1]; //Punto extra para evitar overflow y ahorrar complejidad

            for (int i = 0; i < 6; i++)
            {
                subset[0] = points[0];
                subset[1] = points[i + 1];
                subset[2] = points[i + 2];
                hexatris[i] = new PolygonData((Point[])subset.Clone(), Brushes.DarkGray);
            }
            //Slot extra para que al calcular cuales usa evitar overflow y ahorrar complejidad
            hexatris[6] = hexatris[0];

            //CONTROLLER-----------------------------------------------------------------------------------
            h = controller_side_lenght / 5;
            originx = 3 * hexagon_tri_height + h;
            originy = Height / 2 - h * 2;
            tempx = h * 3 / 4;
            tempy = h * 1 / 8;
            points = new Point[] {new Point(originx - h + tempy, originy + h + tempy),
                                  new Point(originx + 3*h + tempy, originy + h + tempy),
                                  new Point(originx + h + tempy, originy - h + tempy),
                                  new Point(originx + h + tempy, originy + 3*h + tempy)};
            string[] texts = new string[] { "←", "→", "↑", "↓" };
            //Generar los botones de las casillas
            for (int y = 0; y < 3; y++)
            {
                for (int x = 0; x < 3; x++)
                {
                    Button btn = new Button();
                    btn.Width = h; btn.Height = h;
                    btn.Location = new Point(originx + x * h, originy + y * h);
                    btn.Font = new Font("Times New Roman", 16, FontStyle.Bold);
                    btn.AutoSize = false;
                    btn.Click += BoardClick;
                    boardBtns[x, y] = btn;
                    Controls.Add(btn);
                }
            }
            //Generar los botones de movimiento
            for (int i = 0; i < 4; i++)
            {
                Button btn = new Button();
                btn.Width = tempx; btn.Height = tempx;
                btn.Location = points[i];
                btn.Text = texts[i];
                btn.Name = i.ToString();
                btn.Font = new Font("Times New Roman", 16, FontStyle.Bold);
                btn.BackColor = Color.Beige;
                btn.AutoSize = false;
                if (i < 2) btn.Click += BtnClickRotate;
                else btn.Click += BtnClickSwitchLayer;
                controlBtns[i] = btn;
                Controls.Add(btn);
            }

            //CARAS---------------------------------------------------------------------------------------
            //Nota: las ids de las caras están asignadas de forma correspondiente a los visuales del hexágono / cubo transparente
            //      Arriba-0 Abajo-3, el resto de las caras laterales son del 1-5,  saltandose el 3
            casilla[,] ids = new casilla[3, 3];
            int add = 0;
            face[] links = new face[2] { null, null };

            //Generar las caras de arriba y abajo, que no tienen un patron muy fijo
            int[,] intes = new int[2, 9] {{2,23,20,5,25,17,8,11,14},
                                          {12,15,18,9,24,21,6,3,0}};
            for (int i = 0; i < 2; i++)
            {
                for (int y = 0; y < 3; y++)
                    for (int x = 0; x < 3; x++)
                        ids[x, y] = new casilla(intes[i, x + y * 3], Brushes.Beige);
                faces[i * 3] = new face((casilla[,])ids.Clone(), (face[])links.Clone(), i * 3);
            }
            //Generar el resto de las caras con un patrón
            for (int i = 1; i < 6; i++)
            {
                for (int y = 0; y < 3; y++)
                    for (int x = 0; x < 3; x++)
                        ids[x, y] = new casilla(2 - y + x * 3 + add, Brushes.Beige);
                if (i == 3) i++;
                add += 6;
                face car = new face((casilla[,])ids.Clone(), (face[])links.Clone(), i);
                faces[i] = car;
                links[0] = car;
                faces[i - (i != 4 ? 1 : 2)].links[1] = car;
            }
            //Arreglar la última cara lateral para hacer un tipo de lista circular
            //Arreglar las ids
            for (int i = 0; i < 3; i++)
                faces[5].board[2, i].id = 2 - i;
            //Arreglas los links
            faces[1].links[0] = faces[5];
            faces[5].links[1] = faces[1];

            //Meter todas las casillas en el diccionario de ids idénticas (se podría hacer dentro de los loops, decidí no hacerlo para ser menos confuso)
            foreach (face f in faces)
            {
                foreach (casilla c in f.board)
                {
                    if (!cubits.ContainsKey(c.id))
                        cubits[c.id] = new List<casilla>();
                    cubits[c.id].Add(c);
                }
            }

            //Conectar los punteros de las capas a las caras correspondientes (arriba, lateral y abajo)
            layers[0] = faces[0];
            layers[1] = faces[2];
            layers[2] = faces[3];

            ReloadVisuals();
        }

        /*FUNCIONES-------------------------------------------------------------------------------------------
        OnPaint es una función ya existente, solo se le agrega que renderice los cubos 3d y el hexágono
        ReloadVisuals recarga todos los gráficos, usados despues de cada movimiento para renderizar el cubo y botones
        Para verificar ganador, se dividió en dos funciones, checkwinner, que utiliza threeInRow:
                    checkwinner, por cada cara, crea una matirz con las jugadas en base a la semilla
                    threeInRow verifica si esa cara tiene algún tres en línea ganador
        Rotate board se utiliza en las caras de arriba y abajo, cuando rota el cubo, sus matrices también rotarán
        BtnClickRotate es llamada por los botones laterales, rotan las caras de arriba y recorren la cara lateral
        BtnClickSwitchLayer es llamada por los botones de arriba y abajo, alterna entre las 3 caras para jugar
            (Arriba, lateral y abajo) ------------------------------------------------------------------------
        */
        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            Graphics g = e.Graphics;

            foreach (PolygonData face in cubefaces)
            {
                g.FillPolygon(face.FillColor, face.Points);
                g.DrawPolygon(Pens.DarkCyan, face.Points);
            }
            foreach (PolygonData tri in hexatris)
            {
                g.FillPolygon(tri.FillColor, tri.Points);
                g.DrawPolygon(Pens.Wheat, tri.Points);
            }
        }
        void ReloadVisuals()
        {
            for (int y = 0; y < 3; y++)
                for (int x = 0; x < 3; x++)
                {
                    //Pinta el cubo 3d con las caras correspondientes
                    cubefaces[4, x, y].FillColor = layers[0].board[x, y].color;
                    cubefaces[0, x, y].FillColor = layers[1].board[x, y].color;
                    cubefaces[1, x, y].FillColor = layers[1].board[x, y].color;
                    cubefaces[2, x, y].FillColor = layers[1].links[1].board[x, y].color;
                    cubefaces[3, x, y].FillColor = layers[1].links[1].board[x, y].color;
                    cubefaces[5, x, y].FillColor = layers[2].board[x, y].color;

                    //Configura los botones para correpsonder con la cara activa
                    boardBtns[x, y].BackColor = ((SolidBrush)layers[activeLayer].board[x, y].color).Color;
                    boardBtns[x, y].Text = (layers[activeLayer].board[x, y].id + 1).ToString();
                    boardBtns[x, y].Enabled = layers[activeLayer].board[x, y].enabled;
                }
            //Pinta el cubo transparente con la cara activa
            //Nota: las funciónes de los botones de movimiento despintan la última cara activa
            hexatris[layers[activeLayer].faceid].FillColor = Brushes.Orange;
            hexatris[layers[activeLayer].faceid + 1].FillColor = Brushes.Orange;
            this.Invalidate(); //Función de C# para llamar a re- renderizar el form
        }
        bool threeInRow(int[,] v)
        {
            for (int i = 0; i < 3; i++)
            {
                if (v[i, 0] != 0 && v[i, 0] == v[i, 1] && v[i, 1] == v[i, 2]) return true;
            }

            for (int j = 0; j < 3; j++)
            {
                if (v[0, j] != 0 && v[0, j] == v[1, j] && v[1, j] == v[2, j]) return true;
            }

            if (v[0, 0] != 0 && v[0, 0] == v[1, 1] && v[1, 1] == v[2, 2]) return true;
            if (v[0, 2] != 0 && v[0, 2] == v[1, 1] && v[1, 1] == v[2, 0]) return true;
            return false;
        }
        bool checkWinner()
        {
            // Representación de las 6 caras del cubo
            int[][,] matFaces = new int[6][,]
            {
        // Cara 1 (arriba)
        new int[,] {{ 0, 1, 2 }, { 3, 4, 5 }, { 6, 7, 8 }},
        
        // Cara 2 (izquierda)
        new int[,] {{ 0, 3, 6 }, { 21, 24, 9 }, { 18, 15, 12 }},
        
        // Cara 3 (frontal)
        new int[,] {{ 6, 7, 8 }, { 9, 10, 11 }, { 12, 13, 14 }},
        
        // Cara 4 (derecha)
        new int[,] {{ 8, 5, 2 }, { 11, 25, 23 }, { 14, 17, 20 }},
        
        // Cara 5 (abajo)
        new int[,] {{ 12, 13, 14 }, { 15, 16, 17 }, { 18, 19, 20 }},
        
        // Cara 6 (atrás)
        new int[,] {{ 18, 19, 20 }, { 21, 22, 23 }, { 0, 1, 2 }}
            };

            // Comprobar cada cara
            for (int face = 0; face < 6; face++)
            {
                int[,] mat = matFaces[face];

                // Comprobar filas
                for (int i = 0; i < 3; i++)
                {
                    if (seed[mat[i, 0]] != 0 &&
                        seed[mat[i, 0]] == seed[mat[i, 1]] &&
                        seed[mat[i, 1]] == seed[mat[i, 2]])
                    {
                        return true;
                    }
                }

                // Comprobar columnas
                for (int j = 0; j < 3; j++)
                {
                    if (seed[mat[0, j]] != 0 &&
                        seed[mat[0, j]] == seed[mat[1, j]] &&
                        seed[mat[1, j]] == seed[mat[2, j]])
                    {
                        return true;
                    }
                }

                // Comprobar diagonales
                if (seed[mat[0, 0]] != 0 &&
                    seed[mat[0, 0]] == seed[mat[1, 1]] &&
                    seed[mat[1, 1]] == seed[mat[2, 2]])
                {
                    return true;
                }

                if (seed[mat[0, 2]] != 0 &&
                    seed[mat[0, 2]] == seed[mat[1, 1]] &&
                    seed[mat[1, 1]] == seed[mat[2, 0]])
                {
                    return true;
                }
            }

            return false;
        }
        casilla[,] RotateBoard(casilla[,] board, int direction)
        {
            //0 rota a la izquierda, 1 a la derecha
            int x1, y1;
            casilla[,] newboard = (casilla[,])board.Clone();
            if (direction == 0)
            {
                for (int y = 0; y < 3; y++)
                {
                    x1 = 2 - y;
                    for (int x = 0; x < 3; x++)
                    {
                        y1 = x;
                        newboard[x, y] = board[x1, y1];
                    }
                }
            }
            else
            {
                for (int y = 0; y < 3; y++)
                {
                    x1 = y;
                    for (int x = 0; x < 3; x++)
                    {
                        y1 = 2 - x;
                        newboard[x, y] = board[x1, y1];
                    }
                }
            }
            return newboard;
        }
        void PlayIA()
        {
            List<double> entradaIA = new List<double>(); //iguala a seed que 
            for (int i = 0; i < 26; i++)
                entradaIA.Add(seed[i]);

            // Obtener el mejor movimiento de la IA usando la nueva función
            int mejorMovimientoIA = Posicion(entradaIA);

            foreach (casilla c in cubits[mejorMovimientoIA])
            {
                seed[c.id] = activePlayer;
                c.color = playerColors[activePlayer - 1];
                c.enabled = false;
            }
            moves++;
            ReloadVisuals();

            if (checkWinner())
            {
                MessageBox.Show("El jugador " + activePlayer + " ganó!!\nMovimientos: " + moves);
                endGame();
                return;
            }
            if (moves == 26)
            {
                MessageBox.Show("El juego terminó en un empate :/");
                endGame();
                return;
            }

            activePlayer = (activePlayer == 1 ? 2 : 1);
        }
        void BoardClick(object sender, EventArgs e)
        {
            Button btn = (Button)sender;
            foreach (casilla c in cubits[Convert.ToInt16(btn.Text) - 1])
            {
                seed[c.id] = activePlayer;
                c.color = playerColors[activePlayer - 1];
                c.enabled = false;
            }
            moves++;
            ReloadVisuals();

            if (checkWinner())
            {
                MessageBox.Show("El jugador " + activePlayer + " ganó!!\nMovimientos: " + moves);
                endGame();
                return;
            }
            if (moves == 26)
            {
                MessageBox.Show("El juego terminó en un empate :/");
                endGame();
                return;
            }
            activePlayer = (activePlayer == 1 ? 2 : 1);//elige
            PlayIA();
        }
        void BtnClickRotate(object sender, EventArgs e)
        {
            //Despintar el hexágono
            hexatris[layers[activeLayer].faceid].FillColor = Brushes.DarkGray;
            hexatris[layers[activeLayer].faceid + 1].FillColor = Brushes.DarkGray;
            //Cambiar las capas (recorrer las caras laterales, rotas las de arriba y abajo)
            Button btn = (Button)sender;
            if (btn.Name == "0")
            {
                layers[1] = layers[1].links[1];
                layers[0].board = RotateBoard(layers[0].board, 1);
                layers[2].board = RotateBoard(layers[2].board, 0);
            }
            else
            {
                layers[1] = layers[1].links[0];
                layers[0].board = RotateBoard(layers[0].board, 0);
                layers[2].board = RotateBoard(layers[2].board, 1);
            }
            ReloadVisuals();
        }
        void BtnClickSwitchLayer(object sender, EventArgs e)
        {
            //Despintar el hexágono
            hexatris[layers[activeLayer].faceid].FillColor = Brushes.DarkGray;
            hexatris[layers[activeLayer].faceid + 1].FillColor = Brushes.DarkGray;
            //Cambiar cúal es la capa activa y desactivar botones si es necesario
            Button btn = (Button)sender;
            if (btn.Name == "2")
            {
                if (activeLayer == 2) controlBtns[3].Enabled = true;
                activeLayer--;
                if (activeLayer == 0) controlBtns[2].Enabled = false;
            }
            else
            {
                if (activeLayer == 0) controlBtns[2].Enabled = true;
                activeLayer++;
                if (activeLayer == 2) controlBtns[3].Enabled = false;
            }
            ReloadVisuals();
        }
        void endGame()
        {
            Button btn = new Button();
            btn.Height = 80;
            btn.Location = new Point(30, 30);
            btn.Font = new Font("Times New Roman", 16, FontStyle.Bold);
            btn.Text = ("Reiniciar el juego");
            btn.AutoSize = true;
            btn.BackColor = Color.Beige;
            btn.Click += restartGame;
            Controls.Add(btn);

            foreach (face f in faces)
            {
                foreach (casilla c in f.board)
                {
                    c.enabled = false;
                }
            }
            ReloadVisuals();
        }
        double sigmoid(double x)
        {
            return 1.0 / (1.0 + Math.Exp(-x));
        }
        List<double> Query(List<double> input)
        {
            var ans = new List<double>(input); //var == auto

            for (int l = 0; l < weights.Count; l++)
            {
                List<double> res = new List<double>(new double[lay[l + 1]]);

                for (int i = 0; i < lay[l + 1]; i++)
                {
                    double sum = biases[l][i];

                    for (int j = 0; j < lay[l]; j++)
                    {
                        sum += weights[l][i][j] * ans[j];
                    }
                    res[i] = sigmoid(sum);
                }
                ans=res;
            }
            
            return ans;
        }
        public void Init()
        {
            // Resetea la variable redCargada para indicar que la IA está en proceso de carga
            // Leemos todo el contenido del archivo "1.txt" y separamos los tokens (números) por espacios y saltos de línea.
            string fileContent = File.ReadAllText("tryhard.txt");
            string[] tokens = fileContent.Split(new char[] { ' ', '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
            int pos = 0;

            // Se lee la cantidad de capas (n)
            int n = int.Parse(tokens[pos++]);

            // Se inicializan las listas con la cantidad adecuada de elementos.
            lay = new List<int>(n);
            biases = new List<List<double>>(n - 1);
            weights = new List<List<List<double>>>(n - 1);

            bool exists = File.Exists("tryhard.txt");
            Console.WriteLine("¿Existe tryhard.txt en el directorio de salida? " + exists);

            string contenido = File.ReadAllText("tryhard.txt");
            Console.WriteLine("Contenido del archivo:\n" + contenido);

            // Lectura de las dimensiones de cada capa.
            for (int i = 0; i < n; i++)
            {
                lay.Add(int.Parse(tokens[pos++]));
            }

            // Se crean las estructuras para pesos y sesgos según las dimensiones de las capas.
            for (int l = 0; l < n - 1; l++)
            {
                int rows = lay[l + 1];
                int cols = lay[l];

                // Inicializa la matriz de pesos para la capa actual
                var weightLayer = new List<List<double>>(rows);
                // Inicializa el vector de sesgos para la capa siguiente
                var biasLayer = new List<double>(rows);

                for (int r = 0; r < rows; r++)
                {
                    // Se crea un vector (fila) para los pesos con "cols" columnas y se inicializa en cero.
                    var rowList = new List<double>(new double[cols]);
                    weightLayer.Add(rowList);
                    // Se asigna un sesgo inicial (0) para la neurona correspondiente.
                    biasLayer.Add(0);
                }

                weights.Add(weightLayer);
                biases.Add(biasLayer);
            }

            // Lectura de los valores de los pesos.
            for (int l = 0; l < weights.Count; l++)
            {
                for (int r = 0; r < weights[l].Count; r++)
                {
                    for (int c = 0; c < weights[l][r].Count; c++)
                    {
                        weights[l][r][c] = double.Parse(tokens[pos++]);
                    }
                }
            }

            // Lectura de los valores de los sesgos.
            for (int l = 0; l < biases.Count; l++)
            {
                for (int r = 0; r < biases[l].Count; r++)
                {
                    biases[l][r] = double.Parse(tokens[pos++]);
                }
            }
        }        
        public int Posicion(List<double> que) //que es seed
        {
            //igual que a seed 
           // MessageBox.Show("NO");
            if (!redCargada)
            {//break
                Init();
                redCargada = true;
            }

            List<double> res = Query(que);

            // Encuentra el mejor movimiento
            double maxVal = -1;//siempre encuentra
            int mejorMovimiento = -1;

            for (int i = 0; i < 26; i++)
            {
                if (que[i] == 0 && res[i] > maxVal)
                {
                    maxVal = res[i];
                    mejorMovimiento = i;
                }
            }

            return mejorMovimiento; // Retorna índice base 0 (0-25)
        }  
        void restartGame(object sender, EventArgs e)
        {
            activePlayer = 1;
            Button btn = (Button)sender;
            Controls.Remove(btn);

            foreach (face f in faces)
            {
                foreach (casilla c in f.board)
                {
                    c.enabled = true;
                    c.color = Brushes.Beige;
                }
            }
            Array.Clear(seed, 0, 27);
            moves = 0;
            ReloadVisuals();
        }
    }
}