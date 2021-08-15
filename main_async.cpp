#include "ws_client_async.h"

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
    const char* host = argv[1];
    const char* port = argv[2];
    const char* dialogfile = argv[3];

    ws_client_async *wsc=NULL;
reconnect:
    if (wsc != NULL)
        delete(wsc);
    wsc = new ws_client_async();
    wsc->Init(host,port,dialogfile);

    int runs=0;
    int millis=1000;
    do {
        std::cout<<"performing run "<<runs<<std::endl;
        if(runs++==100)
            millis=-1;
        if(wsc->run(millis)!=0) {
            if(millis==-1)
                break;
            else {
                boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
                goto reconnect;
            }
        }
    }while(1);

    return EXIT_SUCCESS;
}
