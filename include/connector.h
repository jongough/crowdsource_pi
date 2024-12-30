#ifndef _CONNECTOR_H_
#define _CONNECTOR_H_

#include <wx/wx.h>
#include <wx/thread.h>
#include <wx/fileconf.h>
#include "routecache.h"
#include "socketutils.h"
#include "avroutils.h"

class Connector : public wxThread {
private:
    Routecache *routecache;
    wxFileConfig *config;
    Socket *socket;
    AvroSchema *schema;
    int callid;

public:
    Connector(
        Routecache *routecache,
        std::string plugin_dir,
        wxFileConfig *config);
    ~Connector();
    wxThread::ExitCode Entry() override;
    AvroValueFromSchema ParseResponse(int reply_discriminant);
    void Login();
    void SendTracks();
};
    
#endif
