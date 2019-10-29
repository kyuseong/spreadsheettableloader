#pragma once

#include <string>
#include <codecvt>
#include <stringapiset.h>

namespace ssxml
{


inline std::wstring utf8_to_wstring(const std::string& s)
{
	int len;
	int slength = (int)s.length();
	len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, 0, 0);
	std::wstring r(len, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, s.c_str(), slength, &r[0], len);
	return r;
}

inline std::string utf8_to_string(const char *utf8str, const std::locale& loc = std::locale())
{
	// UTF-8 to wstring
	std::wstring_convert<std::codecvt_utf8<wchar_t>> wconv;
	std::wstring wstr = wconv.from_bytes(utf8str);
	// wstring to string
	std::vector<char> buf(wstr.size());
	std::use_facet<std::ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
	return std::string(buf.data(), buf.size());
}


}