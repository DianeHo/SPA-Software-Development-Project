#include "cjecl.h"
#include "Tokenizer.h"
#include <iostream>
#include <algorithm>
#include <list>
#include <unordered_map>
#include <regex>


static bool checkEntities(const string& token) {
	return regex_search(token, regex(ENTITIES_PATTERN));
}
