//
// Copyright (c) 2016-2017 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket client, asynchronous
//
//------------------------------------------------------------------------------

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

#define DIALOGMAX 10

using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

//------------------------------------------------------------------------------

// Report a failure
void
fail(boost::system::error_code ec, char const* what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

// Sends a WebSocket message and prints the response
class session : public std::enable_shared_from_this<session>
{
    tcp::resolver resolver_;
    websocket::stream<tcp::socket> ws_;
    boost::beast::multi_buffer buffer_;
    std::string host_;
    std::string* sendline_;
    int lineno_;
    int stagecounter;


public:
    // Resolver and socket require an io_context
    explicit
    session(boost::asio::io_context& ioc)
        : resolver_(ioc)
        , ws_(ioc)
    {
    }

    // Start the asynchronous operation
    std::shared_ptr<session> *
    run(
        char const* host,
        char const* port,
        std::string * sendline,
        int lineno)
    {
        // Save these for later
        host_ = host;
        sendline_ = sendline;
        lineno_=0;
        stagecounter=0;

        // Look up the domain name
        resolver_.async_resolve(
            host,
            port,
            std::bind(
                &session::on_resolve,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));

    }

    void
    on_resolve(
        boost::system::error_code ec,
        tcp::resolver::results_type results)
    {
        if(ec)
            return fail(ec, "resolve");

        // Make the connection on the IP address we get from a lookup
        boost::asio::async_connect(
            ws_.next_layer(),
            results.begin(),
            results.end(),
            std::bind(
                &session::on_connect,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_connect(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "connect");

        // Perform the websocket handshake
        ws_.async_handshake(host_, sendline_[stagecounter++],
            std::bind(
                &session::on_handshake,
                shared_from_this(),
                std::placeholders::_1));
    }

    void
    on_handshake(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "handshake");
        
        // Send the message
        ws_.async_write(
                boost::asio::buffer(sendline_[stagecounter++]),
            std::bind(
                &session::on_write,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_write(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "write");
        
        // Read a message into our buffer
        ws_.async_read(
            buffer_,
            std::bind(
                &session::on_read,
                shared_from_this(),
                std::placeholders::_1,
                std::placeholders::_2));
    }

    void
    on_read(
        boost::system::error_code ec,
        std::size_t bytes_transferred)
    {
        boost::ignore_unused(bytes_transferred);

        if(ec)
            return fail(ec, "read");

        // The buffers() function helps print a ConstBufferSequence
        std::string instring=boost::beast::buffers_to_string(buffer_.data());
        std::cout<<"receved: "<<instring<<std::endl;
        buffer_.consume(buffer_.size());

        if (++stagecounter<lineno_) {
            // Send the message
            ws_.async_write(
                    boost::asio::buffer(sendline_[stagecounter++]),
                    std::bind(
                            &session::on_write,
                            shared_from_this(),
                            std::placeholders::_1,
                            std::placeholders::_2));
        }
        /**/
        else {
            // Read a message into our buffer
            ws_.async_read(
                    buffer_,
                    std::bind(
                            &session::on_read,
                            shared_from_this(),
                            std::placeholders::_1,
                            std::placeholders::_2));
        }
        /**/
        /*
        else {
            std::cout<<"closing connection"<<std::endl;
            // Close the WebSocket connection
            ws_.async_close(websocket::close_code::normal,
                            std::bind(
                                    &session::on_close,
                                    shared_from_this(),
                                    std::placeholders::_1));
        }
        /**/
    }

    void
    on_close(boost::system::error_code ec)
    {
        if(ec)
            return fail(ec, "close");

        // If we get here then the connection is closed gracefully
    }

    void
    test(std::string s)
    {
        std::cout<<s<<std::endl;
        /*
        ws_.async_write(
                boost::asio::buffer(sendline_[0]),
                std::bind(
                        &session::on_write,
                        shared_from_this(),
                        std::placeholders::_1,
                        std::placeholders::_2));
        */
    }
};

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
    // Check command line arguments.
    if(argc != 4)
    {
        std::cerr <<
            "Usage: websocket-client-async <host> <port> <text>\n" <<
            "Example:\n" <<
            "    websocket-client-async echo.websocket.org 80 \"Hello, world!\"\n";
        return EXIT_FAILURE;
    }
    auto const host = argv[1];
    auto const port = argv[2];
    auto const dialogfile = argv[3];

    boost::filesystem::ifstream fileHandler(dialogfile);
    std::string sendline[DIALOGMAX];
    int    lineno=0;
    while(getline(fileHandler, sendline[lineno])) {
        std::cout << "read fromfile " << sendline[lineno] << std::endl;
        lineno++;
        if ( lineno==DIALOGMAX) {
            std::cout << "DIALOGMAX "<< DIALOGMAX <<" reached" << std::endl;
            break;
        }
    }

    typedef std::chrono::duration<long long int,std::ratio<1,1000>> MyMilliSecondTick;
    MyMilliSecondTick milli_1000(1000);

    // The io_context is required for all I/O
    boost::asio::io_context ioc;

    // Launch the asynchronous operation
    std::shared_ptr<session> *sess = std::make_shared<session>(ioc)->run(host, port, sendline, lineno);
    /**/
    session *psess=sess->get();
    // Run the I/O service. The call will return when
    // the socket is closed.
    //ioc.run();

    do {
        size_t handlers_executed=ioc.run_for( milli_1000 );
        std::cout<<"executed "<< handlers_executed<<" handlers"<<std::endl;
        if(ioc.stopped()) {
            std::cout<<"leaving handler execution loop"<<std::endl;
            break;
        }
        ioc.restart();
        psess->test("bananentest von aussen");
    }while(1);
    /**/


    return EXIT_SUCCESS;
}
