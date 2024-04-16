#pragma once
#include <string>
#include <vector>
#include "Database.h"
#include "Query.h"
#include "QueryProcessor.h"

using namespace std;


class CallsClauseEvaluator
{
public:
    void evaluate(bool callsAst, vector<vector<string>>& results, string parentType, string parentValue, string childType, string childValue);
};

