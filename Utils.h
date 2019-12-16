#ifndef __UTILS_H
#define __UTILS_H
#include "Container.h"
class Utils
{
	static bool isOp(char a);
	static void findAndReplaceAll(string & data, string toSearch, string replaceStr, Container& momory);
	static unsigned long int getVariableValue(string& key, Container& memory);
	static void divideAndReplace(string body, list<string>& funContainer, list<string>& argContainer);

public:
	static unsigned long int CalculateBodyofFunction(string body, Container& memory);
	static unsigned long int CalculateFunction(string key, string argument, Container& memory);
};
#endif