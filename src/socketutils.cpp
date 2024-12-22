#include "socketutils.h"

Socket::Socket(const std::string& ip, int port, int min_reconnect_time, int max_reconnect_time) :
  reconnect_time(0),
  ip(ip),
  port(port),
  min_reconnect_time(min_reconnect_time),
  max_reconnect_time(max_reconnect_time) {
    sock = nullptr;
}

void Socket::ConnectionFailure(wxSocketError error, const std::string& attempt) {
    if (sock) {
        sock->Close();
        delete sock;
        sock = nullptr;
    }
    if (reconnect_time == 0) {
        reconnect_time = min_reconnect_time;
    } else if (reconnect_time < max_reconnect_time) {
        reconnect_time = reconnect_time * 2;
    }
    throw SocketException(error, attempt);
}

void Socket::Connect() {
    if (reconnect_time > 0) wxMilliSleep(reconnect_time);
 
    wxIPV4address serv_addr;
    serv_addr.Hostname(ip);
    serv_addr.Service(port);

    sock = new wxSocketClient();
    sock->SetTimeout(10);

    sock->Connect(serv_addr, false);

    sock->WaitOnConnect(10);

    if (!sock->IsConnected()) {
        ConnectionFailure(sock->LastError(), "Connection Failed");
    }

    reconnect_time = min_reconnect_time;
}

void Socket::EnsureConnection() {
    while (sock == nullptr) {
        try {
            Connect();
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
}

void Socket::Send(const std::string& data, int flags) {
    if (!sock) {
        ConnectionFailure(wxSOCKET_INVSOCK, "Socket disconnected when trying to send");
    }
    if (!sock->IsConnected()) {
        ConnectionFailure(sock->LastError(), "Socket disconnected when trying to send");
    }

    sock->Write(data.c_str(), data.size());

    if (sock->Error()) {
        ConnectionFailure(sock->LastError(), "Send failed");
    }
}

void Socket::WaitAndSendInitial(const std::string& data, int flags) {
    while (true) {
        EnsureConnection();
        try {
            Send(data, flags);
            return;
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
}
