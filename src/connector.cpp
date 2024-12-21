#include "connector.h"

Connector::Connector(Routecache *routecache) :
  wxThread(wxTHREAD_DETACHED),
  routecache(routecache) { }

Connector::~Connector() {
    delete socket;
}

wxThread::ExitCode Connector::Entry() {
    socket = new Socket("127.0.0.1", 9900, 100, 60000);
    while (true) {
        socket->WaitAndSendInitial("Hello world", 0);
        wxThread::Sleep(10000);
    }
    return 0;
};
