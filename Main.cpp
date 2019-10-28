#include "pch.h"
#include <iostream>
#include <vector>
#include <cassert>

#include "SpreadsheetTableLoader.h"

struct ExData
{
	int col1;
	std::string col2;
	std::string col3;
	int col4;
	int col5;
};

void Bind(ssxml::TableBinder& loader, ExData* Table)
{
	loader.Bind("col1", Table->col1);
	loader.Bind("col3", Table->col3);
	loader.Bind("col5", Table->col5);
	loader.Bind("col2", Table->col2);
	loader.Bind("col4", Table->col4);
}

int main()
{
	ssxml::XmlTableLoader Loader("Ex.xml");

	std::vector<ExData> data = Loader.LoadSheet<ExData>("Ex");
	
	return 0;
}
