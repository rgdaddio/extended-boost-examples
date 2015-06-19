//
// blocking_tcp_echo_client.cpp 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// with ssl support
// ~~~~~~~~~~~~~~~~
// Copyright (c) 2003-2011 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// 06-2015 Rich D'Addio added SSL support
// This should talk to the ssl example e.g.
// http://www.boost.org/doc/libs/1_47_0/doc/html/boost_asio/example/ssl/server.cpp 
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//g++ <*.cpp>  -lboost_system -lboost_thread -lpthread -lboost_system -lcrypto -lssl -o <client>
// ./client <host> <port>
 
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
  try
  {
    if (argc != 3)
    {
      std::cerr << "Usage: blocking_tcp_echo_client <host> <port>\n";
      return 1;
    }
    
    boost::system::error_code ec;
    boost::asio::io_service io_service;
    boost::asio::ssl::context context(io_service, boost::asio::ssl::context::sslv23);
    
    context.set_verify_mode(boost::asio::ssl::context::verify_peer);
    context.load_verify_file("server.crt");
    
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(tcp::v4(), argv[1], argv[2]);
    tcp::resolver::iterator iterator = resolver.resolve(query);
    
    typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> SSLSocket;
    SSLSocket s(io_service, context);

    s.lowest_layer().connect(*iterator, ec);

    s.handshake(SSLSocket::client, ec);


    std::cout << "Enter message: ";
    char request[max_length];
    std::cin.getline(request, max_length);
    size_t request_length = std::strlen(request);
    boost::asio::write(s, boost::asio::buffer(request, request_length));

    char reply[max_length];
    size_t reply_length = boost::asio::read(s,
					    boost::asio::buffer(reply, request_length));
    std::cout << "Reply is: ";
    std::cout.write(reply, reply_length);
    std::cout << "\n";

    s.shutdown(ec);
    s.next_layer().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    s.next_layer().close(ec);

  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
