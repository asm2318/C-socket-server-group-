#ifndef SERVER_HPP
#define SERVER_HPP
#define BUFFERSIZE 1024*1024
#define TIMEOUT 120
#include <sys/select.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include "Parser.hpp"
#include "Port.hpp"
class Client;
class FileUpload;
class Parser;
class Port;

class Server{
private:
    fd_set read_current;
    fd_set write_current;
    std::vector<Client *> allclients;
    std::vector<FileUpload *> allfiles;
    std::vector<Port *> allports;
    struct timeval timeout;
    char buf[BUFFERSIZE + 1];
    Parser *_parser;
    std::map<std::string, std::string> contentType;
    
public:
    Server();
    ~Server();
    
    void refillSets();
    int getLastSock();
    int selector();
    void addPort(Port *port);
    size_t portsCount();
    Port *getPort(size_t ind);
    void addClient(Client *client);
    size_t clientsCount();
    Client *getClient(size_t ind);
    void addFile(FileUpload *file);
    size_t filesCount();
    void handleConnections();
    void readRequests();
    void sendAnswer();
    void cleaner();
    Port *hasPort(int val);
    void setParser(Parser *parser);
    bool isSetRead(int fd);
    bool isSetWrite(int fd);
    void clearPorts();
    void fillContentTypes();
};

#endif
