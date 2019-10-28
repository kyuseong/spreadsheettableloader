#pragma once

#include <algorithm>

#include "SpreadsheetXmlLoader.h"

namespace ssxml
{

// binder
class TableBinder
{
public:
	virtual bool Bind(const char* col, std::string &val) = 0;
	virtual bool Bind(const char* col, int& val) = 0;
	virtual bool Bind(const char* col, float& val) = 0;
};

// binder
class TableHeadBinder : public TableBinder
{
	class Column
	{
	public:
		std::string m_Col;
		int m_Index{-1};
		// const char* m_Data;

		Column()
		{}
		Column(const char* col) : m_Col(col)
		{}
	};
	std::vector<Column> m_Cols;

public:
	bool Bind(const char* col, std::string &val)
	{
		m_Cols.push_back(col);
		return true;
	}
	bool Bind(const char* col, int& val)
	{
		m_Cols.push_back(col);
		return true;
	}
	bool Bind(const char* col, float& val)
	{
		m_Cols.push_back(col);
		return true;
	}

	void SetValue(const std::vector<const char*> & row)
	{
		for (int i = 0;i < row.size();++i)
		{
			for (auto & Col : m_Cols)
			{
				if (Col.m_Col == row[i])
				{
					Col.m_Index = i;
					break;
				}
			}
		}
	}

	int GetIndex(const char* col)
	{
		auto Itr = std::find_if(m_Cols.begin(), m_Cols.end(), [col](const auto& v) { return v.m_Col == col; });
		if (Itr == m_Cols.end()) {
			assert(false);
			return -1;
		}

		assert(Itr->m_Index != -1);
		return Itr->m_Index;
	}
};

// binder
class TableDataBinder : public TableBinder
{
	TableHeadBinder& m_Header;
	const std::vector<const char*>* m_Datas;

	std::vector<int> m_Indexed;
	int m_Cur;

public:
	TableDataBinder(TableHeadBinder& binder) : m_Header(binder)
	{
		m_Cur = 0;
		m_Datas = nullptr;
	}

	int GetIndex(const char* col)
	{
		if (m_Cur < m_Indexed.size())
		{
			return m_Indexed[m_Cur++];
		}

		int Index = m_Header.GetIndex(col);
		m_Indexed.push_back(Index);
		m_Cur++;

		return Index;
	}

	bool Bind(const char* Col, std::string &val)
	{
		const char* Data = GetValue(Col);

		val = Data;
		return true;
	}

	bool Bind(const char* Col, int& val)
	{
		int index = GetIndex(Col);

		val = atoi(m_Datas->at(index));
		return true;
	}

	bool Bind(const char* col, float& val)
	{
		int index = GetIndex(col);

		val = (float)atof(m_Datas->at(index));

		return true;
	}

	void SetValue(const std::vector<const char*>& Datas)
	{
		m_Datas = &Datas;
		Clear();
	}

	const char* GetValue(const char* Col)
	{
		int Index = GetIndex(Col);
		return m_Datas->at(Index);
	}

	void Clear()
	{
		m_Cur = 0;
	}
};

// xml table읽기
class XmlTableLoader
{
	SpreadsheetXmlLoader loader;
public:
	XmlTableLoader(const char* xml)
	{
		loader.Open(xml);
	}

	// sheet 을 읽어들임
	// head 정보 컬럼을 셋팅해야한다.
	template<class T>
	std::vector<T> LoadSheet(const char* sheet, int HeadCol = 1)
	{
		std::vector<T> Table;
		TableHeadBinder Binder;
		{
			T Dummy;
			Bind(Binder, &Dummy);
		}

		TableDataBinder DataBinder(Binder);
		loader.Load(sheet, [HeadCol, &Binder, &DataBinder, &Table](int RowIndex, const std::vector<const char*> & RowDatas)
		{
			if (RowIndex == HeadCol)
			{
				Binder.SetValue(RowDatas);
			}
			else
			{
				DataBinder.SetValue(RowDatas);

				T Data;
				Bind(DataBinder, &Data);

				Table.push_back(Data);
			}
		}
		);

		return Table;
	}
};


}