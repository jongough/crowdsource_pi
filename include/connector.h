#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <wx/wx.h>
#include <wx/thread.h>
#include "routecache.h"
#include "socketutils.h"

class Connector : public wxThread {
private:
    Routecache *routecache;
    Socket *socket;
    bool finalize;
    
public:
    Connector(Routecache *routecache);
    ~Connector();
    wxThread::ExitCode Entry() override;
};
    
#endif
