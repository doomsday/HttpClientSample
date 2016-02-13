// HttpClientSample.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cpprest\http_client.h>
#include <cpprest\filestream.h>

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

//		auto b64cred = conversions::to_base64(std::string("Скрипников:1"));
		http_request req(methods::GET);
		req.headers().add(L"Authorization", L"Basic 0KHQutGA0LjQv9C90LjQutC+0LI6MQ==");
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

