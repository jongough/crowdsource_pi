#include "socketutils.h"

Socket::Socket(const std::string& ip, int port, int min_reconnect_time, int max_reconnect_time, CancelFunction cancel_function) :
  reconnect_time(0),
  ip(ip),
  port(port),
  min_reconnect_time(min_reconnect_time),
  max_reconnect_time(max_reconnect_time),
  cancel_function(cancel_function) {
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

bool Socket::TryCancel() {
    if (!cancel_function) return false;
    return cancel_function();
}

void Socket::Connect() {
    int slept;
    int tosleep;
    // Sleep reconnect_time ms, but check TryCancel regularly
    if (reconnect_time > 0) {
        slept = 0;
        while (!TryCancel() && (slept < reconnect_time)) {
            tosleep = reconnect_time - slept > 200 ? 200 : reconnect_time - slept;
            wxMilliSleep(tosleep);
            slept += tosleep;
        }
    }
    wxIPV4address serv_addr;
    serv_addr.Hostname(ip);
    serv_addr.Service(port);

    sock = new wxSocketClient();
    sock->SetTimeout(10);

    sock->Connect(serv_addr, false);

    slept = 0;
    while (!TryCancel() && (slept < 10000)) {
        tosleep = 10000 - slept > 200 ? 200 : 10000 - slept;
        if (sock->WaitOnConnect(0, tosleep)) break;
        slept += tosleep;
    }

    if (!sock->IsConnected()) {
        ConnectionFailure(sock->LastError(), "Connection Failed");
    }

    reconnect_time = min_reconnect_time;
}

void Socket::EnsureConnection() {
    while (!TryCancel() && !sock) {
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
    while (!TryCancel()) {
        EnsureConnection();
        try {
            Send(data, flags);
            return;
        } catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }
}
