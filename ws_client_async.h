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

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>

#define DIALOGMAX 10

typedef std::chrono::duration<long long int,std::ratio<1,1000>> MyMilliSecondTick;


// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session> {
    // Access the derived class, this is part of
    // the Curiously Recurring Template Pattern idiom.

    boost::asio::ip::tcp::resolver *resolver_;
    boost::beast::websocket::stream<boost::asio::ip::tcp::socket> ws_;
    boost::beast::multi_buffer buffer_;
    std::string host_;
    std::string *sendline_;
    int lineno_;
    int stagecounter;

protected:
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::steady_timer timer_;

    void do_timeout(void);
    void on_timer(boost::system::error_code ec);
    void set_timer(int t);

    void on_resolve(boost::system::error_code ec,boost::asio::ip::tcp::resolver::results_type results);
    void on_connect(boost::system::error_code ec);
    void on_handshake(boost::system::error_code ec);
    void on_write(boost::system::error_code ec,std::size_t bytes_transferred);
    void on_read(boost::system::error_code ec,std::size_t bytes_transferred);
    void on_close(boost::system::error_code ec);
    void test(std::string s);

public:
    // Resolver and socket require an io_context
    explicit session(boost::asio::io_context &ioc);
    ~session();

    std::shared_ptr<session> * run(char const *host,
                                   char const *port,
                                   std::string *sendline,
                                   int lineno);
};


class ws_client_async {

private:
protected:

    //std::shared_ptr<session> *sess;
    std::string sendline[DIALOGMAX];
    std::shared_ptr<session> *sess;

public:
    ws_client_async();
    ~ws_client_async();

    //std::shared_ptr<session> * Init(const char*  host, const char* port, const char*  dialogfile);
    int Init(const char*  host, const char* port, const char*  dialogfile);

    static int run(int millis);

};


#endif //FIRST_WS_CLIENT_ASYNC_H
