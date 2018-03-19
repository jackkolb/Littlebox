#ifndef __CLIENT_HEADER_H__
#define __CLIENT_HEADER_H__

#include <cstdlib>
#include <cstring>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>

using boost::asio::ip::tcp;

void push_file(tcp::socket&, std::string); // pushes a file (adds to the server)
void pull_file(tcp::socket&, int); // pulls a file (retrieves from the server)
std::vector<std::vector<std::string>> get_file_map(); // gets the contents of the file map
std::string get_file_name(int); // gets the associated name of a file ID
int get_file_size(const char*); // gets the size (in bytes) of a file

#endif