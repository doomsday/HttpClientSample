// HttpClientSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest\http_client.h>
#include <cpprest\filestream.h>
#include <openssl\bio.h>
#include <openssl\evp.h>
#include <openssl\applink.c>
#include <mbctype.h>

#define BUFFER_SIZE 100

using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams


int main(int argc, char* argv[])
{
	auto fileStream = std::make_shared<ostream>();

	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("results.json"), std::ios_base::trunc)

		.then([=](ostream outFile)
	{
		*fileStream = outFile;

		http_client client(U("http://csk1.arta.kz/Synergy"));

		// Convert credentials to "std::vector<unsigned char>" for "to_base64"		
		
		size_t i;
		char *pMBBuffer = (char*)malloc(BUFFER_SIZE);
		wchar_t* pWCBuffer = L"restapi_test:restapi_test";
		_wcstombs_s_l(&i, pMBBuffer, (size_t)BUFFER_SIZE, pWCBuffer, (size_t)BUFFER_SIZE, _create_locale(LC_ALL, ".1251"));


		std::string pString(pMBBuffer);
		std::vector<unsigned char> vCred(i-1);
		std::transform(pString.begin(), pString.end(), vCred.begin(),
			[](wchar_t c)
		{
			return static_cast<wchar_t>(c);
		});

		const auto b64cred = conversions::to_base64(vCred);
		http_request req(methods::GET);
		utility::string_t valBegin = U("Basic ");
		req.headers().add(L"Authorization", valBegin.append(b64cred));
		req.set_request_uri(L"/rest/api/userchooser/search");

		return client.request(req);
	})

	// Handle response headers arriving.
		.then([=](http_response response)
	{
		printf("Received response status code:%u\n", response.status_code());

		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
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

