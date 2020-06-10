#include "client_header.h"


std::string FILE_MAP_PATH = "./client/file_map";
int MAX_PACKET_LENGTH = 1024;

// manages a "push", sending a file to the server
void push_file(tcp::socket& _socket, std::string filepath) {
  // How it works: 
  //   1. Get the size of the file
  //   2. while at least packet_length bytes remain: send packet of the file
  //   3. send remaining bytes in a special-fitted packet
  //
  // Example: file of 3700 bytes, packets of size 1000
  //   Packet 1 (1000)-->  Packet 2 (1000)-->  Packet 3 (1000)-->  Packet 4 (700)-->  done!

  std::cout << "Pushing " << filepath << ":" << std::endl;

  std::ifstream source_file(filepath, std::ios::binary); // open the file
  std::vector<char> output_buffer (MAX_PACKET_LENGTH, 0); // initialize the output vector

  int file_size = get_file_size(filepath.c_str()); // gets the size of the file

  int bytes_remaining = file_size;

  // First, using the max packet size, send as many packets as possible (until remaining bytes is less than the max packet size)
  while (bytes_remaining >= MAX_PACKET_LENGTH) {
    source_file.read(output_buffer.data(), MAX_PACKET_LENGTH); // read from the source file
    boost::asio::write(_socket, boost::asio::buffer(output_buffer.data(), MAX_PACKET_LENGTH)); // write packet to the socket
    bytes_remaining -= MAX_PACKET_LENGTH;
    int stars = (int) ( (file_size - bytes_remaining) / float(file_size) * 10 ); // calculates the number of stars to display (for the progress bar)
    std::cout << "Progress: [" << std::string(stars, '*') << std::string(10-stars, ' ') << "]" << "\r"; // fancy progress bar
  }

  // Send the last packet in a special buffer
  if (bytes_remaining > 0) {
    std::vector<char> new_buffer (bytes_remaining, 0); // create special buffer
    source_file.read(new_buffer.data(), new_buffer.size()); // read from the source file
    boost::asio::write(_socket, boost::asio::buffer(new_buffer.data(), MAX_PACKET_LENGTH)); // write packet to the socket
    bytes_remaining = 0;
    int stars = (int) ( (file_size - bytes_remaining) / float(file_size) * 10 ); // calculates the number of stars to display (for the progress bar)
    std::cout << "Progress: [" << std::string(stars, '*') << std::string(10-stars, ' ') << "]" << "\r"; // fancy progress bar
  }    
  std::cout << std::endl; // endl for the progress bar
  source_file.close(); // close the source file
}

// manages a "pull", copying a file from the server
void pull_file(tcp::socket& _socket, int fileID) {
  // How it works: 
  //   1. Get the size of the file
  //   2. while at least packet_length bytes remain: receive packet of the file
  //   3. receive remaining bytes in a special-fitted packet
  //
  // Example: file of 3700 bytes, packets of size 1000
  //   -->Packet 1 (1000)  -->Packet 2 (1000)  -->Packet 3 (1000)  -->Packet 4 (700)  done!
  std::string file_path = "./client/" + get_file_name(fileID); // gets the file name from the ID, turns into a path
  std::cout << "Pulling: " << file_path << ":" << std::endl;
  remove(file_path.c_str()); // deletes the file if it exists
  
  char input_buffer[MAX_PACKET_LENGTH]; // input buffer for reading from the socket
  char output_buffer[MAX_PACKET_LENGTH]; // output buffer for writing to the socket

  memset(output_buffer, MAX_PACKET_LENGTH, '\0'); // wipe the output buffer
  output_buffer[0] = '*'; // "*" is a flag used to signal "ready"
  boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // signal "ready to get file size"
  boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // read file size
  int file_size = std::stoi(std::string(input_buffer)); // turn the input buffer (file size) into a string (to get rid of null characters), and from there to an integer

  FILE* output_file = fopen(file_path.c_str(), "wb"); // open the destination file
  int bytes_remaining = file_size;
  memset(output_buffer, MAX_PACKET_LENGTH, '\0'); // wipes the output buffer

  output_buffer[0] = '*'; // sets the output buffer to the "ready" flag
  boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // signal "ready to receive"
  int packet_count = 0;
  try {
    while (bytes_remaining >= MAX_PACKET_LENGTH) {
      boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // signal "ready to receive next packet"
      boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // read packet into buffer
      fwrite(input_buffer, sizeof(char), sizeof(input_buffer), output_file); // write packet to destination file
      bytes_remaining -= MAX_PACKET_LENGTH;
      int stars = (int) ( (file_size - bytes_remaining) / float(file_size) * 10 ); // calculates the number of stars to display (for the progress bar)
      std::cout << "Progress: [" << std::string(stars, '*') << std::string(10-stars, ' ') << "]" << "\r"; // fancy progress bar
    }

    if (bytes_remaining > 0) {
      char final_buffer[bytes_remaining];
      boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // signal "ready to receive next packet" (final packet)
      boost::asio::read(_socket, boost::asio::buffer(final_buffer, bytes_remaining)); // ready final packet into buffer
      fwrite(final_buffer, 1, sizeof(final_buffer), output_file); // write the final packet to the destination flie
      bytes_remaining = 0;
      int stars = (int) ( (file_size - bytes_remaining) / float(file_size) * 10 ); // calculates the number of stars to display (for the progress bar)
      std::cout << "Progress: [" << std::string(stars, '*') << std::string(10-stars, ' ') << "]" << "\r"; // fancy progress bar
    }
    fclose(output_file); // close the destination file
  }
  catch (std::exception& e) { // catchall exception
    std::cout << "Error: " << e.what() << std::endl;
  }
  std::cout << std::endl; // endl for the progress bar
}

// gets the file map contents
std::vector<std::vector<std::string>> get_file_map() {
  std::vector<std::vector<std::string>> map_info;
  std::ifstream file_map(FILE_MAP_PATH); // opens the file_map file

  std::string fileID;
  std::string filename;

  std::vector<std::string> entry(4, ""); // initializes empty array for the entry

  while (file_map >> fileID) {
    file_map >> filename;
    entry.clear();
    entry.push_back(fileID);
    entry.push_back(filename);
    map_info.push_back(entry);
  }
  file_map.close();

  return map_info;
}

// gets the file name associated with a specified file ID
std::string get_file_name(int fileID) {
  std::vector<std::vector<std::string>> map_info = get_file_map();

  for (std::vector<std::string> entry : map_info) {
    if (std::to_string(fileID) == entry[0]) {
      return entry[1];
    }
  }
  return ":(";
}

// gets the size of a file
int get_file_size(const char* filename) {
  std::ifstream file(filename, std::ifstream::binary);
  file.seekg(0, file.end);
  return file.tellg();
}