#ifndef SERVER_HPP
#define SERVER_HPP
#define BUFFERSIZE 1024*1024 //TODO был слишком медленный
#include <sys/select.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Client.hpp"
#include "Port.hpp"

class Parser;

class Server{
private:
    fd_set read_current;
    fd_set write_current;
    std::vector<Client *> allclients;
    std::vector<Client *> toerase;
    std::vector<FileUpload *> allfiles;
    std::vector<FileUpload *> toeraseF;
    std::vector<Port *> allports;
    struct timeval timeout;
    char buf[BUFFERSIZE];
    Parser *_parser;
    
public:
    Server()
    {
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
    }
    ~Server(){}
    
    void refillSets()
    {
        //std::cout << "SET REFILL\n";
        FD_ZERO(&read_current);
        FD_ZERO(&write_current);
        //std::cout << "Clients size = " << allclients.size() << "\n";
        for (std::vector<Client *>::iterator it = allclients.begin(); it != allclients.end(); ++it)
        {
            if (!(*it)->getStatus())
            {
                FD_SET((*it)->getDescriptor(), &read_current);
                //std::cout << (*it)->getDescriptor() << " (CLIENT) ADDED TO READ SET\n";
            }
            if ((*it)->getStatus() == 6)
            {
                FD_SET((*it)->getPipe(), &read_current);
                //std::cout << (*it)->getPipe() << " (PIPE) ADDED TO READ SET\n";
            }
            if ((*it)->getStatus() == 3)
            {
                if ((*it)->getFile()->getStatus() == 1)
                    (*it)->setStatus(4);
                else if ((*it)->getFile()->getStatus() == -2)
                    (*it)->setStatus(-1);
                //(*it)->resetFile();
            }
            if ((*it)->getStatus() == 2 || (*it)->getStatus() == 4)
            {
                FD_SET((*it)->getDescriptor(), &write_current);
                //std::cout << (*it)->getDescriptor() << " (CLIENT) ADDED TO WRITE SET\n";
            }
        
        }
        
        for (std::vector<FileUpload *>::iterator it = allfiles.begin(); it != allfiles.end(); ++it)
        {
            if (!(*it)->getStatus())
            {
                FD_SET((*it)->getDescriptor(), &write_current);
                //std::cout << (*it)->getDescriptor() << " (FILE) ADDED TO WRITE SET\n";
            }
        }
        
        for (std::vector<Port *>::iterator it = allports.begin(); it != allports.end(); ++it)
            FD_SET((*it)->getDescriptor(), &read_current);
        //std::cout << "SET REFILLED\n";
    }
    
    int getLastSock()
    {
        int last_sock = -1;
        int descr;
        
        for (size_t i = 0; i < allfiles.size(); i++)
        {
            descr = allfiles[i]->getDescriptor();
            if (descr > last_sock)
                last_sock = descr;
        }
        
        for (size_t i = 0; i < allclients.size(); i++)
        {
            descr = allclients[i]->getDescriptor();
            if (descr > last_sock)
                last_sock = descr;
            if (allclients[i]->getStatus() == 6)
            {
                descr = allclients[i]->getPipe();
                if (descr > last_sock)
                    last_sock = descr;
            }
        }
        
        for (size_t i = 0; i < allports.size(); i++)
        {
            descr = allports[i]->getDescriptor();
            if (descr > last_sock)
                last_sock = descr;
        }
        return (last_sock);
    }
    
    int selector()
    {
        return (select(getLastSock() + 1, &read_current, &write_current, NULL, NULL));
    }
    
    void addPort(Port *port)
    {
        allports.push_back(port);
    }
    
    size_t portsCount()
    {
        return (allports.size());
    }
    
    Port *getPort(size_t ind)
    {
        return (allports[ind]);
    }
    
    void addClient(Client *client)
    {
        allclients.push_back(client);
    }
    
    size_t clientsCount()
    {
        return (allclients.size());
    }
    
    Client *getClient(size_t ind)
    {
        return (allclients[ind]);
    }
    
    bool isSetRead(int fd)
    {
        return (FD_ISSET(fd, &read_current));
    }
    
    bool isSetWrite(int fd)
    {
        return (FD_ISSET(fd, &write_current));
    }
    
    void removeClient(Client *client)
    {
        toerase.push_back(client);
    }
    
    void addFile(FileUpload *file)
    {
        allfiles.push_back(file);
    }
    
    void removeFile(FileUpload *file)
    {
        toeraseF.push_back(file);
    }
    
    size_t filesCount()
    {
        return (allfiles.size());
    }
    
    void remove()
    {
        for (size_t i = toeraseF.size(); i > 0; i--)
        {
            std::vector<FileUpload *>::iterator it = std::find(allfiles.begin(), allfiles.end(), toeraseF.back()); //TODO Что вообще тут происходит?
            //std::cout << "File " << (*it)->getDescriptor() << " DELETED\n";
            delete (*it);
            allfiles.erase(it);
            toeraseF.pop_back();
        }
        for (size_t i = toerase.size(); i > 0; i--)
        {
            std::vector<Client *>::iterator it = std::find(allclients.begin(), allclients.end(), toerase.back());
            //std::cout << "Client " << (*it)->getDescriptor() << " DELETED\n";
            delete (*it);
            allclients.erase(it);
            toerase.pop_back();
            //std::cout << "Client DELETION COMPLETE\n";
        }
    }
    
    fd_set &readSet()
    {
        return (read_current);
    }
    
    fd_set &writeSet()
    {
        return (write_current);
    }
    
    void handleConnections()
    {
        Port *curport;
        int descr;
        for (size_t i = 0; i < portsCount(); ++i)
        {
            curport = allports[i];
            descr = curport->getDescriptor();
            //std::cout << "Curport = " << descr << "\n";
            if (isSetRead(descr))
            {
                try{
                Client *client_sock = new Client(curport, timeout);
                //std::cout << "New read connection, fd = " << client_sock->getDescriptor() << "\n";
                addClient(client_sock);
                }catch (Exception &e){
                    std::cout << e.what() << "\n";
                }
            }
        }
    }
    
    void readRequests()
    {
        ssize_t ret;
        int descr;
        Client *curclient;
        for (size_t i = 0; i < clientsCount(); i++)
        {
            curclient = allclients[i];
            descr = curclient->getDescriptor();
            //std::cout << "Check client for read: " << descr << " | status = " << curclient->getStatus() << "\n";
            if (isSetRead(descr) && !curclient->getStatus())
            {
                //std::cout << descr << "-client is readable\n";
                ret = recv(descr, buf, BUFFERSIZE, 0);
                //std::cout << descr << ": Ret = " << ret << "\n";
                if (ret > 0)
                {
                    for (ssize_t k = 0; k < ret; k++)
                    {
                        curclient->fillBuffer(buf[k]);
                        buf[k] = 0;
                    }
                    //std::cout << "Buffer:\n" << readbufs[*it].str() << "\nEnd buffer\n";
                    if (curclient->is_full())
                    {
                        curclient->handleRequest(_parser);
                        if (curclient->getStatus() == 3)
                            addFile(curclient->getFile());
                    }
                }
                if (ret < 0)
                    continue ;
                    //std::cout << "Ret - 1\n";
                if (!ret)
                    removeClient(curclient);
            }
            else if (curclient->getStatus() == 6)
            {
                //std::cout << curclient->getPipe() << "-pipe is readable\n";
                ret = read(curclient->getPipe(), buf, BUFFERSIZE);
                //std::cout << ": Ret = " << ret << "\n";
                if (ret > 0)
                {
                    for (ssize_t k = 0; k < ret; k++)
                    {
                        curclient->fillContent(buf[k]);
                        //std::cout << "#" << buf[i] << "#";
                        buf[k] = 0;
                    }
                }
                if (ret < 0)
                {
                    std::cout << "Pipe read error\n";
                    removeClient(curclient);
                }
                if (!ret)
                {
                    FD_CLR(curclient->getPipe(), &read_current);
                    curclient->finishPipe();
                }
            }
            else if (curclient->getStatus() == -1)
                removeClient(curclient);
        }
        //std::cout << "FInished to read requests\n";
    }
    
    
    void sendAnswer()
    {
        // int ret;
        int descr;
        // int len;
        Client *curclient;
        FileUpload *file;
        for (size_t i = 0; i < filesCount(); i++)
        {
            file = allfiles[i];
            descr = file->getDescriptor();
            //std::cout << "Check file for write: " << descr << "\n";
            if (isSetWrite(descr))
            {
                //std::cout << descr << "-file is ready for writing\n";
                file->fileWrite();
            }
            else if (file->getStatus() == -1)
                removeFile(file);
        }
        
        for (size_t i = 0; i < clientsCount(); i++)
        {
            curclient = allclients[i];
            descr = curclient->getDescriptor();
            //std::cout << "Check client for write: " << descr << " | status = " << curclient->getStatus() << "\n";
            if (isSetWrite(descr))
            {
                //std::cout << descr << "-client is ready for answer\n";
                if (curclient->getStatus() == 4)
                    curclient->formAnswer();
                else
                    curclient->sendResponse();
            }
            else if (curclient->getStatus() == -1)
                removeClient(curclient);
        }
    }
    
    void cleaner()
    {
        int descr;
        Client *curclient;
        for (size_t i = 0; i < clientsCount(); i++)
        {
            curclient = allclients[i];
            descr = curclient->getDescriptor();
            //std::cout << "Check client for clean: " << descr << " | status = " << curclient->getStatus() << "\n";
            curclient->setStatus(-1);
        }
    }
    
    Port *hasPort(int val)
    {
        for (size_t i=0; i < allports.size(); i++)
        {
            if (allports[i]->getPort() == val)
                return (allports[i]);
        }
        return (0);
    }
    
    void setParser(Parser *parser)
    {
        this->_parser = parser;
    }
};

#include "Parser.hpp"
#endif
