#pragma once
#include <string>
#include <vector>

using namespace std;

typedef struct {
    vector<vector<string>> patternResults;
} PatternResults;

typedef struct {
    vector<vector<string>> suchThatResults;
} SuchThatResults;

typedef struct {
    vector<string> selectResults;
} SelectResults;

class ResultStorage {
public:
    ResultStorage();

    void addPatternResults(const vector<vector<string>>& patternResults);
    void addSuchThatResults(const vector<vector<string>>& suchThatResults);
    void addSelectResults(const vector<string>& selectResults);

    void printSelectResults();
    void printSuchThatResults();
    void printPatternResults();
    vector<PatternResults> patternResults;
    vector<SuchThatResults> suchThatResults;
    vector<SelectResults> selectResults;
};
