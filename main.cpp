#include <iostream>
#include "boost/asio.hpp"

struct ConnectionSession
{
	boost::asio::ip::tcp::socket* clientSocket = nullptr;
	std::thread* clientThread = nullptr;

	~ConnectionSession()
	{
		delete clientSocket;
		delete clientThread;
	}
};

// Vector, Array, ArrayList, List, 
// /------------------------------\
//  1 2 3 5 6 7 
// 

// pointer to previous
// pointer to next
// Data

// () -> () -> ()


std::list<ConnectionSession> connectionList;
std::mutex connectionMutex;


void clientProcessing(ConnectionSession* connectionSession)
{

	boost::system::error_code e;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		// Read some - 
		// Read until - 0

		std::vector<char> data;

		boost::asio::read_until(*connectionSession->clientSocket, boost::asio::dynamic_buffer(data), '|', e);

		if (e)
		{
			std::cout << "Reading error happened!" << std::endl;
			break;
		}

		connectionMutex.lock();

		for (auto itr = connectionList.begin(); itr != connectionList.end(); itr++)
		{
			itr->clientSocket->write_some(boost::asio::buffer(&data[0], data.size()));
		}
		connectionMutex.unlock();
	}
}


int main()
{
	std::cout << "Server started! " << std::endl;

	boost::asio::io_context context;

	boost::asio::ip::tcp::acceptor myAcceptor(context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8000));

	while (true)
	{

		boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(context);

		myAcceptor.accept(*socket);

		std::cout << "Accepted successfully!" << std::endl;

		ConnectionSession connSession;

		connectionMutex.lock();
		auto ptr = connectionList.insert(connectionList.end(), connSession);
		connectionMutex.unlock();

		ptr->clientSocket = socket;

		auto function = [ptr]() {
			clientProcessing(&(*ptr));
		};

		ptr->clientThread = new std::thread(function);

	}


}