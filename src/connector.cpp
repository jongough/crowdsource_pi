#include "connector.h"
#include <wx/string.h>
#include <wx/fileconf.h>

Connector::Connector(Routecache *routecache, std::string plugin_dir, wxFileConfig *config) :
  wxThread(wxTHREAD_JOINABLE),
  routecache(routecache),
  config(config),
  socket(nullptr),
  schema(nullptr),
  callid(0) {
      wxString slash(wxFileName::GetPathSeparator());
      wxString schema_file =
          wxString(plugin_dir)
          + slash + wxString("data")
          + slash + wxString("proto_avro.json");
      wxFile file(schema_file);
      wxString schema_json;
      if (!file.IsOpened()) {
          throw std::runtime_error("Unable to open file: " + schema_file.ToStdString());
      }    
      file.ReadAll(&schema_json, wxConvUTF8);
      schema = new AvroSchema(std::string(schema_json.ToUTF8()));
}

Connector::~Connector() {
    if (socket) delete socket;
    socket = nullptr;
    if (schema) delete schema;
    schema = nullptr;
}

void Connector::Login() {
    wxString api_key;
    config->Read("/Server/api_key", &api_key, "");

    AvroValueFromSchema value(*schema);
    AvroValue call = value.Get("Message").SetCurrentBranch(0).Get("Call");
    call.Get("id").Set(++callid);
    AvroValue login = call.Get("Call").SetCurrentBranch(0).Get("Login");
    login.Get("apikey").Set(std::string(api_key.ToUTF8()));
    
    value.Debug();
    socket->Send(value.Serialize(), 0);
}

void Connector::SendTracks() {
    AvroValueFromSchema value(*schema);
    AvroValue call = value.Get("Message").SetCurrentBranch(0).Get("Call");
    call.Get("id").Set(++callid);
    AvroValue submit = call.Get("Call").SetCurrentBranch(1).Get("Submit");
    
    if (!routecache->Retrieve(submit)) return;
    value.Debug();
    socket->Send(value.Serialize(), 0);
    routecache->MarkAsSent(submit);
}

wxThread::ExitCode Connector::Entry() {
    wxString server;
    long port;
    float min_reconnect_time;
    float max_reconnect_time;
    config->Read("/Server/server", &server, "crowdsource.kahu.earth");
    config->Read("/Server/port", &port, 9900);
    config->Read("/Connection/min_reconnect_time", &min_reconnect_time, 100.0);
    config->Read("/Connection/max_reconnect_time", &max_reconnect_time, 600.0);
    
    socket = new Socket(
        std::string(server.ToUTF8()),
        port,
        min_reconnect_time,
        max_reconnect_time,
        [this]() { return this->TestDestroy(); },
        [this]() { return this->Login(); }
    );

    while (!TestDestroy()) {
        try {
            socket->EnsureConnection();
            SendTracks();
            wxThread::Sleep(500);
        } catch (const std::exception& e) {
            std::cerr << e.what() << " in Connector\n";
        }
    }
    return 0;
};
