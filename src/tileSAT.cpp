#include "tileSAT.h"
#include <iostream>
#include <fstream>

using std::cout, std::endl;

const Tile *stubTile;
constexpr int NUM_TILE_ELEMENTS = sizeof(*stubTile)/sizeof(stubTile->nord);

TileSAT::TileSAT()
{
    //ctor
}

void TileSAT::generarClausCondicionalHor(unsigned initVar){

    const int numVarsPerCasella = inputTiles.size();
    for(int tileId=0;tileId<numVarsPerCasella;tileId++){
        vector<int> clausula;
        const auto &ourTile = inputTiles[tileId];

        clausula.push_back(-(initVar+tileId));
        for(int i=0;i<numVarsPerCasella;i++){
            const auto &tmpTile = inputTiles[i];
            if(ourTile.est==tmpTile.oest)
                clausula.push_back(initVar+numVarsPerCasella+i);
        }
        clausules.addClause(clausula);
    }
}

void TileSAT::generarClausCondicionalVertical(unsigned initVar){

    const int numVarsPerCasella = inputTiles.size();
    for(int tileId=0;tileId<numVarsPerCasella;tileId++){
        vector<int> clausula;
        const auto &ourTile = inputTiles[tileId];

        clausula.push_back(-(initVar+tileId));
        for(int i=0;i<numVarsPerCasella;i++){
            const auto &tmpTile = inputTiles[i];
            if(ourTile.sud==tmpTile.nord)
                clausula.push_back(initVar+numVarsPerCasella*amplada+i);
        }
        clausules.addClause(clausula);
    }
}

void TileSAT::generarTiles(){

    const int numCaselles = amplada*alcada, numVarsPerCasella = inputTiles.size();

    //Exactly one per cada casella
    int varAct = 1;
    for(int i=1; i<=numCaselles; i++){
            vector<int> temp;
            for(int k=0; k<numVarsPerCasella; k++){
                temp.push_back(varAct+k);
            }
            clausules.addEO(temp);
            varAct += numVarsPerCasella;
    }

    //Casella -> possibles successores
    for(int i=0;i<alcada;i++){ //per cada fila
        for(int j=0;j<amplada;j++){ //per cada columna
            const int pos = (i*amplada+j)*numVarsPerCasella+1; //ULL
            if(j!=amplada-1) //si no es ultima columna condicionem laterals
                generarClausCondicionalHor(pos);
            if(i!=alcada-1) //si no es ultima fila condicionem inferiors
                generarClausCondicionalVertical(pos);
        }
    }
}

vector<Tile> TileSAT::llegirTiles(const string &filename){

    ifstream file(filename);
    string buff;
    vector<Tile> tiles;

    if(!file)
        throw "Could not open file " + filename;

    file >> nTiles >> nColors >> amplada >> alcada;

    for(int _=0;_<nTiles;_++){
        decltype(stubTile->nord) tile[NUM_TILE_ELEMENTS];
        for(int i=0;i<NUM_TILE_ELEMENTS;i++)
            file >> tile[i];
        tiles.push_back(*reinterpret_cast<Tile*>(tile));
    }

    return tiles; //spray and pray for NRVO
}

//Post: retorna el vector amb les variables de la solucio amb la primera posicio stubejada
//(les variables reals van de 1 a n)
vector<bool> TileSAT::llegirResultatMinisat(const string &filename) const{
    //obrir arxiu filename i llegir el resultat de minisat
    ifstream file(filename);
    string buff;
    bool sat = false;
    vector<bool> varsSolucio;

    file >> buff;
    if(buff == "SAT")
         sat = true;
    else if(buff == "UNSAT")
        throw "Error: UNSAT, no solution found";
    else if(buff == "INDET")
        throw "Error: Minisat computation interrupted/out of memory";
    else
        throw "Error: Invalid minisat file format";

    if(sat){
        //posicio 0 stub
        varsSolucio.push_back(false);
        do{
            file >> buff;
            varsSolucio.push_back(buff[0]!='-');
        }while(!file.eof());
    }

    return varsSolucio;
}

void TileSAT::mostrarSolucio() const{

    for(int i=0;i<alcada;i++){
        for(int j=0;j<amplada;j++)
            cout<<tilesSolucio[j][i]<<' ';
        cout<<endl;
    }
    cout<<endl;
}

void TileSAT::guardarSolucioTiles(const string &filename) const{

    ofstream file(filename);

    for(int i=0;i<alcada;i++){
        for(int j=0;j<amplada;j++){
            file<<tilesSolucio[j][i]<<' ';
        }
        file<<endl;
    }
    file.close();
}

void TileSAT::convertirAMatriuSolucio(const vector<bool> &varsSolucio){

    for(int i=0;i<amplada;i++){ //com que tilesSolucio és "column major" iterant primer per les columnes és més caché friendly
        for(int j=0;j<alcada;j++){
            const int valor = getVariable(varsSolucio, j, i);
            tilesSolucio[i][j] = valor;
        }
    }
}

//Post: retorna cert si la solucio és correcte, fals en c.c.
bool TileSAT::comprovarSolucio() const{

    for(int i=0;i<amplada-1;i++){
        for(int j=0;j<alcada-1;j++){
            const auto &tileActual = inputTiles[tilesSolucio[i][j]-1];
            const auto &tileDreta = inputTiles[tilesSolucio[i+1][j]-1];
            const auto &tileInferior = inputTiles[tilesSolucio[i][j+1]-1];

            if(tileActual.est != tileDreta.oest || tileActual.sud != tileInferior.nord)
                return false;
        }
    }
    return true;
}

void TileSAT::doTiles(const string &inputTilesFile, bool printSolution, bool checkSolution, bool drawTiles, bool solve, int ampladaOverride, int alcadaOverride){

    //Clausules clausules;
    //vector<Tile> tiles; //ULL! les tiles van de 0..nTiles, pero tilesSolucio va de 1..=nTiles
    //int nTiles, nColors, amplada, alcada;
    const string cleanInputFilename = cleanFilename(inputTilesFile);

    inputTiles = llegirTiles(inputTilesFile);
    if(ampladaOverride != -1)
        amplada = ampladaOverride;
    if(alcadaOverride != -1)
        alcada = alcadaOverride;

    generarTiles();
    const string tmp_folder = "tmp";
    createFolderIfDoesntExist(tmp_folder);

    const string cnfFolder = tmp_folder + "/", tempCnfFile = "out.cnf.gz", tempSolFilePath = cnfFolder + "sol.tiles";
    clausules.guardarCNF(cnfFolder + tempCnfFile, amplada*alcada*nTiles, cleanInputFilename, nTiles, nColors, amplada, alcada, false);

    if(solve){
        //executem minisat
        bool noMinisatOutput = false;
        const string comanda = "minisat -var-decay=0.99 ", tempCnfOutput = "out.res", ignoreMinisatOutput = " >/dev/null";
        string comandaAll = comanda + " " + cnfFolder + tempCnfFile + " " + cnfFolder + tempCnfOutput;
        if(noMinisatOutput)
            comandaAll += ignoreMinisatOutput;
        int resultatMinisat = system(comandaAll.c_str());
        if(resultatMinisat != 2560)
            throw "Error: minisat process has returned " + to_string(resultatMinisat);
        cout<<endl;

        //vector<vector<int>> tilesSolucio(amplada); //els index extrets d'aquesta estructura comencen per 1. El rang és 1..=nTiles
        tilesSolucio.resize(amplada);
        for(auto &fila : tilesSolucio)
            fila.resize(alcada);

        convertirAMatriuSolucio(llegirResultatMinisat(cnfFolder + tempCnfOutput));

        if(checkSolution && !comprovarSolucio())
            throw "Internal error: obtained solution is invalid";

        if(printSolution)
            mostrarSolucio();

        guardarSolucioTiles(tempSolFilePath);

        if(drawTiles){
            const string outputFolder = "output";
            createFolderIfDoesntExist(outputFolder);
            string outputSvg = outputFolder + "/" + cleanInputFilename + "_" + to_string(amplada) + "_" + to_string(alcada) + ".svg";
            comandaAll = "python3 drawTiles.py " + inputTilesFile + " " + tempSolFilePath + " color " + outputSvg;

            int drawRetVal = system(comandaAll.c_str());
            if(drawRetVal != 0)
                throw "Error generating SVG. drawTiles.py returned " + to_string(drawRetVal);
            else
                cout<<"SVG saved at "<<outputSvg<<endl;
        }
    }
}
