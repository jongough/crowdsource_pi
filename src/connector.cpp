#include "connector.h"

Connector::Connector(Routecache *routecache) :
  wxThread(wxTHREAD_DETACHED),
  routecache(routecache),
  finalize(false) { }

Connector::~Connector() {
    finalize = true;
}

wxThread::ExitCode Connector::Entry() {
    socket = new Socket("127.0.0.1", 9900, 100, 60000);
    while (!finalize) {
        socket->WaitAndSendInitial("Hello world", 0);
        wxThread::Sleep(10000);
    }
    delete socket;
    socket = nullptr;
    
    return 0;
};
