#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>

#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <boost/filesystem.hpp>

//#include <boost/filesystem/ifstream.hpp>


#include <cstdlib>
#include <iostream>
#include <string>

#define DIALOGMAX 10

using namespace boost::archive::iterators;
using namespace std;
using tcp = boost::asio::ip::tcp;               // from <boost/asio/ip/tcp.hpp>
namespace websocket = boost::beast::websocket;  // from <boost/beast/websocket.hpp>

// Sends a WebSocket message and prints the response
int main(int argc, char** argv)
{
    try
    {
        // Check command line arguments.
        if(argc != 4)
        {
            cerr <<
            "Usage: "<< argv[0] <<" websocket-client-sync <host> <port> <dialog>\n" <<
            "Example:\n" <<
            argv[0] <<" 127.0.0.1 6001 ../dialog.txt\n";
            return EXIT_FAILURE;
        }
        auto const host = argv[1];
        auto const port = argv[2];
        auto const dialog = argv[3];

        boost::filesystem::ifstream fileHandler(dialog);
        string sendline[DIALOGMAX];
        int    lineno=0;
        while(getline(fileHandler, sendline[lineno])) {
            cout << "read fromfile " << sendline[lineno] << endl;
            lineno++;
            if ( lineno==DIALOGMAX) {
                cout << "DIALOGMAX "<< DIALOGMAX <<" reached";
                break;
            }
        }

        // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        websocket::stream<tcp::socket> ws{ioc};

        cout <<"resolving "<<host<<endl;
        // Look up the domain name
        auto const results = resolver.resolve(host, port);

        cout <<"connecting "<<endl;
        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(ws.next_layer(), results.begin(), results.end());

        cout << "handshake: " << sendline[0] << endl;
        // Perform the websocket handshake
        ws.handshake(host, sendline[0]);

        // This buffer will hold the incoming message
        boost::beast::multi_buffer buffer;

        //How many opening telegrams do we have in sendline ?
        unsigned stagecounter=1;

        bool finished=false;
        // Read a message into our buffer
        do
        {
            cout <<" wait for response "<<endl;
            ws.read(buffer);
            string instring=boost::beast::buffers_to_string(buffer.data());
            cout<<"receved: "<<instring<<endl;
            buffer.consume(buffer.size());

            if( instring.find("there") != std::string::npos ) {
                cout<<"leaving main loop"<<endl;
                finished=true;
            }

            //boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));

            //send an opener if available ?
            if(stagecounter<lineno) {
                cout <<"send: "<< sendline[stagecounter] << endl;
                ws.write(boost::asio::buffer(sendline[stagecounter]));
                stagecounter++;
            }

        }while(!finished);

        // Close the WebSocket connection
        ws.close(websocket::close_code::normal);

        // If we get here then the connection is closed gracefully
    }
    catch(exception const& e)
    {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
