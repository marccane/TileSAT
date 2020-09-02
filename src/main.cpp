#include <iostream>
#include <vector>
#include "tileSAT.h"

int main(int argc, char** argv)
{
    string inputTilesFile;

    bool parametresOk = true;
    bool printSolution = false;
    bool checkSolution = true;
    bool drawTiles = false;
    bool onlyTransform = false;
    bool solve = true;

    if(argc == 1 || (argc == 2 && (string(argv[1]) == "-h" || string(argv[1]) == "--help"))){
        cout<<"Usage: " << argv[0] << " [Options]...  <TilesProblemFile>" <<endl;
        cout<<"Options:"<<endl;
        cout<<"     -p      print solution in console"<<endl;
        //cout<<"     -c      checkSolution (requires python and checkSolution.py)"<<endl;
        cout<<"     -d      drawTiles (requires python3 in PATH, drawTiles.py and it's deps.)"<<endl;
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

    /*if(onlyTransform){ //todo: fix this
        inputTilesFile = argv[2];
        string minisatFilename = argv[3];

        int nTiles, nColors, amplada, alcada;
        vector<vector<int>> tilesSolucio;
        vector<Tile> tiles = llegirTiles(inputTilesFile, nTiles, nColors, amplada, alcada);
        const string cleanInputFilename = cleanFilename(inputTilesFile);
        convertirAMatriuSolucio(llegirResultatMinisat(minisatFilename), tilesSolucio, alcada, amplada, nTiles);
        string outFilename = cleanInputFilename + "_" + to_string(amplada) + "_" + to_string(alcada) + ".tiles";
        guardarSolucioTiles(outFilename, tilesSolucio, alcada, amplada);
    }*/

    if(parametresOk)
        try{
            TileSAT tileSAT;
            tileSAT.doTiles(inputTilesFile, printSolution, checkSolution, drawTiles, solve);
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
