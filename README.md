# Term Project 2 (Part 2) - Reliable UDP File Transfer with RDT Protocol

## 📚 Overview

This project implements a reliable file transfer protocol over UDP sockets in C++.  
The sender splits a file into fixed-size chunks and transmits them to the receiver using UDP.  
A Stop-and-Wait RDT (Reliable Data Transfer) protocol is implemented to handle packet loss, duplication, retransmission, timeouts, and ACK loss, simulating real network conditions.

---

## 🚦 Features

- **Stop-and-Wait RDT Protocol:** Handles packet loss, duplication, retransmission, and ACK loss.
- **Configurable Loss Probability:** Settable drop probability for both data and ACK packets via command-line arguments.
- **Timeout and Retransmission:** Uses system alarm and signal handling for timeouts.
- **Detailed Logging:** All events (send, receive, drop, timeout, retransmit) are logged with timestamps and packet info.
- **Packet Serialization:** Packet objects are serialized/deserialized for transmission.
- **File Transfer:** Supports transferring files of various sizes (from KB to MB).

---

## 🗂️ Project Structure

```
.
├── include/           # Header files
│   ├── Packet.h
│   ├── Receiver.h
│   ├── Sender.h
│   ├── UDPSocket.h
│   └── Utils.h
├── src/               # Source files
│   ├── Packet.cpp
│   ├── Receiver.cpp
│   ├── Sender.cpp
│   ├── UDPSocket.cpp
│   └── Utils.cpp
├── data_sample/       # Sample data files for testing
│   ├── 1KB.txt ... 2MB.txt
├── log/               # Event and packet logs
│   ├── sender/
│   └── receiver/
├── build/             # Compiled binaries
│   ├── sender
│   └── receiver
├── main_sender.cpp
├── main_receiver.cpp
├── Makefile
└── README.md
```

---

## ⚙️ Build Instructions

### 1. Using Makefile (Recommended)

```bash
make
```
- This will generate `build/sender` and `build/receiver` binaries.

### 2. Manual Compilation

```bash
g++ main_sender.cpp src/Sender.cpp src/Packet.cpp src/UDPSocket.cpp src/Utils.cpp -Iinclude -o build/sender
g++ main_receiver.cpp src/Receiver.cpp src/Packet.cpp src/UDPSocket.cpp src/Utils.cpp -Iinclude -o build/receiver
```

---

## 🚀 How to Run

### Receiver

```bash
./build/receiver <port> <data_drop_prob>
# Example:
./build/receiver 5000 0.05
```
- `<port>`: Port number to bind the receiver.
- `<data_drop_prob>`: Probability to drop incoming data packets (0.0 ~ 1.0).

### Sender

```bash
./build/sender <sender_port> <receiver_ip> <receiver_port> <timeout> <file_name> <ack_drop_prob>
# Example:
./build/sender 4000 127.0.0.1 5000 2 data_sample/2MB.txt 0.05
```
- `<sender_port>`: Port number for the sender socket.
- `<receiver_ip>`: IP address of the receiver.
- `<receiver_port>`: Port number of the receiver.
- `<timeout>`: Timeout interval (seconds).
- `<file_name>`: Path to the file to send.
- `<ack_drop_prob>`: Probability to drop received ACK packets (0.0 ~ 1.0).

---

## 📝 Packet Structure

- **type**: DATA (0), ACK (1), EOT (2)
- **seqNum**: Sequence number (starts from 0)
- **ackNum**: ACK number
- **length**: Data length (0~1000)
- **data**: Actual data (up to 1000 bytes)

---

## 🧪 Sample Data Generation

```bash
head -c 2048 < /dev/urandom | tr -dc '0-9' | head -c 2048 > data_sample/2KB.txt
head -c 3072 < /dev/urandom | tr -dc '0-9' | head -c 3072 > data_sample/3KB.txt
head -c 5120 < /dev/urandom | tr -dc '0-9' | head -c 5120 > data_sample/5KB.txt
head -c 7168 < /dev/urandom | tr -dc '0-9' | head -c 7168 > data_sample/7KB.txt
head -c 2097152 < /dev/urandom | tr -dc '0-9' | head -c 2097152 > data_sample/2MB.txt
```

---

## 📄 Log Files

- `log/sender/event_log.txt`, `log/receiver/event_log.txt`: Event logs with timestamps, seqNum/ackNum, and event type.
- `log/sender/packet_log.txt`, `log/receiver/packet_log.txt`: Detailed packet information.

---

## ⚠️ Notes

- The Stop-and-Wait protocol only allows one packet in flight at a time.
- All events (send, receive, drop, timeout, retransmit) are logged for debugging and analysis.
- Test with various file sizes, drop probabilities, and timeout values for robust evaluation.

---

## 👨‍💻 Author

- Name: GyeongHo Cha (CSE-202010687)
- Course: Data Communication
- Instructor: Dohyung Kim

---