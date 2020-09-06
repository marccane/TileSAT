#ifndef TILESAT_H
#define TILESAT_H
#include "tile.h"
#include "clausules.h"
#include "utils.h"

class TileSAT
{
    public:
        TileSAT(const string &);
        TileSAT(const string &, int, int);
        void doTiles(bool printSolution, bool drawTiles, bool solve=true);

    private:
        void generarTiles();
        void generarClausCondicionalHor(unsigned initVar);
        void generarClausCondicionalVertical(unsigned initVar);
        vector<Tile> llegirTiles(const string &filename);
        vector<bool> llegirResultatMinisat(const string &filename) const;
        void mostrarSolucio() const;
        void guardarSolucioTiles(const string &filename) const;
        void convertirAMatriuSolucio(const vector<bool> &varsSolucio);
        bool comprovarSolucio() const;

        //Pre: No hi ha 2 variables diferents marcades en una sola casella
        //Post: retorna la id de la tile, throweja en cas d'incomplir la pre
        inline int getVariable(const vector<bool> &varsSolucio, int fila, int columna) const{
            const int primeraVar = fila*amplada*nTiles + columna*nTiles;
            int i=1; while(!varsSolucio[primeraVar+i]) i++;
            if(i > nTiles)
                throw "Internal error in getVariable(): incongruent cnf result";
            return i;
        }

        const string &inputTilesFile;
        int nTiles, nColors, amplada, alcada; //parametres de la instancia
        vector<Tile> inputTiles; //tiles d'entrada de la instancia. Rang 0..nTiles
        Clausules clausules; //clausules en CNF generades que representen les restriccions de la instancia
        vector<vector<int>> tilesSolucio; //matriu solucio. Cada enter és un index d'una tile. Rang dels index: 1..=nTiles

};

#endif // TILESAT_H
