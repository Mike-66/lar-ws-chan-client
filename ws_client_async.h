//
// Created by Thomas on 15.08.2021.
//

#ifndef FIRST_WS_CLIENT_ASYNC_H
#define FIRST_WS_CLIENT_ASYNC_H

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

#include <boost/filesystem.hpp>

#include <boost/bind.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/bind_executor.hpp>

#define DIALOGMAX 10

typedef std::chrono::duration<long long int,std::ratio<1,1000>> MyMilliSecondTick;

class ws_client_async {

private:
protected:

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

public:
    ws_client_async();
    ~ws_client_async();

    int Init(const char*  host, const char* port, const char*  dialogfile);

    int run(int millis);

};


#endif //FIRST_WS_CLIENT_ASYNC_H
