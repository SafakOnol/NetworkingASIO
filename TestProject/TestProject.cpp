#include <chrono>
#include <iostream>

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif
#define ASIO_STANDALONE // THIS IS TO EXCLUDE BOOST IN FRAMEWORK

#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>


// create buffer for receiving data
std::vector<char> vBuffer(1 * 1024);

void GatherData(asio::ip::tcp::socket& socket)
{
	socket.async_read_some
	(
		asio::buffer(vBuffer.data(), 
		vBuffer.size()),
		[&](std::error_code errorcode, std::size_t length)
		{
			if (!errorcode)
			{
				std::cout << "\n\nRead " << length << " byted\n\n";

				for (int i = 0; i < length; i++)
				{
					std::cout << vBuffer[i];
				}

				GatherData(socket); // looks like recursive but it's async, therefore it will read whenever it has data only
			}
		}
	); // end of async_read_some
}

int main()
{
	asio::error_code errorcode;

	// Create platform specific interface, "context:
	asio::io_context context;

	// Give idle (fake) tasks to asio so the context doesn't finish
	asio::io_context::work idleWork(context);

	// Start the context with a separate thread
	std::thread threadContext = std::thread([&]() {context.run(); });

	// Get the connection address
	// In this example a tcp style address is defined via ip addrees using the make_address function of asio
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", errorcode), 80); // port 80 is 
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", errorcode), 80); // port 80 is http
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", errorcode), 80); // port 80 is http

	// Create a socket
	asio::ip::tcp::socket socket(context);

	// Try and connect to socket
	socket.connect(endpoint, errorcode);

	// Check and print connection status
	if (!errorcode) { std::cout << "Connected!" << std::endl; }
	else { std::cout << "Failed to connect to address:\n" << errorcode.message() << std::endl; }

	if (socket.is_open())
	{
		GatherData(socket);

		std::string sRequest =
			"GET /index.html HTTP/1.1\r\n"
			"Host: example.com\r\n"
			"Connection: close\r\n\r\n";

		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), errorcode);

		//// stall the thread for 200ms -- HORRIBLE CODE, FOR TESTING PURPOSES ONLY
		//using namespace std::chrono_literals;
		//std::this_thread::sleep_for(200ms);

		socket.wait(socket.wait_read); // add delat to read function

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20000ms);
	}

	system("pause");
	return 0;
}