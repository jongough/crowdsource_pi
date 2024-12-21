#ifndef _SOCKETUTILS_H_
#define _SOCKETUTILS_H_

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/utils.h> 
#include <stdexcept>
#include <string.h>

class Socket {
private:
    wxSocketClient* sock;
    int reconnect_time;
public:
    std::string ip;
    int port;
    int min_reconnect_time;
    int max_reconnect_time;
    
    Socket(const std::string& ip, int port, int min_reconnect_time, int max_reconnect_time);
    void Connect();
    void EnsureConnection();
    void ConnectionFailure(wxSocketError error, const std::string&);
    void Send(const std::string& data, int flags = 0);
    void WaitAndSendInitial(const std::string& data, int flags = 0);
};


class SocketException : public std::exception {
public:
    explicit SocketException(wxSocketError error, std::string attempt) {
        this->attempt = attempt;
        this->error = error;
        
        std::string errstr;
        switch (error)
         {
         case wxSOCKET_NOERROR:    errstr = "No error.";
         case wxSOCKET_INVOP:      errstr = "Invalid operation.";
         case wxSOCKET_IOERR:      errstr = "Input/output error.";
         case wxSOCKET_INVADDR:    errstr = "Invalid address.";
         case wxSOCKET_INVSOCK:    errstr = "Invalid socket.";
         case wxSOCKET_NOHOST:     errstr = "Host not found.";
         case wxSOCKET_INVPORT:    errstr = "Invalid port.";
         case wxSOCKET_WOULDBLOCK: errstr = "Operation would block.";
         default:                  errstr = "Unknown error.";
        }
        
        full_error = attempt + ": " + errstr;
    };
    
    wxSocketError error;
    std::string attempt;
    std::string full_error;
 
    const char* what() const noexcept override {
        return full_error.c_str();
    }
};

#endif
