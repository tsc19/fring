#ifndef DISCOVERY_H
#define DISCOVERY_H

#include <string>
#include <memory>

#include "transport.h"
#include "node_table.h"

// UDP packet format
class Packet {
private:
    // Can be extended with hash, signature, etc.
    std::string payload;

public:
    // Assemble to UDP datagram
    virtual std::string pack() const;
    // Dissemble from UDP datagram
    virtual void unpack(std::string datagram);

    // Packet design-specific functions
    std::string get_payload() const;

    void set_payload(std::string new_payload);
};


class Discovery: public Receiver, public std::enable_shared_from_this<Discovery> {
    // Let wire protocol control the table
    friend class AsyncUDPServer;
private:

    // UDP server for discovery
    std::unique_ptr<AsyncUDPServer> udp_server;

    // Node storage
    std::shared_ptr<NodeTable> node_table;

    // Implement udp receive call back
    virtual void receive(const std::string& ip, unsigned short port, const std::string& data) override;
    
    // Encapsulating low-level stuff
    void send(const std::string& ip, unsigned short port, std::string payload);

    /* Wire protocol calls */
    void send_ping(const std::string& id, const std::string& ip, unsigned short port);
    void send_pong(const std::string& ip, unsigned short port);
    
    void receive_ping(const std::string& ip, unsigned short port);
    void receive_pong(const std::string& id, const std::string& ip, unsigned short port);

    // discovery loop
    void discovery();

public:
    static const char CMD_PING = 0x01;
    static const char CMD_PONG = 0x02;

    // A port and the storage for discovery must be given
    Discovery(const std::shared_ptr<NodeTable>& node_table, unsigned short port);
    
    // Join the network and run the maintenance loop
    void start();
    // Leave the network
    void stop();

};

#endif