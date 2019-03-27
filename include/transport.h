#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <string>
#include <queue>
#include <iostream>
#include <thread>
#include <condition_variable>

#include <string.h>
#include <stdint.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <fcntl.h>
#include <unistd.h>

template <typename T>
class AtomicQueue {

public:
    void enqueue_and_notify(const T&);

    T wait_for_dequeue();
    
private:
    std::queue<T> q;

    std::mutex buffer_mutex;
    std::condition_variable buffer_refreshed_cv;
};

template <typename T>
void AtomicQueue<T>::enqueue_and_notify(const T& t) {
    std::unique_lock<std::mutex> buffer_lock(buffer_mutex);
    this->q.push(t);
    buffer_refreshed_cv.notify_one();
    buffer_lock.unlock();
}

template <typename T>
T AtomicQueue<T>::wait_for_dequeue() {
    std::unique_lock<std::mutex> buffer_lock(buffer_mutex);
    while(this->q.empty()) {
        buffer_refreshed_cv.wait(buffer_lock);
    }
    T t = q.front();
    q.pop();
    buffer_lock.unlock();
    return t;
};


// Classes that implement Receiver can be registered to the server
// receive() will be called to handle received data
class Receiver {
public:
    virtual void receive(const std::string& ip, unsigned short port, const std::string& data) = 0;

    // For safe destruction
    virtual ~Receiver() = 0;
};

using BufferItemType = std::tuple<std::string, unsigned short, std::string>;

// A class that implements asynchronous UDP send and receive 
class AsyncUDPServer {
public:
    static const std::size_t BUFFER_SIZE = 65536;

    // Constructor
    AsyncUDPServer(const std::shared_ptr<Receiver>& receiver, unsigned short port);

    // run the receive loop
    void run();

    // stop the server
    void stop();

    // Encapsulate low-level mechanism
    void send(const std::string& ip, unsigned short port, const std::string& data);

    ~AsyncUDPServer();

private:
    // Registered upper level receiver, calling its receive(const std::string & , unsigned short , const std::string & )
    std::shared_ptr<Receiver> receiver;

    // One thread for sending, another for calling upper-level
    std::thread receive_worker;
    std::thread buffer_handler;

    std::unique_ptr<AtomicQueue<BufferItemType>> buffer;

    // Routine of the threads
    void receive();
    void handle();

    // Socket fd
    int32_t udp_socket_id_;

};

#ifdef NDEBUG
// A class that implements asynchronous TCP send and receive
class TCPConnection: public boost::enable_shared_from_this<TCPConnection> {
public:
    static const std::size_t BUFFER_SIZE = 65536;

    using Pointer = boost::shared_ptr<TCPConnection>;

    static Pointer Create(boost::asio::io_service& io_service,
        const std::shared_ptr<AtomicQueue<BufferItemType>>& buffer);

    tcp::socket& get_socket();
    void reset_socket(boost::asio::io_service& io_service);

    void start();

    void stop();

    void write(const std::string& ip, unsigned short port, const const std::string& data);

private:
    using Header = unsigned long;

    std::unique_ptr<tcp::socket> socket;
    std::array<char, BUFFER_SIZE> read_buffer;
    unsigned long total_length;
    std::string datagram;
    
    // Clinet
    tcp::resolver resolver;

    std::shared_ptr<AtomicQueue<BufferItemType>> buffer;

    TCPConnection(boost::asio::io_service& io_service,
        const std::shared_ptr<AtomicQueue<BufferItemType>>& buffer);
    
    void read();

    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(boost::shared_ptr<std::string> data, const boost::system::error_code& error, size_t bytes_transferred);
};


class AsyncTCPServer {
public:
    // Constructor
    AsyncTCPServer(const std::shared_ptr<Receiver>& receiver, unsigned short port);
    
    // run the receive loop
    void run();

    // stop the server
    void stop();

    // encapsulate low-level machanism
    void send(const std::string& ip, unsigned short port, const std::string& data);
    
private:
    std::shared_ptr<Receiver> receiver;

    boost::asio::io_service io_service;
    std::unique_ptr<boost::asio::io_service::work> work;
    std::thread io_worker;

    // std::unordered_map<std::string, TCPConnection::Pointer> tcp_connections;
        TCPConnection::Pointer send_conn;

    std::thread handler;

    // Server
    unsigned short port;
    tcp::acceptor acceptor;
    // Client
    tcp::resolver resolver;

    std::shared_ptr<AtomicQueue<BufferItemType>> buffer;


    void accept();

    void handle_accept(TCPConnection::Pointer connection,
        const boost::system::error_code& error);
    
    void handle_resolve(const boost::system::error_code& error,
        tcp::resolver::iterator endpoint_iterator,
        std::shared_ptr<std::string> datagram);
    void handle_connect(const boost::system::error_code& error,
        tcp::resolver::iterator endpoint_iterator,
        std::shared_ptr<std::string> datagram);
    
    // Buffer handler
    void handle();
    void receive();
};
#else

using AsyncTCPServer = AsyncUDPServer;
#endif

#endif
