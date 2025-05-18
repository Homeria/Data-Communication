#ifndef TRANSPORT_LAYER_HPP
#define TRANSPORT_LAYER_HPP

class TransportLayer {
    public:
        void init_sender(const std::string& ip, int port);
        void send_message(const std::string& msg);
};

#endif