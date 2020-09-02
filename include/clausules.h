#ifndef CLAUSULES_H
#define CLAUSULES_H
#include <vector>
#include <sstream>

using namespace std;

class Clausules
{
    public:
        Clausules();
        void addClause(const vector<int>&);
        void addAMO(const vector<int>&);
        void addEO(const vector<int>&);
        void addALO(const vector<int>&);
        void print();
        void guardarCNF(const string &, int, const string &, int, int, int, int, bool);
        void netejarClausules();

    private:
        vector<vector<int>> clausules;
};

#endif // CLAUSULES_H
