#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <wx/wx.h>
#include <wx/thread.h>
#include "routecache.h"
#include "socketutils.h"
#include "avroutils.h"

class Connector : public wxThread {
private:
    Routecache *routecache;
    Socket *socket;
    AvroSchema *schema;
    bool finalize;
    bool finalized;
    int callid;

public:
    Connector(Routecache *routecache, std::string plugin_dir);
    ~Connector();
    wxThread::ExitCode Entry() override;
    void SendTracks();
};
    
#endif
