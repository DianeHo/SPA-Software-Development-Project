#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "Database.h"
#include "Query.h"
#include "CallsClauseEvaluator.h"


using namespace std;


void CallsClauseEvaluator::evaluate(bool callsAst, vector<vector<string>>& results, string parentType, string parentValue, string childType, string childValue)
{
	if (callsAst) {
		Database::getCalls(1, results, parentType, parentValue, childType, childValue);
	}
	else {
		Database::getCalls(0, results, parentType, parentValue, childType, childValue);
	}
}
