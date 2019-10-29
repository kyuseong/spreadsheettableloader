#include "pch.h"
#include "SpreadsheetXmlLoader.h"

#include <iostream>

#include "StringUtils.h"

namespace ssxml
{

SpreadsheetXmlLoader::SpreadsheetXmlLoader()
{
}

SpreadsheetXmlLoader::~SpreadsheetXmlLoader()
{
}

// 파일을 읽어서 파싱한다.
bool SpreadsheetXmlLoader::Open(const char* XmlFilename)
{
	auto result = m_Doc.load_file(XmlFilename, pugi::parse_default, pugi::encoding_utf8);
	if (!result)
	{
		std::cerr << "open failed - status:" << result.status << ",desc:" << result.description() << std::endl;
		return false;
	}

	auto WorkbookNode = m_Doc.child("Workbook");
	if (strcmp(WorkbookNode.name(), "Workbook") != 0 && 
		strcmp(WorkbookNode.name(), "ss:Workbook") != 0)
	{
		std::cerr << "open failed - can't find workbook" << std::endl;
		return false;
	}

	m_WorkbookNode = WorkbookNode;

	return true;
}

// sheet 이름에 해당하는 sheet node 을 반환한다.
pugi::xml_node SpreadsheetXmlLoader::GetSheetNode(const char* sSheetName)
{
	for (auto& workbook : m_WorkbookNode)
	{
		if (strcmp(workbook.name(), "Worksheet") != 0 && strcmp(workbook.name(), "ss:Worksheet") != 0)
			continue;

		auto sheet = workbook.attribute("ss:Name");
		const char* sheetName = sheet.value();
		if (nullptr == sheetName)
			continue;

		auto Table = workbook.child("Table");
		if (!Table)
			Table = workbook.child("ss:Table");
		if (!Table)
			continue;

		std::string SheetName = ssxml::utf8_to_string(sheetName);

		if (SheetName == sSheetName)
		{
			return Table;
		}
	}
	return pugi::xml_node();
}

// 현재 줄 수를 반환한다.
int SpreadsheetXmlLoader::IncrRow(const pugi::xml_node& RowNode, int& RowIndex)
{
	auto str_row_attr = RowNode.attribute("ss:Index");
	const char* str_row = str_row_attr.as_string();
	if (str_row != nullptr && str_row[0] != '\0')
		RowIndex = atoi(str_row);
	else
		RowIndex++;

	return RowIndex;
}

// 현재 셀 인덱스와 값을 반환한다.
const char* SpreadsheetXmlLoader::AsData(pugi::xml_node Cell, int& ColIndex )
{
	const char* value = nullptr;

	auto ColAttr = Cell.attribute("ss:Index");
	if (!!ColAttr)
	{
		ColIndex = ColAttr.as_int();
	}
	else
		ColIndex++;

	auto Data = Cell.child("Data");
	if (!!Data)
	{
		auto Val = Data.first_child();
		if (!!Val)
		{
			const char* data = Val.value();
			if (data && data[0] != '\0')
			{
				value = data;
			}
		}
	}

	auto mergeAcrossAttr = Cell.attribute("ss:MergeAcross");
	if (!!mergeAcrossAttr)
	{
		ColIndex += mergeAcrossAttr.as_int();
	}

	return value;
}

// 실제로 sheetname에 해당하는 데이터를 읽어서 CB으로 전달해 준다.
bool SpreadsheetXmlLoader::Load(const char* SheetName,
	std::function< void(int, const std::vector<const char*>&) > RowCB)
{
	pugi::xml_node SheetNode = GetSheetNode(SheetName);
	if (SheetNode == false)
	{
		return false;
	}

	int TotalCol = SheetNode.attribute("ss:ExpandedColumnCount").as_int();
	
	std::vector<const char*> ColsData;
	ColsData.resize(TotalCol);

	int RowIndex = 0;
	for (auto& RowNode : SheetNode)
	{
		if (strcmp(RowNode.name(), "Row") != 0 && strcmp(RowNode.name(), "ss:Row") != 0)
			continue;

		std::fill(ColsData.begin(), ColsData.end(), nullptr);

		// 현재 유효한 로우를 반환
		RowIndex = IncrRow(RowNode, RowIndex);
		
		int ColIndex = 0;
		for (auto& cell_node : RowNode)
		{
			if (strcmp(cell_node.name(), "Cell") != 0 && strcmp(cell_node.name(), "ss:Cell") != 0)
				continue;

			// 해당 셀의 값을 반환하고 현재 인덱스를 증가
			const char* data = AsData(cell_node, ColIndex);
			ColsData[ColIndex - 1] = data;
		}

		RowCB(RowIndex, ColsData);
	}

	return true;
}

}