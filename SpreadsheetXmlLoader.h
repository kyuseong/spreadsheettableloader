#pragma once

#include <functional>
#include <vector>

#include "pugixml/src/pugixml.hpp"

namespace ssxml
{

// xml 포멧의 스프레드시트 Loader
class SpreadsheetXmlLoader
{
	pugi::xml_document m_Doc;
	pugi::xml_node m_WorkbookNode;
public:
	SpreadsheetXmlLoader();
	~SpreadsheetXmlLoader();

	// 파일을 읽어서 파싱한다.
	bool Open(const char* XmlFilename);

	// 실제로 sheetname에 해당하는 데이터를 읽어서 CB으로 전달해 준다.
	bool Load(const char* sheet_name, std::function< void(int, const std::vector<const char*>&) > RowCB);

private:
	pugi::xml_node GetSheetNode(const char* sheet);
	int IncrRow(const pugi::xml_node& RowNode, int& RowIndex);
	const char* AsData(pugi::xml_node Cell, int& ColIndex);

};
}