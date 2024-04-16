#pragma once
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <unordered_map>
#include <algorithm>
#include <iostream>

using namespace std;

typedef struct {
    string name;
    string designEntity;
} SelectClause;

typedef struct { //{ { Modifies, { read, r }, {variable, v1} }
    string relRef; //e.g. uses, modifies, parent
    vector<string> firstArgument; //index 0 is design entity / undeclared / line number or index 1 is synonym, i.e. uses(a,v) = firstArgument[0] = a, firstArgument[1] = v
    vector<string> secondArgument;
} SuchThatClause;

typedef struct { //{ { a1 { variable , v }, { both partial, x+1} }
    string patternSynonym; 
    vector<string> LHS; //index 0 is design entity/undeclared/IDENT, index 1 is entity Reference (synonym, _ or "IDENT")
    vector<string> RHS; //index 0 is partial match/exact match t/undeclared, index 1 is the expression
} PatternClause;

class Query
{
public:
    Query();

    void addSelectClause(string name, string designEntity);
    void addSuchThatClause(string relRef, vector<string> firstArgument, vector<string> secondArgument);
    void addPatternClause(string synonymName, vector<string> LHS, vector<string> RHS);
    void addDeclarationList(map<string, string> declarationList);

    vector<SelectClause> selectClauses;
    vector<SuchThatClause> suchThatClauses;
    vector<PatternClause> patternClauses;
    map<string, string> declarationList;
};

