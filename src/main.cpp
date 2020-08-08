#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include "clausules.h"

using namespace std;
namespace fs = std::filesystem;

const unsigned NORD=0, EST=1, SUD=2, OEST=3;

vector<int> *condicionalsHor, *condicionalsVert;

void generarClausCondicionalHor(unsigned initVar, const vector<vector<int>> &inputTiles, Clausules &c){
    const int numVarsPerCasella = inputTiles.size();

    for(int tileId=0;tileId<numVarsPerCasella;tileId++){
        vector<int> clausula;

        clausula.push_back(-(initVar+tileId));
        const auto &ch = condicionalsHor[tileId];
        for(int i=0;i<ch.size();i++)
            clausula.push_back(initVar+numVarsPerCasella+ch[i]);

        c.addClause(clausula);
    }
}

void generarClausCondicionalVertical(unsigned initVar, const unsigned amplada, const vector<vector<int>> &inputTiles, Clausules &c){
    const int numVarsPerCasella = inputTiles.size();

    for(int tileId=0;tileId<numVarsPerCasella;tileId++){
        vector<int> clausula;
        clausula.push_back(-(initVar+tileId));
        const auto &ch = condicionalsVert[tileId];
        for(int i=0;i<ch.size();i++)
            clausula.push_back(initVar+numVarsPerCasella*amplada+ch[i]);

        c.addClause(clausula);
    }
}

void preComputeConditionals(const vector<vector<int>> &inputTiles){
    const int len = inputTiles.size();
    condicionalsHor = new vector<int>[len];
    condicionalsVert = new vector<int>[len];

    for(int i=0;i<len;i++){
        const auto &t1 = inputTiles[i];
        for(int j=0;j<len;j++){
            const auto &t2 = inputTiles[j];
            if(t1[EST]==t2[OEST])
                condicionalsHor[i].push_back(j);
            if(t1[SUD]==t2[NORD])
                condicionalsVert[i].push_back(j);
        }
    }
}

void generarTiles(Clausules &clausules, const int amplada, const int alcada, const vector<vector<int>> &inputTiles){

    const int numCaselles = amplada*alcada, numVarsPerCasella = inputTiles.size();

    preComputeConditionals(inputTiles);

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
            const int pos=(i*amplada+j)*numVarsPerCasella+1; //ULL
            if(j!=amplada-1){ //si no es ultima columna condicionem laterals
                generarClausCondicionalHor(pos, inputTiles, clausules); // -(pos+tileId) v tilesCompatbiles()
            }
            if(i!=alcada-1){ //si no es ultima fila condicionem inferiors
                generarClausCondicionalVertical(pos, amplada, inputTiles, clausules);
            }
        }
    }

    //clausules.addClause(vector<int>{i*numVarsXCasella + val});
}

vector<vector<int>> llegirTiles(string filename, int &numTiles, int &numColors, int &amplada, int &alcada){

    ifstream file;
    string buff;
    vector<vector<int>> tiles;
    vector<int> tile(4);

    file.open(filename);
    if(!file)
        throw "Could not open file " + filename;

    file >> numTiles >> numColors >> amplada >> alcada;

    for(int i=0;i<numTiles;i++){
        tile.clear();
        for(int i=0;i<4;i++){
            int subtile;
            file >> subtile;
            tile.push_back(subtile);
        }
        tiles.push_back(tile);
    }

    return tiles;
}

//Pre: No hi ha 2 variables diferents marcades en una sola casella
//Post: retorna la id de la tile, throweja en cas d'incomplir la pre
inline int getVariable(int amplada, int numTiles, const vector<bool> &varsSolucio, int fila, int columna){

    const int primeraVar = fila*amplada*numTiles + columna*numTiles;// +1;
    int i=1;
    while(!varsSolucio[primeraVar+i])i++;

    if(i > numTiles)
        throw "Internal error in getVariable(): incongruent cnf result"; //podria ser un assert

    return i;
}

//Post: retorna el vector amb les variables de la solucio amb la primera posicio stubejada
//(les variables reals van de 1 a n)
vector<bool> llegirResultatMinisat(const string &filename){
    //obrir arxiu filename i llegir el resultat de minisat
    ifstream file;
    file.open(filename);
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

void mostrarSolucio(int alcada, int amplada, const vector<bool> &varsSolucio, int numTiles){

    if(!varsSolucio.empty()){
        for(int i=0;i<alcada;i++){
            for(int j=0;j<amplada;j++){
                const int valor = getVariable(amplada, numTiles, varsSolucio, i, j);
                cout<<valor<<' ';
            }
            cout<<endl;
        }
    }
    cout<<endl;
}

void guardarSolucioTiles(string filename, int alcada, int amplada, const vector<bool> &varsSolucio, int numTiles){

    ofstream file;
    file.open(filename);

    for(int i=0;i<alcada;i++){
        for(int j=0;j<amplada;j++){
            const int valor = getVariable(amplada, numTiles, varsSolucio, i, j);
            file<<valor<<' ';
        }
        file<<endl;
    }
    file.close();
}

void convertirAMatriuSolucio(const vector<bool> &varsSolucio, vector<vector<int>> &tilesSolucio, int alcada, int amplada, int numTiles){

    for(int i=0;i<amplada;i++){ //com que tilesSolucio és "column major" iterant primer per les columnes és més caché friendly
        for(int j=0;j<alcada;j++){
            const int valor = getVariable(amplada, numTiles, varsSolucio, j, i);
            tilesSolucio[i][j] = valor;
        }
    }
}

//Post: retorna cert si la solucio és correcte, fals en c.c.
bool comprovarSolucio(const vector<vector<int>> &tiles, const vector<vector<int>> &tilesSolucio, int alcada, int amplada, int numTiles){

    for(int i=0;i<amplada-1;i++){
        for(int j=0;j<alcada-1;j++){
            const auto &tileActual = tiles[tilesSolucio[i][j]-1];
            //cout<<"i:"<<i<<"j:"<<j<<"tilesSol[i+1][j]:"<<tilesSolucio[i+1][j]<<endl;
            const auto &tileDreta = tiles[tilesSolucio[i+1][j]-1];
            const auto &tileInferior = tiles[tilesSolucio[i][j+1]-1];

            if(tileActual[EST] != tileDreta[OEST] || tileActual[SUD] != tileInferior[NORD])
                return false;
        }
    }

    return true;
}

//https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
//because c++ string library doesn't provide it...
std::vector<std::string> split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

string cleanFilename(string filename){

    const auto filenameSplit1 = split(filename, '/'); //!fixme: hardcoded linux path sep.
    const auto filenameSplit2 = split(filenameSplit1.back(), '.');
    return filenameSplit2[0];
}

void doTiles(const string &inputTilesFile, bool printSolution, bool checkSolution, bool drawTiles, bool solve=true, int ampladaOverride=-1, int alcadaOverride=-1){

    Clausules clausules;
    vector<vector<int>> tiles; //ULL! les tiles van de 0..nTiles, pero tilesSolucio va de 1..=nTiles
    vector<bool> varsSolucio;
    int nTiles, nColors, amplada, alcada;
    const string cleanInputFilename = cleanFilename(inputTilesFile);

    tiles = llegirTiles(inputTilesFile, nTiles, nColors, amplada, alcada);
    if(ampladaOverride != -1)
        amplada = ampladaOverride;
    if(alcadaOverride != -1)
        alcada = alcadaOverride;

    generarTiles(clausules, amplada, alcada, tiles);
    const string tmp_folder = "tmp";

    if(!fs::is_directory(tmp_folder)){
        if(!fs::exists(tmp_folder)){
            cout<<"Creating "<<tmp_folder<<" folder"<<endl;
            fs::create_directory(tmp_folder);
        }
        else
            throw tmp_folder + " is not a folder";
    }

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

        //{
        varsSolucio = llegirResultatMinisat(cnfFolder + tempCnfOutput);
        vector<vector<int>> tilesSolucio(amplada); //els index extrets d'aquesta estructura comencen per 1! 1..=nTiles
        tilesSolucio.resize(amplada);
        for(auto &fila : tilesSolucio)
            fila.resize(alcada);
        convertirAMatriuSolucio(varsSolucio, tilesSolucio, alcada, amplada, nTiles);
        //}

        if(!comprovarSolucio(tiles, tilesSolucio, alcada, amplada, nTiles))
            throw "Internal error: obtained solution is invalid"; //podria ser un assert

        if(printSolution)
            mostrarSolucio(alcada, amplada, varsSolucio, nTiles);

        guardarSolucioTiles(tempSolFilePath, alcada, amplada, varsSolucio, nTiles);

        if(drawTiles){
            string outputSvg = "output/" + cleanInputFilename + "_" + to_string(amplada) + "_" + to_string(alcada) + ".svg";
            comandaAll = "python3 drawTiles.py " + inputTilesFile + " " + tempSolFilePath + " color " + outputSvg;

            int drawRetVal = system(comandaAll.c_str());
            if(drawRetVal != 0)
                throw "Error generating SVG. drawTiles.py returned " + to_string(drawRetVal);
            else
                cout<<"SVG saved at "<<outputSvg<<endl;
        }
    }
}

int main(int argc, char** argv)
{
    string inputTilesFile;

    bool parametresOk = true;
    bool printSolution = false;
    bool checkSolution = false;
    bool drawTiles = false;
    bool onlyTransform = false;
    bool solve = true;

    if(argc == 1 || argc == 2 && (string(argv[1]) == "-h" || string(argv[1]) == "--help")){
        cout<<"Usage: " << argv[0] << " [Options]...  <TilesProblemFile>" <<endl;
        cout<<"Options:"<<endl;
        cout<<"     -p      printSolution in ascii"<<endl;
        //cout<<"     -c      checkSolution (requires python and checkSolution.py)"<<endl;
        cout<<"     -d      drawTiles (requires python, drawTiles.py and it's deps.)"<<endl;
        cout<<"     -g      don't solve, only generate cnf. Invalidates other options"<<endl;
        //cout<<"     -t      (special) transform minisat output to tiles solution. format: "<<argv[0]<<" -t <TilesProblemFile> <MiniSAT output>"<<endl;
        parametresOk = false;
    }
    else if(argc > 1){
        inputTilesFile = argv[argc-1];
        int i=1;
        while(i<argc-1){
            string act = argv[i];
            if(act=="-d")
                drawTiles = true;
            else if(act=="-c")
                checkSolution = true;
            else if(act=="-p")
                printSolution = true;
            else if(act=="-g")
                solve = false;
            else if(act=="-t"){
                onlyTransform = true;
                parametresOk = false;
                break;
            }
            else{
                cerr<<"Unknown parameter"<<endl;
                parametresOk = false;
                break;
            }
            i++;
        }
    }

    if(onlyTransform){ //todo: fix this
        inputTilesFile = argv[2];
        string minisatFilename = argv[3];

        int nTiles, nColors, amplada, alcada;
        vector<vector<int>> tiles = llegirTiles(inputTilesFile, nTiles, nColors, amplada, alcada);
        const string cleanInputFilename = cleanFilename(inputTilesFile);
        vector<bool> varsSolucio = llegirResultatMinisat(minisatFilename);
        if(varsSolucio.size() > 0){
            string outFilename = cleanInputFilename + "_" + to_string(amplada) + "_" + to_string(alcada) + ".tiles";
            guardarSolucioTiles(outFilename, alcada, amplada, varsSolucio, nTiles);
        }
    }

    if(parametresOk)
        try{
            doTiles(inputTilesFile, printSolution, checkSolution, drawTiles, solve);
        }catch(const char* c){
            cerr << c << endl;
            return 0x10;
        } catch(string s){
            cerr << s << endl;
            return 0x11;
        } catch(std::exception& e){
            cerr << e.what() << endl;
            return 0x12;
        } catch(...){
            cerr << "Unknown exception (...)" << endl;
            return 0x13;
        }
    else
        return 0x14;

    /*for(int i=2;i<=100;i++)
        doTiles(inputTilesFile, false, false, false, false, i, i);*/

    return 0;
}
