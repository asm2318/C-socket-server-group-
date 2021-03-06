#ifndef SERVERBLOCK_HPP
#define SERVERBLOCK_HPP
#define UPLOAD_DEFAULT_DIR "/goinfre/jnoma/buffers/server1/uploads"
#define BUFFER_DEFAULT_DIR "/goinfre/jnoma/buffers/server1/.buffer"
#include <sys/stat.h>
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include "Location.hpp"
#include "Exception.hpp"
class Server;

enum Status{

    clean,
    waitForServer,
    waitForServerParams,
    waitForLocation,
    waitForLocationParams,
    waitForErrorPageNumber,

    waitForListen,
    waitForServerName,
    waitForServerIndex,
    waitForServerRoot,
    waitForServerMethod,
    waitForServerAutoIndex,
    waitForServerClientMaxBodySize,
    waitForServerTryFiles,
    waitForErrorPage,
    waitForUploadsDirectory,
    waitForBufferDirectory,
    waitForDomainRedirect,

    waitForLocationIndex,
    waitForLocationRoot,
    waitForLocationMethod,
    waitForLocationAutoIndex,
    waitForCgi,
    waitForLocationClientMaxBodySize,
    waitForLocationTryFiles,
    waitForLocationRedirect
};
class ServerBlock
{
public:
    ServerBlock();
    ~ServerBlock();
    friend class Parser;
    friend class Port;
    bool createDirs();
    void fillPorts(Server *server);
    std::string getRoot();
    std::string getBuffer();
    std::string getErrorPage(size_t val);
    std::string getUploadsDir();
    std::string &getIsBufferRoot();
    std::string &getIsUploadsRoot();
    bool redirIsTemp();
    std::string &getRedirect();

private:

    Status status;
    std::set<std::string> server_name;
    std::set<size_t> listen;
    std::set<Method> methods;
    std::map<size_t, std::string> error_page;
    std::string root;
    std::string uploads_directory;
    std::vector<Location> locations;
    std::vector<std::string> try_files;
    size_t client_max_body_size;
    std::vector<std::string>index;
    std::string bufferDir;
    BoolPlusNil autoindex;
    friend bool operator ==(const ServerBlock& lhs, const ServerBlock& rhs);
    friend bool operator !=(const ServerBlock& lhs, const ServerBlock& rhs);
    friend bool operator <(const ServerBlock& lhs, const ServerBlock& rhs);
    friend bool operator <=(const ServerBlock& lhs, const ServerBlock& rhs);
    friend bool operator >(const ServerBlock& lhs, const ServerBlock& rhs);
    friend bool operator >=(const ServerBlock& lhs, const ServerBlock& rhs);
    friend std::ostream& operator<<(std::ostream &os, const ServerBlock& d);
    bool getTry;
    std::string bufferRoot;
    std::string uploadsRoot;
    std::string domainRedirect;
    bool redirectIsTemp;
};

std::ostream& operator<<(std::ostream &os, const ServerBlock& d);
#endif
