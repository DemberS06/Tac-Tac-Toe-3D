/*
    Este codigo nos ayuda a reestructurar la Base de Datos
*/
#include <bits/stdc++.h>

using namespace std;

ifstream fin("Base_de_Datos_NO_Estructurada.txt");  // Archivo de entrada: Base de Datos no estructurada
ofstream fout("Base_de_Datos.txt");                 // Archivo de salida: Base de Datos reestructurada

long long semillas, x, i, y, z, tableros, estrategias, player, optimo, deep;
map<long long, vector<int>> Move; // Guardar La profundidad de los movimientos posibles

int main(){
    cin.tie(0); cout.tie(0); ios_base::sync_with_stdio(0);
    fin>>estrategias; // Leemos la cantidad de estrategias revisadas en el creador de la base de datos 
    while(estrategias--){
        fin>>semillas; // Por cada estrategia se revisa una cantidad de semillas
        while(semillas--){
            fin>>tableros; // Por cada semilla con una respectiva estrategia genera una cantidad de tableros
            while(tableros--){
                deep=0;
                for(x=0, i=0, y=1; i<26; i++, y*=3){ // Leemos el Id del tablero
                    fin>>z;
                    x+=y*z;
                    deep+=(z>0);
                }
                player=1+(deep&1);
                
                if(Move[x].empty())Move[x].resize(26, 0); // Si es la primera vez que nos da el tablero lo inicializamos
    
                for(i=0; i<26; i++){ // Leemos los movimientos
                    fin>>y;
                    if(y==0)continue; // Si no es un movimiento optimo no lo usamos
                    if(player==1)Move[x][i]=min(max(Move[x][i], 1)+0ll, y); // Si eres jugador 1 te conviene minimizar la distancia
                    else Move[x][i]=max(Move[x][i]+0ll, y); // Si eres el jugador 2 te conviene maximizar la cantidad de movimientos
                }
            }
        }
    }

    vector<long long> f;

    for(auto &[u,v]:Move){ // Si estado no tiene movimientos optimos es un caso basura, entonces se deshecha
        x=0;
        for(auto &w:v)x+=w;
        if(!x)f.push_back(u);
    }
    for(auto &u:f)Move.erase(u);

    fout<<Move.size()<<"\n";

    for(auto &[u, v]:Move){ // Imprimimos la base de datos
        deep=0;
        for(i=0, x=u; i<26; i++, deep+=(x%3)>0, x/=3)fout<<x%3<<' ';
        player=1+(deep&1);
        if(player==1){ // Si eres el jugador 1 quieres minimizar la cantidad de movimientos
            optimo=50;
            for(auto &w:v)if(w)optimo=min(optimo, w+0ll); 
        }
        else{ // Si eres jugador 2 quieres maximizar la cantidad de movimientos
            optimo=0;
            for(auto &w:v)optimo=max(optimo, w+0ll);
        }
        
        for(auto &w:v)fout<<(w==optimo)<<' ';
    }

    return estrategias+1;
}
