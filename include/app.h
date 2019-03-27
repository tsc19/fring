#ifndef APP_H
#define APP_H

#include <string>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

#include "peer_manager.h"
#include "node.h"
#include "node_table.h"
#include "param_constants.h"
#include "message.h"
#include "utils.h"

// string of data of length 1024 bytes
static const std::string one_kb_data(1024, 'S');
static const std::string data_of_block_size(1024*17, 'S');

class BaseApp {
private:
    std::shared_ptr<Node> node;
    std::shared_ptr<NodeTable> node_table;
    std::shared_ptr<PeerManager> peer_manager;

public:
    // constructor
    BaseApp(std::string ip, unsigned short port, std::string id);

    // getters
    std::shared_ptr<Node> get_node();
    std::shared_ptr<NodeTable> get_node_table();
    std::shared_ptr<PeerManager> get_peer_manager();

    // form the network topology
    void form_structure(int num_nodes_in_dist, int num_cnodes_in_dist, 
    	int num_nodes_in_city, int num_cnodes_in_city, 
    	int num_nodes_in_state, int num_cnodes_in_state, 
    	int num_nodes_in_country, int num_cnodes_in_country, 
    	int num_nodes_in_continent, int num_continents,
        int num_cnodes_in_continent,
        unsigned short starting_port_number);

    // start the application
    void start(const std::string &start_time, int num_nodes_in_dist, int num_cnodes_in_dist, 
        int num_nodes_in_city, int num_cnodes_in_city, 
        int num_nodes_in_state, int num_cnodes_in_state, 
        int num_nodes_in_country, int num_cnodes_in_country, 
        int num_nodes_in_continent, int num_continents,
        int num_cnodes_in_continent,
        unsigned short starting_port_number);

    // stop the application
    void stop();

    // broadcast
    void broadcast(const std::string &data);
};

#endif