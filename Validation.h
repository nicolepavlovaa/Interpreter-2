#ifndef __VALIDATION_H
#define __VALIDATION_H
#include "RPN.h"
#include "Utils.h"

class Validation
{
	static bool validateNum(string num);
	static bool validateVar(string var);
	static bool validateFun(string fun);
	static bool validateTerm(string term);
	static bool validateFactor(string factor);
	static bool validateExpr(string expr);
public:
	static bool validateLine(string line, Container& memory);
};

#endif