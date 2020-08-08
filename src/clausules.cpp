#include <iostream>
#include <fstream>
#include <gzip/compress.hpp>
#include <sstream>
#include "clausules.h"

using namespace std;

Clausules::Clausules()
{
    //ctor
}

void Clausules::addClause(const vector<int>& clause){
    clausules.push_back(clause);
}

void Clausules::addAMO(const vector<int>& vars){
    /*cout<<"AMO: ";
    for(int v : vars)
        cout<<v<<" ";
    cout<<endl;*/

    vector<int> temp;
    int len=vars.size();
    for(int i=0;i<len-1;i++){
        for(int j=i+1;j<len;j++){
            temp.push_back(-vars[i]);
            temp.push_back(-vars[j]);
            clausules.push_back(temp);
            temp.clear();
        }
    }
}

void Clausules::addEO(const vector<int>& vars){
    addALO(vars);
    addAMO(vars);
}

/*int* crash=nullptr;
    *crash=123;*/

void Clausules::addALO(const vector<int>& vars){
    /*cout<<"ALO: ";
    for(int v : vars)
        cout<<v<<" ";
    cout<<endl;*/
    addClause(vars);
}

void Clausules::print(){
    cout<<"------";
    for(vector<int> clausula: clausules){
        for(int i: clausula){
            cout<<i<<" ";
        }
        cout<<endl;
    }
}

void Clausules::guardarCNF(const string &filename, int numvars, const string &cleanFilename, int nTiles, int nColors, int amplada, int alcada, bool Gzip = false){

    ofstream file;
    stringstream ss;
    const int BUFF_SIZE = 200;
    int numclausules = clausules.size();

    file.open(filename);
    if(!file)
        throw "Error saving CNF file";

    ss << "c TileSat 0.2" << endl;
    //ss << "c filename nTiles nColors width height" << endl;
    char buff[BUFF_SIZE];
    snprintf(buff, BUFF_SIZE, "c %s %d %d %d %d", cleanFilename.c_str(), nTiles, nColors, amplada, alcada);
    ss << buff << endl;
    snprintf(buff, BUFF_SIZE, "p cnf %d %d", numvars, numclausules);
    ss << buff << endl;

    if(Gzip){
        for(auto clausula : clausules){
            for(int literal : clausula){
                ss << literal << " ";
            }
            ss << 0 << endl;
        }

        string data = ss.str();
        string compressed = gzip::compress(data.data(), data.size());
        file << compressed;
    }
    else{
        file << ss.str();
        for(auto clausula : clausules){
            for(int literal : clausula){
                file << literal << " ";
            }
            file << 0 << endl;
        }
    }
    file.close();

    /*namespace bio = boost::iostreams;

    std::stringstream compressed;
    std::stringstream origin(filename);

    bio::filtering_streambuf<bio::input> out;
    out.push(bio::gzip_compressor(bio::gzip_params(bio::gzip::best_compression)));
    out.push(origin);
    bio::copy(out, compressed);

    ofstream file;
    file.open(filename);

    file << compressed.str() << endl;

    file.close();*/
}

void Clausules::netejarClausules(){
    clausules.clear();
}
