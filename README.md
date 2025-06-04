# Term Project 2 (part 1) - UDP File Transfer


## Overview
This project implements a simple file transfer protocol over UDP sockets in C++.
The sender and receiver communicate using predefined text message, and a file is transferred chunk by chunk.


## Message Flow
1. sender -> Receiver - `"Greeting"`, `file name`
2. Receiver -> Sender - `"OK"`
3. Sender -> Receiver - `file data` (chunked), then `"Finish"`
4. Receiver -> Sender - `"WellDone"`

""Note"" This version does not implement packet loss handling or file integrity verification.
In one test, a 1MB `.txt` file became ~98KB after transfer due to UDP packet loss, which is expected behavior for now.

The txt file of 1MB was approximately 98KB after it arrived at the receiver,
which means a packet drop occurred while transferring the txt file.

---

## Environment
Langunage : C++
OS : Ubuntu 24.04.2 LTS
Network : UDP (Datagram Sockets)
Build Tool : `g++`, `make`

---

## Build Instructions

### Option 1 : Compile with g++

```bash
# Compile sender
g++ main_sender.cpp src/Sender.cpp src/UDPSocket.cpp -Iinclude -o sender

# Compile receiver
g++ main_receiver.cpp src/Receiver.cpp src/UDPSocket.cpp -Iinclude -o receiver
```
### Option 2 : Use makefile
Each of sender and receiver has a Makefile.
From the project root, simply run: `make`
This will generate the sender and receiver binaries in the build/ folder.

---

## Run Instructions

### Receiver
``` bash
./receiver <output_file> <port> 
# Example:
./receiver received.txt 5000 
```

### Sender
``` bash
./sender <file_path> <receiver_port>
# Example:
./sender sample.txt 5000
```

---

## File Structure
.
├── include/
│   ├── Receiver.hpp 
│   ├── Sender.hpp 
│   ├── UDPSocket.hpp 
├── src/
│   ├── Receiver.cpp
│   ├── Sender.cpp
│   ├── UDPSocket.cpp
├── main_sender.cpp
├── main_receiver.cpp
├── README.md
└── sample.txt          # Sample file to send

---

## Notes
Receiver buffer size: 4096 bytes
File is transmitted in 1024-byte chunks
No ACK, retransmission, or ordering is implemented.
Tested with .txt files ranging from 100KB to 1MB

---

## Author
Name : GyeongHo Cha (CSE-202010687)
Course : Data Communication
Instructor : Dohyung Kim