#include "server_header.h"

// test test


// overarching connection handler: calls the push/pull/delete functions
void Connection::handle_connection(tcp::socket &_socket, std::string message) {
	char output_buffer[MAX_PACKET_LENGTH]; // response buffer
	std::stringstream message_stream(message); // stringstream representation of the message (for easier parsing)

	std::string command; // "push", "pull", "delete"
	message_stream >> command; // gets the command from the message stream

	bool message_results = verify_message(message); // filter bad messages, returns ":(" if bad, ":)" if good
	if (message_results == false) { // message is invalid
	  std::strcpy(output_buffer, ":(");
	  boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // write the output buffer to the socket
	  return;
	}
	else { // message is valid
	  std::strcpy(output_buffer, ":)"); // copies ":)" to the output buffer
    log += " [ACCEPT]"; // append to the session log
	  boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // write the output buffer to the socket
	}   

	if (command == "push") {
	  std::string filename; // name of the file to push ("filename.txt")
    int filesize; // size of the file being pushed, in bytes
    std::string filepass; // password to assign to the file
	  message_stream >> filename >> filesize >> filepass; // draws the above from the message stream
	  push_file(_socket, filename, filesize, filepass); // function to push the file
	}

	if (command == "pull") {
	  int fileID; // file ID (also the file name)
	  message_stream >> fileID; // gets the file ID from the message stream
	  pull_file(_socket, fileID); // function to pull the file
	}

	if (command == "delete") {
	  int fileID; // file ID (also the file name)
	  message_stream >> fileID; // gets the file ID from the message stream
    delete_file(_socket, fileID); // function to delete the file
  }
}

// manages a file push (file is transferred from the client to here)
void Connection::push_file(tcp::socket &_socket, std::string filename, int file_size, std::string filepass) {
  // How this works: reads from the socket stream, appending to a temp file, for the entire file size
  // When the file is all read, rename it

  std::remove("./server/temp"); // if a temp file exists, remove it

  int bytes_remaining = file_size; // bytes remaining in the file transfer
  char input_buffer[MAX_PACKET_LENGTH]; // recyclable input buffer (read from client)
  size_t request_length = MAX_PACKET_LENGTH; // length of inbound packets

  FILE* output_file = fopen("./server/temp", "wb"); // temp file to read the file data into, will be renamed if the operation succeeds


  int reply_length = boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // read first buffer sequence 
  std::fwrite(input_buffer, sizeof(char), sizeof(input_buffer), output_file); // write buffer sequence to file
  bytes_remaining -= MAX_PACKET_LENGTH;
  
  try {
    // First, read as many max packet sizes as possible, writing to the temp file
    while (bytes_remaining >= MAX_PACKET_LENGTH) {
      reply_length = boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // reads from the socket stream
      fwrite(input_buffer, sizeof(char), sizeof(input_buffer), output_file); // writes to the file
      bytes_remaining -= MAX_PACKET_LENGTH;
    }
    // if there are remaining bytes, read/write that too
    if (bytes_remaining > 0) {
      char final_buffer[bytes_remaining]; // create new input buffer with the remaining bytes
      reply_length = boost::asio::read(_socket, boost::asio::buffer(final_buffer, bytes_remaining));
      fwrite(final_buffer, sizeof(char), sizeof(final_buffer), output_file);
    }
  }
  catch (std::exception& e) {
      log += " [ERROR IN RECEIVING]"; std::cout << e.what();
      return;
  }

  fclose(output_file); // close the output file
  int fileID = get_file_map_size(); // the fileID is the number of files present + file_map, so it works out

  std::string filepath = "./server/" + std::to_string(fileID); // create the file path
  int result= rename("./server/temp", filepath.c_str()); // rename the temp file
  if (result != 0) {
    log += " [ERROR: RENAME TEMP]";
    remove("./server/temp");
  }

  append_file_map(filename); // adds the file name to the file_map registry
  append_key_map(filepass); // adds the file password to the file_keys registry
  log += " [SUCCESS: PUSH " + std::to_string(fileID) + "]";
}

// manages a file pull (file is transferred from here to the client)
void Connection::pull_file(tcp::socket &_socket, int fileID) {
  // How it works: After some confirmation interchange, reads from the file and writes to the socket stream
  // packet by packet until the entire file is sent
  // Packet order (read: ready from socket; write: write to socket), the checks are used to prevent packet loss:
  //   1. read '*' to signal ready to receive size
  //   2. write file size
  //   3. read '*' to signal ready to start receiving
  //   4. while: read '*' to signal next packet, write next packet

  std::string filepath = "./server/" + std::to_string(fileID); // creates the file path from given fileID
  if (fileID == 0) { // special case: fileID "0" is reserved for the file_map
    filepath = FILE_MAP_PATH;
  }
  int file_size = get_file_size(filepath); // gets the size of the file to be sent

  std::ifstream source_file(filepath, std::ios::binary); // opens the file in binary mode

  std::vector<char> output_buffer(MAX_PACKET_LENGTH, 0); // creates the output buffer
  int bytes_remaining = file_size;

  char input_buffer[MAX_PACKET_LENGTH]; // creates the input buffer
  memset(input_buffer, '\0', MAX_PACKET_LENGTH); // sets the input buffer to null values
  boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // wait for "ready" signal from client

  char size_response[MAX_PACKET_LENGTH]; // buffer for the size response
  memset(size_response, '\0', MAX_PACKET_LENGTH); // wipes the size response buffer // BROKE?
  for (int i = 0; i < std::to_string(file_size).size(); i++) { // writes the file size as a string
    size_response[i] = std::to_string(file_size).at(i);
  }

  boost::asio::write(_socket, boost::asio::buffer(size_response, MAX_PACKET_LENGTH)); // writes the file size response to the socket stream
  boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // wait for "ready" signal, for receiving transmissions

  // First, waits for "ready" signals, reads from the file, and writes to the socket
  while (bytes_remaining >= MAX_PACKET_LENGTH) {
    boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // wait for "ready" signal, for next packet
    source_file.read(output_buffer.data(), MAX_PACKET_LENGTH); // read from file
    boost::asio::write(_socket, boost::asio::buffer(output_buffer.data(), MAX_PACKET_LENGTH)); // write file packet to socket
    output_buffer.clear(); // clear the buffer
    bytes_remaining -= MAX_PACKET_LENGTH;
  }

  // if there are remaining bytes, send that too
  if (bytes_remaining > 0) {
    boost::asio::read(_socket, boost::asio::buffer(input_buffer, MAX_PACKET_LENGTH)); // wait for "ready" signal from the socket
    std::vector<char> new_buffer (bytes_remaining, 0); // create new buffer for the last write
    source_file.read(new_buffer.data(), bytes_remaining); // read from the file
    boost::asio::write(_socket, boost::asio::buffer(new_buffer.data(), bytes_remaining)); // write buffer to the socket
  }

  source_file.close(); // close the input file
  log += " [SUCCESS: PULL ID:" + std::to_string(fileID) + "]";
}

// manages a file delete (removes file from the server directory, updates file_map and file_keys)
void Connection::delete_file(tcp::socket &_socket, int fileID) {
  char output_buffer[MAX_PACKET_LENGTH]; // response for the success (or not) of the deletion
  if (fileID == 0) { // special case: no deleting the file_map!
    log += " [REJECT: ATTEMPT DELETE FILE_MAP]";
    strcpy(output_buffer, ":("); // ":(" indicates bad!
    boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // writes output buffer to socket
    return;
  }
  std::string filename = std::to_string(fileID); // the filename is the fileID in a string form
  std::string filepath = "./server/" + filename; // creates the file path

  if (remove(filepath.c_str()) != 0) { // if failed to delete the file
    log += " [ERROR: DELETE ID:" + std::to_string(fileID) + "]";
    strcpy(output_buffer, ":("); // ":(" indicates bad!
    boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // writes output buffer to socket
    return;
  }
  else {
    log += " [SUCCESS: DELETE ID:" + std::to_string(fileID) + "]";           
    strcpy(output_buffer, ":)"); // ":)" indicates good!
    boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // writes output buffer to socket
  }

  // Updates the file_map and file_keys registries

  // creates a temporary file_map, which will get renamed
  std::ifstream file_map_old = std::ifstream(FILE_MAP_PATH);
  std::ofstream file_map_new = std::ofstream("./server/file_map_temp");

  // creates a temporary file_keys, which will get renamed
  std::ifstream file_keys_old = std::ifstream(FILE_KEYS_PATH);
  std::ofstream file_keys_new = std::ofstream("./server/key_map_temp");

  int file_count = get_file_map_size(); // gets the number of files

  int parse_fileID;
  std::string file_name;
  std::string file_key;

  // goes through the file_map and file_keys, transfers everything but the deleted file,
  // renames files with IDs greater than the deleted file ID
  for (int i = 0; i < file_count; i++) {
    file_map_old >> parse_fileID;
    file_map_old >> file_name;

    file_keys_old >> parse_fileID;
    file_keys_old >> file_key;

    if (i < fileID) { // ID < deleted file ID: keep the same
      file_map_new << std::to_string(i) << " " << file_name <<std::endl;
      file_keys_new << std::to_string(i) << " " << file_key << std::endl;
    }
    else if (i > fileID) { // ID > deleted file ID: subtract 1 (to shift down), rename the files in the directory
      file_map_new << std::to_string(i-1) << " " << file_name << std::endl;
      file_keys_new << std::to_string(i-1) << " " << file_key << std::endl;
      int result = rename(("./server/" + std::to_string(i)).c_str(), ("./server/" + std::to_string(i-1)).c_str()); // renames the file
      if ( result != 0 ) {
        log += " [ERROR: UPDATE FILES: RENAME]";
      }
    }
  }

  file_map_old.close();
  file_map_new.close();

  file_keys_old.close();
  file_keys_new.close();

  if (remove(FILE_MAP_PATH.c_str()) != 0 || remove(FILE_KEYS_PATH.c_str()) != 0) { // removes the old file_map and file_keys files
    log += " [ERROR: SWAPOUT FILE_MAP: REMOVE]";
    strcpy(output_buffer, ":(");
    boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // writes ":(" if it doesn't work
  }
  else {             
    strcpy(output_buffer, ":)");
    boost::asio::write(_socket, boost::asio::buffer(output_buffer, MAX_PACKET_LENGTH)); // writes ":)" if it works
  }

  int result = rename("./server/file_map_temp", FILE_MAP_PATH.c_str()); // renames the "file_map_temp" to "file_map"
  result = rename("./server/key_map_temp", FILE_KEYS_PATH.c_str()); // renames the "file_keys_temp" to "file_keys"
  if ( result != 0 ) {
    log += " [ERROR: SWAPOUT FILE_MAP: RENAME]";
  }
}

// gets the contents of the file_map
std::vector<std::vector<std::string>> Connection::get_file_map() {
  std::vector<std::vector<std::string>> map_info;
  std::ifstream file_map(FILE_MAP_PATH);

  std::string fileID;
  std::string filename;
  std::vector<std::string> entry(2, ""); // [file ID, file name]

  // reads the file map content into the vectors
  while (file_map >> fileID) {
    file_map >> filename;
    entry.clear(); // clears the entry before each pass
    entry.push_back(fileID);
    entry.push_back(filename);
    map_info.push_back(entry);
  }
  file_map.close();

  return map_info;
}

// gets the contents of the file_keys
std::vector<std::vector<std::string>> Connection::get_key_map() {
  std::vector<std::vector<std::string>> key_info;
  std::ifstream file_keys(FILE_KEYS_PATH);

  std::string fileID;
  std::string filepass;
  std::vector<std::string> entry(2, ""); // [file ID, file pass]

  // reads the key map contents into the vectors
  while (file_keys >> fileID) {
    file_keys >> filepass;
    entry.clear(); // clears the entry before each pass
    entry.push_back(fileID);
    entry.push_back(filepass);
    key_info.push_back(entry);
  }
  file_keys.close();

  return key_info;
}

// verifies that the file ID exists in the file map
std::string Connection::verify_id(std::string fileID) {
  std::vector<std::vector<std::string>> map_info = get_file_map();
  for (std::vector<std::string> entry : map_info) {
    if (fileID == entry[0]) {
      return ":)";
    }
  }
  return ":(";
}

// retrieves the file name associated to a given ID
std::string Connection::get_file_name(int fileID) {
  std::vector<std::vector<std::string>> map_info = get_file_map();
  for (std::vector<std::string> entry : map_info) {
    if (std::to_string(fileID) == entry[0]) {
      return entry[1];
    }
  }
  return ":(";
}

// appends a new file name to the file map (also automatically adds the file ID)
void Connection::append_file_map(std::string filename) {
  std::ofstream file_map;
  file_map.open(FILE_MAP_PATH, std::ios_base::app);
  file_map << std::to_string(get_file_map_size()) << " " << filename << std::endl;
  file_map.close();
  return;
}

// appends a new file pass to the key map (also automatically adds the file ID)
void Connection::append_key_map(std::string filepass) {
  std::ofstream file_keys;
  file_keys.open(FILE_KEYS_PATH, std::ios_base::app);
  file_keys << std::to_string(get_key_map_size())<< " " << filepass << std::endl;
  file_keys.close();
  return;
}

// gets the number of files in the file_map registry
int Connection::get_file_map_size() {
  std::ifstream file_map(FILE_MAP_PATH);
  std::vector<std::vector<std::string>> map_info = get_file_map();
  return map_info.size();
}

// gets the number of keys in the key_map registry
int Connection::get_key_map_size() {
  std::ifstream file_keys(FILE_KEYS_PATH);
  std::vector<std::vector<std::string>> keys_info = get_key_map();
  return keys_info.size();
}

// creates a default file map
void Connection::generate_file_map() {
  std::ofstream map(FILE_MAP_PATH);
  map << "0 file_map" << std::endl;
  map.close();
  return;
}

// gets the key of a specified file ID
std::string Connection::get_file_key(int fileID) {
  std::ifstream keys(FILE_KEYS_PATH);
  int ID;
  std::string password;
  while (keys >> ID) {
    keys >> password;
    if (ID == fileID) {
      return password;
    }
  }
  return ":(";
}

// creates a default key map
void Connection::generate_key_map() {
  std::ofstream keys(FILE_KEYS_PATH);
  keys << "0 *" << std::endl;
  keys.close();
  return;
}

// gets the server passcode from the passcode file
std::string Connection::get_server_passcode() {
  std::ifstream passcode_file(PASSCODE_PATH);
  std::string actual_server_password;
  passcode_file >> actual_server_password;
  passcode_file.close();
  return actual_server_password;
}

// scans a message to determine its validity
bool Connection::verify_message(std::string message) {
	std::stringstream input(message); // converts the message into a string stream, for easier parsing
	std::string command;
 	std::string fileID = "-1";
  int file_size;
	input >> command; // depending on the command, the message will be structured differently

	if (command == "push") {
  	std::string filename;
  	input >> filename;
  	input >> file_size;
	}
	else if (command == "pull") {
  	input >> fileID;
	}
	else if (command == "delete") {
  	input >> fileID;
	}
	else { // reject if the command is not "push", "pull", or "delete"
    log += "[REJECT] - bad command";
    return false;
  }

  // confirm file password
  std::string file_password;
  input >> file_password;
  if (command != "push" && file_password != get_file_key(stoi(fileID))) { // reject if the file passwords do not match
    std::cout << ">" << file_password << "_" << get_file_key(stoi(fileID)) << "<" << std::endl;
    log += " [REJECT] - bad file password " + file_password;
    return false;
  }

  // confirm server password
  std::string server_password;
  input >> server_password;
  std::string actual_server_password = get_server_passcode(); // gets the server passcode
	if (server_password != actual_server_password) { // reject if the server passwords do not match
  	log += " [REJECT] - bad server passcode";
	  return false;
	}

	if (fileID != "-1") { // "push" does not require a file ID
  	if (get_file_name(std::stoi(fileID)) == ":(") { // reject if the file ID does not exist in the registries
  	  log += "[REJECT] - bad file ID";
  	  return false;
  	}
	}

  return true; // if nothing gets rejected, great!
}

// gets the file of an inputted file name/path
int Connection::get_file_size(std::string filename) {
  std::ifstream file(filename.c_str(), std::ifstream::binary);
  file.seekg(0, file.end);
  return file.tellg();
}