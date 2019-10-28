#pragma once

#include <string>
#include <codecvt>

namespace ssxml
{
inline std::string to_utf8(const std::string& str, const std::locale& loc = std::locale{}) {
	using namespace std::string_literals;
	using wcvt = std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t>;
	std::u32string wstr(str.size(), U'\0');
	std::use_facet<std::ctype<char32_t>>(loc).widen(str.data(), str.data() + str.size(), &wstr[0]);
	return wcvt{}.to_bytes(
		reinterpret_cast<const int32_t*>(wstr.data()),
		reinterpret_cast<const int32_t*>(wstr.data() + wstr.size())
	);
}

inline std::string from_utf8(const std::string& str, const std::locale& loc = std::locale{}) {
	using namespace std::string_literals;
	using wcvt = std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t>;
	auto wstr = wcvt{}.from_bytes(str);
	std::string result(wstr.size(), '0');
	std::use_facet<std::ctype<char32_t>>(loc).narrow(
		reinterpret_cast<const char32_t*>(wstr.data()),
		reinterpret_cast<const char32_t*>(wstr.data() + wstr.size()),
		'?', &result[0]);
	return result;
}
}