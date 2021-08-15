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

    ws_client_async *wsc = new ws_client_async();
    wsc->Init(host,port,dialogfile);

    do {
        if(wsc->run(1000)!=0)
        break;
    }while(1);

    return EXIT_SUCCESS;
}
