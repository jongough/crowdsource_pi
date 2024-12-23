#include "connector.h"
#include <wx/string.h>
#include <wx/fileconf.h>

Connector::Connector(Routecache *routecache, std::string plugin_dir) :
  wxThread(wxTHREAD_DETACHED),
  routecache(routecache),
  socket(nullptr),
  schema(nullptr),
  finalize(false),
  finalized(false),
  callid(0) {
      wxString slash(wxFileName::GetPathSeparator());
      wxString schema_file = wxString(plugin_dir) + slash + wxString("proto_avro.json");
      wxFile file(schema_file);
      wxString schema_json;
      if (!file.IsOpened()) {
          throw std::runtime_error("Unable to open file: " + schema_file.ToStdString());
      }    
      file.ReadAll(&schema_json, wxConvUTF8);
      schema = new AvroSchema(std::string(schema_json.ToUTF8()));
}

Connector::~Connector() {
    finalize = true;
    while (!finalized) wxThread::Sleep(500);
    if (socket) delete socket;
    socket = nullptr;
    if (schema) delete schema;
    schema = nullptr;
}

void Connector::SendTracks() {
    AvroValueFromSchema value(*schema);
    AvroValue call = value.SetCurrentBranch(0).Add("Call");
    call.Add("id").Set(++callid);
    AvroValue submit = call.Add("Call").SetCurrentBranch(1).Add("Submit");
    
    routecache->Retrieve(submit);
    socket->WaitAndSendInitial(value.Serialize(), 0);
    routecache->MarkAsSent(submit);
}

wxThread::ExitCode Connector::Entry() {
    struct Finalizer {
        bool& finalized;
        Finalizer(bool& f) : finalized(f) {}
        ~Finalizer() { finalized = true; }
    };
    Finalizer finalizer(finalized);
 
    socket = new Socket("127.0.0.1", 9900, 100, 60000);
    while (!finalize) {
        SendTracks();
        wxThread::Sleep(10000);
    }
    return 0;
};
