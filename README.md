# Littlebox

A simple, efficient, file server

THIS PROJECT IS NOT YET SECURE! See jackkolb/tinyrsa!

Littlebox aims to be an easy-to-deploy, fast, secure file server that users can run to access files from anywhere in the world.

## Current Features:

- Push files
- Pull files
- Delete files


## Upcoming Features 

- RSA + Symmetric Key Encryption (through jackkolb/tinyrsa)
- Client GUI wrapper (likely in Python)
- Figure out how not to store the file_map and key_map in plain text
- Rename files


## How it Works

Files on the server are stored in the ./server/ directory, listed numerically with two support files: "file_map" and "key_map"
```
./server/
  1
  2
  3
  file_map
  key_map
  passcode
 ```
 
 Currently the support files' contents are stored in plain text, this will likely change in future versions.
 
 The file_map maps file IDs (the numerical file name) to their "actual" name, allowing files with conflicting names on the server. Users are only able to retrieve files in the file_map, the "0" file ID is reserved for file_map. This also prevent clients from pushing/pulling files outside of the ./server/ folder!
 
 The key_map maps file IDs to a keyphrase required to access them, allowing users to password-protect (or not) the files they push. The client cannot access the key_map, and the file_map always has "\*" as its keyphrase. This means having access only to the server allows a user to see the files it manages, but no more.
 
 The passcode file simply stores the server access keyphrase.
 
```
./server/file_map:

0 file_map
1 nicasio_hills.png
2 supersecretdocument.odf
3 nicasio_hills.png


./server/key_map:

0 *
1 californiadreams
2 supersecretserverpass
3 password123
```

The server has three functions:
- Push: copies a file from your computer to the server
- Pull: copies a file from the server to your computer
- Delete: removes a file from the server
(Rename is considered for the future, currently it can be done by Pull + Delete + Push)
