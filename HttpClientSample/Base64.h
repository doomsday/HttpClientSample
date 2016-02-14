#pragma once
#include <cpprest\http_client.h>
using namespace utility;

class Base64
{
public:
	Base64();

	static utility::string_t constructBase64HeaderValue(const wchar_t* appendToBegin, const wchar_t* dataToConvert);

	~Base64();
};

