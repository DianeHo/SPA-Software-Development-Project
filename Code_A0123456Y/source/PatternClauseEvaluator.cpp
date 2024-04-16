#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "Database.h"
#include "PatternClauseEvaluator.h"
#include "Query.h"


using namespace std;

void PatternClauseEvaluator::evaluate(vector<vector<string>>& results, vector<string> LHSPair, vector<string> RHSPair, map<string, string> declarationList)
{
	Database::getPatternStmt(LHSPair, RHSPair, results);	
}
