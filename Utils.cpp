#include "Utils.h"

void Utils::findAndReplaceAll(string & data, string toSearch, string replaceStr, Container & momory)
{
	size_t pos = data.find(toSearch);

	while (pos != std::string::npos)
	{
		data.replace(pos, toSearch.size(), replaceStr);
		pos = data.find(toSearch, pos + replaceStr.size());
	}
}

unsigned long int Utils::getVariableValue(string & key, Container & memory)
{
	unsigned long int result;
	map<string, unsigned long int>::iterator it;
	it = memory.variables.find(key);
	if (it != memory.variables.end()) result = memory.variables.find(key)->second;
	//exception
	else throw std::invalid_argument("Undefined variable ");
	//
	return result;
}

bool Utils::isOp(char a)
{
	return (a == '+' || a == '-' || a == '*' ||
		a == '/' || a == '%');
}

void Utils::divideAndReplace(string body, list<string>& funContainer, list<string>& argContainer)
{
	int counter = 0;
	int pos = -1;
	string op = "";

	for (int i = 0; i < body.length(); i++)
	{
		if (body[i] == '[') counter++;
		if (body[i] == ']') counter--;
		if (isOp(body[i]) && counter == 0)
		{
			pos = i;
			op = string(" ") + body[i] + string(" ");
			break;
		}
	}
	regex fn("[A-Z]+\\[.+\\]");
	string token1 = "";
	string token2 = "";
	if (pos != -1)
	{
		token1 = body.substr(0, pos - 1);
		token2 = body.substr(pos + 2);
	}

	if (pos == -1 && !(regex_match(body, fn))) return;
	if (pos == -1 && (regex_match(body, fn)))
	{
		unsigned first = body.find('[');
		unsigned second = body.rfind(']');
		string arg = body.substr(first + 1, second - first - 1);
		string fun = body.substr(0, body.find('['));

		funContainer.push_back(fun);
		argContainer.push_back(arg);
		if (!(regex_match(arg, fn))) return;
		else divideAndReplace(arg, funContainer, argContainer);
	}
	else if (regex_match(token1, fn) && pos != -1)
	{
		unsigned first = token1.find('[');
		unsigned second = token1.rfind(']');
		string arg = token1.substr(first + 1, second - first - 1);
		string fun = token1.substr(0, token1.find('['));

		funContainer.push_back(fun);
		argContainer.push_back(arg);
		divideAndReplace(arg, funContainer, argContainer);
	}
	divideAndReplace(token2, funContainer, argContainer);
}

unsigned long int Utils::CalculateBodyofFunction(string body, Container & memory)
{
	list<string> varContainer;

	regex var_reg("[a-z]+");
	sregex_iterator iter(body.begin(), body.end(), var_reg);
	sregex_iterator end;

	while (iter != end)
	{
		for (unsigned i = 0; i < iter->size(); ++i)
		{
			varContainer.push_back((*iter)[i]);
		}
		++iter;
	}
	while (!varContainer.empty())
	{
		string currVar = varContainer.front();
		string replacement = "";
		//exception
		try
		{
			replacement = to_string(getVariableValue(currVar, memory));
		}
		catch (const std::invalid_argument &err)
		{
			throw std::invalid_argument(err.what());
		}
		//
		findAndReplaceAll(body, varContainer.front(), replacement, memory);
		varContainer.pop_front();
	}
	//functions part
	list<string> funContainer;
	list<string> argContainer;

	divideAndReplace(body, funContainer, argContainer);
	while (!funContainer.empty())
	{
		string currFun = funContainer.front();
		string currArg = argContainer.front();
		string currWhole = currFun + "[" + currArg + "]";
		//exception
		string replacement = "";
		try
		{
			replacement = "(" + to_string(CalculateFunction(currFun, currArg, memory)) + ")";
		}
		catch (const std::invalid_argument &err)
		{
			throw std::invalid_argument(err.what());
		}
		//
		findAndReplaceAll(body, currWhole, replacement, memory);
		funContainer.pop_front();
		argContainer.pop_front();
	}
	unsigned long int result = 0;
	try
	{
		result = RPN().calculateRPN(RPN().stringToRPN(body));
	}
	catch (const std::invalid_argument &err)
	{
		throw std::invalid_argument(err.what());
	}
	return result;

}

unsigned long int Utils::CalculateFunction(string key, string argument, Container & memory)
{
	string funBody = "";
	string varToReplace = "";
	try
	{
		funBody = memory.functions.at(key).second;
		varToReplace = memory.functions.at(key).first;
	}
	catch (const std::out_of_range &arr)
	{
		throw std::invalid_argument("Undefined function ");
	}
	findAndReplaceAll(funBody, varToReplace, "(" + to_string(CalculateBodyofFunction(argument, memory)) + ")", memory);
	unsigned long int result = 0;
	try
	{
		result = CalculateBodyofFunction(funBody, memory);
	}
	catch (const std::invalid_argument &err)
	{
		throw std::invalid_argument(err.what());
	}
	return result;
}
