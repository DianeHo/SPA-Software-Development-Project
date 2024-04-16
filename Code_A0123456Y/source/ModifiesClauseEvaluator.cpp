#include <iostream>
#include <string>
#include <algorithm>
#include <map>
#include "Database.h"
#include "ModifiesClauseEvaluator.h"
#include "Query.h"


using namespace std;

void ModifiesClauseEvaluator::evaluate(vector<vector<string>>& results, string firstArgumentType, string firstArgumentValue, string secondArgumentType, string secondArgumentValue)
{
	Database::getModifies(results, firstArgumentType, firstArgumentValue, secondArgumentType, secondArgumentValue);
}
