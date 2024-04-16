#include "ResultStorage.h"
#include <iostream>

ResultStorage::ResultStorage() {
    patternResults = vector<PatternResults>{};
    suchThatResults = vector<SuchThatResults>{};
    selectResults = vector<SelectResults>{};
}

void ResultStorage::addPatternResults(const vector<vector<string>>& patternResults) {
    this->patternResults.push_back({ patternResults });
}

void ResultStorage::addSuchThatResults(const vector<vector<string>>& suchThatResults) {
    this->suchThatResults.push_back({ suchThatResults });
}
void ResultStorage::addSelectResults(const vector<string>& selectResults) {
    this->selectResults.push_back({ selectResults });
}

void ResultStorage::printSelectResults()
{   
    vector<string>singleResultVector;
    int index = 0; //for the single select case;
    if (index >= 0 && index < selectResults.size()) {
        singleResultVector = this->selectResults[index].selectResults;
    }
    else {
        // Handle out-of-range index
        return; // or throw an exception
    }

    for (string p : singleResultVector) {
        cout << p << endl;
    }
}

void ResultStorage::printSuchThatResults()
{
    vector<vector<string>>singleResultVector;
    int index = 0; //for the single select case;
    if (index >= 0 && index < suchThatResults.size()) {
        singleResultVector = this->suchThatResults[0].suchThatResults;
    }
    else {
        // Handle out-of-range index
        return; // or throw an exception
    }

    for (vector<string> p : singleResultVector) {
        cout << p[0] << " " << p[1] << endl;
    }
}

void ResultStorage::printPatternResults() {
    if (patternResults.empty()) {
        // Handle the case where patternResults is empty
        cout << "No pattern results to print." << endl;
        return;
    }

    vector<vector<string>> singleResultVector;
    int index = 0; // for the single select case;

    if (index >= 0 && index < patternResults.size()) {
        singleResultVector = patternResults[index].patternResults;
    }
    else {
        // Handle out-of-range index
        cout << "Index is out of range." << endl;
        return; // or throw an exception
    }

    for (const auto& p : singleResultVector) {
        if (p.size() >= 2) {
            cout << p[0] << " " << p[1] << endl;
        }
        else {
            // Handle the case where p does not contain enough elements
            cout << "Invalid pattern result format." << endl;
        }
    }
}

