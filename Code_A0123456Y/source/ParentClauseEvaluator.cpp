#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "Database.h"
#include "ParentClauseEvaluator.h"
#include "Query.h"


using namespace std;

void ParentClauseEvaluator::evaluateParent(vector<vector<string>>& results, string parentType, string parentValue, string childType, string childValue)
{
	Database::getParent(0, parentValue, parentType, childValue, childType, results);
}

void ParentClauseEvaluator::evaluateParentT(vector<vector<string>>& results, string parentType, string parentValue, string childType, string childValue)
{
	Database::getParent(1, parentValue, parentType, childValue, childType, results);
}