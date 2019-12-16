#include "Validation.h"

bool Validation::validateVar(string var)
{
	regex var_reg("^[a-z]+");
	return regex_match(var, var_reg);
}
bool Validation::validateNum(string num)
{
	regex num_reg("([1-9]+[0-9]*|0)");
	return regex_match(num, num_reg);
}
bool Validation::validateFactor(string factor)
{
	regex factor_reg1("^\\(.+\\)"); //factor->(Expr)
	regex factor_reg2("^(.+\\[.+\\])"); //factor->Fun[Expr]
	//3
	if (regex_match(factor, factor_reg1))
	{
		string token = factor.substr(1, factor.rfind(')') - 1);
		if (validateExpr(token)) return true;
	}
	//4
	else if (regex_match(factor, factor_reg2))
	{
		string token1 = factor.substr(0, factor.find('['));
		unsigned first = factor.find('[');
		unsigned second = factor.rfind(']');
		string token2 = factor.substr(first + 1, second - first - 1);
		if (validateFun(token1) && validateExpr(token2)) return true;
	}
	//1,2
	else if (validateVar(factor)) //factor->var
	{
		return true; 
	}
	else if (validateNum(factor)) //factor-> num
	{
		return true;
	}
	return false;
}
bool Validation::validateTerm(string term)
{
	regex term1_reg("^(.+)");//term->factor
	regex term2_reg("^((.+) \\* (.+))"); //term->term * factor
	regex term3_reg("^((.+) \/ (.+))"); //term->term / factor
	regex term4_reg("^((.+) % (.+))"); //term->term % factor

	//utils

	int multPos = -1;
	int divPos = -1;
	int percPos = -1;
	int counter = 0;
	int counter2 = 0;

	for (int i = 0; i < term.length(); i++)
	{
		if (term[i] == '[') counter++;
		if (term[i] == ']') counter--;
		if (term[i] == '(') counter2++;
		if (term[i] == ')') counter2--;
		if (term[i] == '*' && counter == 0 && counter2 == 0) multPos = i;
		if (term[i] == '/' && counter == 0 && counter2 == 0) divPos = i;
		if (term[i] == '%' && counter == 0 && counter2 == 0) percPos = i;
	}
	//2
	if (regex_match(term, term2_reg) && multPos>divPos && multPos>percPos)
	{
		string token1 = term.substr(0, multPos - 1);
		string token2 = term.substr(multPos + 2);

		if (validateTerm(token1) && validateFactor(token2)) return true;
	}
	//3
	else if (regex_match(term, term3_reg) && divPos>multPos && divPos>percPos)
	{
		string token1 = term.substr(0, divPos - 1);
		string token2 = term.substr(divPos + 2);

		if (validateTerm(token1) && validateFactor(token2)) return true;
	}
	//4
	else if (regex_match(term, term4_reg) && percPos>multPos && percPos>divPos)
	{
		string token1 = term.substr(0, percPos - 1);
		string token2 = term.substr(percPos + 2);

		if (validateTerm(token1) && validateFactor(token2)) return true;
	}
	//1
	else if (regex_match(term, term1_reg))
	{
		if (validateFactor(term)) return true;
	}
	return false;
}
bool Validation::validateExpr(string expr)
{
	regex expr1_reg("^(.+)"); //expr->term
	regex expr2_reg("^(.+ - .+)"); //expr-> expr - term
	regex expr3_reg("^(.+ \\+ .+)"); //expr-> expr + term   

	int minPos = -1;
	int plusPos = -1;
	int counter = 0;
	int counter2 = 0;
	
	for (int i = 0; i < expr.length(); i++)
	{
		if (expr[i] == '[') counter++;
		if (expr[i] == ']') counter--;
		if (expr[i] == '(') counter2++;
		if (expr[i] == ')') counter2--;
		if (expr[i] == '-' && counter == 0 && counter2 == 0) minPos = i;
		if (expr[i] == '+' && counter == 0 && counter2 == 0) plusPos = i;
	}
	//2
	if (regex_match(expr, expr2_reg) && minPos>plusPos)
	{
		string token1 = expr.substr(0, minPos - 1);
		string token2 = expr.substr(minPos + 2);
		
		if (validateExpr(token1) && validateTerm(token2)) return true;
	}
	//3
	else if (regex_match(expr, expr3_reg) && plusPos>minPos)
	{
		string token1 = expr.substr(0, plusPos -1);
		string token2 = expr.substr(plusPos + 2);
		
		if (validateExpr(token1) && validateTerm(token2)) return true;
	}
	//1
	else if (regex_match(expr, expr1_reg))
	{
		if (validateTerm(expr)) return true;
	}
	return false;
}
bool Validation::validateFun(string fun)
{
	regex fun_reg("^([A-Z]+)");
	return regex_match(fun, fun_reg);
}
bool Validation::validateLine(string line, Container& memory)
{
	regex line1_reg("^(.+ = .+)"); //line-> var = expr
	regex line2_reg("^(.+\\[.+\\] = .+)"); //line-> fun[var] = expr
	regex line3_reg("^(read .+)"); //line-> read var
	regex line4_reg("^(print .+)"); //line-> print expr
	//1
	if (regex_match(line, line1_reg))
	{
		string delimiter = " = ";
		string token1 = line.substr(0, line.find(delimiter));
		string token2 = line.substr(line.find(delimiter) + 3);
		if (validateVar(token1) && validateExpr(token2))
		{
			unsigned long int n = 0;
			try
			{
				n = Utils::CalculateBodyofFunction(token2, memory);
			}
			catch (const std::invalid_argument &err)
			{
				throw std::invalid_argument(err.what());
			}
			memory.variables.insert_or_assign(token1, n);
			return true;
		}
	}
	//2
	if (regex_match(line, line2_reg))
	{
		string delimiter = " = ";
		string token1 = line.substr(0, line.find(delimiter));
		string token2 = line.substr(line.find(delimiter) + 3);
		unsigned first = line.find('[');
		unsigned second = line.find(']');
		string var = line.substr(first + 1, second - first - 1);
		string fun = line.substr(0, line.find('['));
		if (validateFun(fun) && validateVar(var) && validateExpr(token2))
		{
			memory.functions.insert_or_assign(fun, make_pair(var, token2));
			return true;
		}
	}
	//3
	if (regex_match(line, line3_reg))
	{
		string token = line.substr(line.rfind(' ') + 1);
		if (validateVar(token))
		{
			unsigned long int value;
			cin >> value;
			memory.variables.insert_or_assign(token, value);
			return true;
		}
		//throw exception
		else throw std::invalid_argument("Invalid argument ");
	}
	//4
	if (regex_match(line, line4_reg))
	{
		string token = line.substr(line.find(' ') + 1);
		if (validateExpr(token))
		{
			unsigned long int n = 0;
			//exception
			try
			{
				n = Utils::CalculateBodyofFunction(token, memory);
			}
			catch (const std::invalid_argument &err)
			{
				throw std::invalid_argument(err.what());
			}
			//
			cout << n << endl;
			return true;
		}
	}
	return false;
}