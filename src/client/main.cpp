#include "client_header.h"

using namespace std;

enum { max_length = 1024 };


int main(int argc, char* argv[])
{
  try
  {
    boost::asio::io_service io_service; // io service is required for boost::asio
    tcp::resolver resolver(io_service); // resolver is for finding out where the IP address is
    tcp::resolver::query query("127.0.0.1", "12698"); // initialize query to the target IP address
    tcp::socket _socket(io_service); //initialize socket

    char input_buffer[1024]; // initialize input buffer (used for socket reads)
    char output_buffer[1024]; // initialize output buffer (used for socket writes)
    
    std::cout << "Enter command (push, pull, delete): ";
    std::string command; std::cin >> command; // get command from user

    std::string fileID, filename, filepath, filepass, request;
    int file_size = -1; // default file_size, could be any value

    // depending on the command entered, get the other required information
    if (command == "pull") { // "pull" a file from the server -- requires a file ID and the file password
      std::cout << "Enter fileID: ";
      std::cin >> fileID;
      std::cout << "Enter file password: ";
      std::cin >> filepass;
      request = command + " " + fileID + " " + filepass;
    }

    else if (command == "push") { // "push" a file to the server -- requires a file name and a file password
      std::cout << "Enter filename: ";
      std::cin >> filename;
      std::cout << "Enter file password: ";
      std::cin >> filepass;
      filename = filename;
      filepath = "./client/" + filename;
      file_size = get_file_size(filepath.c_str()); // automatically gets the file size from the inputted file name
      request = command + " " + filename + " " + to_string(file_size) + " " + filepass;
    }

    else if (command == "delete") { // "delete" a file from the server -- requires a file ID and the file password
      std::cout << "Enter fileID: ";
      std::cin >> fileID;
      std::cout << "Enter file password: ";
      std::cin >> filepass;
      request = command + " " + fileID + " " + filepass;
    }

    std::cout << "Enter server password: ";
    std::string server_password; std::cin >> server_password; // get the server password
    request += " " + server_password; // append server password ot the request
    strcpy(output_buffer, request.c_str()); // copy the request into the output buffer

    std::cout << "Connecting ... ";
    boost::asio::connect(_socket, resolver.resolve(query)); // connect to socket, blocks
    std::cout << "success!" << std::endl;

    boost::asio::write(_socket, boost::asio::buffer(output_buffer, 1024)); // once connected, write the output buffer to the socket
    // first read validity
    boost::asio::read(_socket, boost::asio::buffer(input_buffer, 1024)); // if it a valid request, server returns ":)"; if not, ":("
    std::string message = input_buffer;

    if (message != ":)") { // if request is not valid...
      std::cout << "[REJECT] - incorrect password(s), command, or file ID" << std::endl;
      return 1;
    }

    // run functions depending on the command
    if (command == "pull") {
      pull_file(_socket, stoi(fileID));
    }
    else if (command == "push") {
      push_file(_socket, filepath);
    }
    else if (command == "delete") { // delete is handled on the server end, so simply read for the response
      boost::asio::read(_socket, boost::asio::buffer(input_buffer, 1024));
      message = input_buffer;
      if (message == ":)") { // ":)" indicates successful delete
        std::cout << "Delete confirmed" << std::endl;
      }
      else {
        std::cout << "Delete failed" << std::endl;
      }
    }

    std::cout << "done!" << std::endl;
  }

  catch (std::exception& e) { // catchall exception handler
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}