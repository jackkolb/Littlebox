#include "server_header.h"


int main(int argc, char* argv[]) {

  boost::asio::io_service io_service; // create the io service

  // server infinite loop, try-catch to ensure it keeps running
  while(true) {
    try {
      Connection session; // create new session object

      // ensure file_map and file_keys exist
      if (!std::ifstream(session.FILE_MAP_PATH)) {
        std::cout << "Generating file map" << std::endl;
        session.generate_file_map(); // default file_map
        session.generate_key_map(); // default key_map
      }

      int port = 12698; // port to accept on 
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), port)); // create the acceptor
      tcp::socket _socket(io_service); // create the socket

      acceptor.accept(_socket); // wait for connection (blocking)

      session.log = "[" + std::to_string(std::time(nullptr)) + "]"; // starts session log with [TIME]
      session.log += " IP>" + _socket.remote_endpoint().address().to_string(); // add client's IP address to log

      char input_buffer[1024]; // input buffer for header request (what the client wants to do)
      boost::asio::read(_socket, boost::asio::buffer(input_buffer, 1024)); // read the header request

      session.handle_connection(_socket, input_buffer); // acts upon the header request
      std::cout << session.log << std::endl; // outputs the session log
    }
    catch (std::exception& e) {
      std::cerr << "Outside Exception: " << e.what() << std::endl; // if something goes wrong.. display it!
    }
  }
  return 0;
}

