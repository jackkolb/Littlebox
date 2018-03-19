#ifndef __SERVER_HEADER_H__
#define __SERVER_HEADER_H__

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdio>

#include <ctime>

#include <stdio.h>
#include <vector>
#include <string>

using boost::asio::ip::tcp;

class Connection {
	public:
		std::string FILE_MAP_PATH = "./server/file_map"; // directory of the file map
		std::string FILE_KEYS_PATH = "./server/key_map"; // directory of the key map
		std::string PASSCODE_PATH = "./server/passcode"; // directory of the server password
		std::string log;
		
		void handle_connection(tcp::socket&, std::string); // manages a connection request
		void generate_file_map(); // generates a default file map
		void generate_key_map(); // generates a default key map

	private:
		int MAX_PACKET_LENGTH = 1024; // the standard packet length

		bool verify_message(std::string); // determines if a message is valid

		void push_file(tcp::socket&, std::string, int, std::string); // push a file to the server (adds it to the server)
		void pull_file(tcp::socket&, int); // pulls a file from the server (retrieves it from the server)
		void delete_file(tcp::socket&, int); // delete a file from the server (removes it from the server)

		std::string get_file_name(int); // gets the associated name of a file from the ID
		std::string get_file_key(int); // gets the associated key of a file from the id
		std::string verify_id(std::string); // verifies the existance of a file ID in the registry
		std::string get_server_passcode(); // gets the server password

		void append_file_map(std::string); // adds an entry to the file map
		void append_key_map(std::string); // adds an entry to the key map

		std::vector<std::vector<std::string>> get_file_map(); // gets the contents of the file map
		std::vector<std::vector<std::string>> get_key_map(); // gets the contents of the key map

	    int get_file_map_size(); // gets the number of entries in the file map
		int get_key_map_size(); // get the number of entries in the key map
		int get_file_size(std::string); // get the size (in bytes) of a file
};


#endif