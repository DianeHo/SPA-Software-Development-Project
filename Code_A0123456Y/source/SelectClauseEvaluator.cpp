#include <iostream>
#include <algorithm>
#include "Database.h"
#include "SelectClauseEvaluator.h"

using namespace std;

void SelectClauseEvaluator::evaluate(vector<string>& results, string selectIdxType)
{
	vector<string> tempDatabaseResult;
	if (selectIdxType == "procedure") {
		Database::getProcedures(tempDatabaseResult);
	}
	else if (selectIdxType == "variable") {
		Database::getVariables(tempDatabaseResult);
	}
	else if (selectIdxType == "constant") {
		Database::getConstants(tempDatabaseResult);
	}
	else if (selectIdxType == "call") {
		Database::getCallsProcedures(tempDatabaseResult);
	}
	else { //assign,read,print,stmt,if,while
		Database::getStatements(selectIdxType, tempDatabaseResult);
	}

	
	//remove duplicates from vector before pushing it into databaseResult string
	removeDupe(tempDatabaseResult);

	// post process the results to fill in the output vector
	for (string databaseResult : tempDatabaseResult) {
		results.push_back(databaseResult);
	}
}

void SelectClauseEvaluator::removeDupe(vector<string>& results)
{
	sort(results.begin(), results.end());
	results.erase(unique(results.begin(), results.end()), results.end());
}
