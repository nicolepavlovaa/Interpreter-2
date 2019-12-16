#include "Menu.h"

void Menu::start()
{
	string line;
	string path = "";

	cout << "Please open a file: ";
	cin >> path;
	bool succOpened = true;

	ifstream myfile;
	myfile.open(path);
	if (myfile.is_open())
	{
		if (succOpened) cout << "Successfully opened " << path << endl;
		Container p;
		string line;
		int counter = 0;
		while (!myfile.eof())
		{
			counter++;
			getline(myfile, line);
			{
				bool valid = false;
				try
				{
					valid = Validation::validateLine(line, p);
					if (!valid)
					{
						cout << "Error! Syntax error at line " << counter << endl;
						return;
					}
				}
				catch (const std::invalid_argument &read)
				{
					cout << "Error! " << read.what() << " at line " << counter << endl;
					return;
				}
			}
		}
	}
	myfile.close();
}
