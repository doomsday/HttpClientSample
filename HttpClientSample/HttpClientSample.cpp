// HttpClientSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest\http_client.h>
#include <cpprest\filestream.h>
#include "Base64.h"
#include <cpprest\json.h>

//using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

typedef web::json::value::value_type JsonValueType;
#define TRUE 1
#define FALSE 0

std::wstring JsonValueTypeToString(const JsonValueType& type)
{
	switch (type)
	{
	case JsonValueType::Array: return L"Array";
	case JsonValueType::Boolean: return L"Boolean";
	case JsonValueType::Null: return L"Null";
	case JsonValueType::Number: return L"Number";
	case JsonValueType::Object: return L"Object";
	case JsonValueType::String: return L"String";
	default: return L"Error: Nonconformant type";
	}
}

int main(int argc, char* argv[])
{
	auto fileStream{ std::make_shared<ostream>() };

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.json"), std::ios_base::trunc)

		.then([=](ostream outFile)
	{
		*fileStream = outFile;

		http_request req{ methods::GET };
		req.headers().add(L"Authorization", Base64::constructBase64HeaderValue(L"Basic ", L"restapi_test:restapi_test"));
		req.set_request_uri(L"/rest/api/userchooser/search?showAll=true");

		http_client client{ U("http://csk1.arta.kz/Synergy") };
		return client.request(req);
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());

		if (response.status_code() == status_codes::OK)
			return response.extract_json();

		return pplx::create_task([] { return json::value(); });
	})

		.then([](json::value jsonValue)
	{

		if (jsonValue.is_null())
			throw std::domain_error{ "JSON value is null" };
		if (jsonValue.is_array() != TRUE)
			throw std::domain_error{ "JSON is not an array" };

		for (auto iterArray = jsonValue.as_array().cbegin(); iterArray != jsonValue.as_array().cend(); iterArray++) {

			// The array iterator returns json::values, and the object iterator now returns std::pair<string_t, json::value>
			for (auto iterObj = iterArray->as_object().cbegin(); iterObj != iterArray->as_object().cend(); ++iterObj) {

				const string_t& field{ iterObj->first };
				const string_t& value{ iterObj->second.serialize() };
				auto valueType{ JsonValueTypeToString(iterObj->second.type()) };

				std::wcout << "field: " << field << ", value: " << value << ", type: " << valueType << "\n";
			}
			std::wcout << std::endl;
		}
	});

	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		printf("Error exception:%s\n", e.what());
	}

	return 0;

}

