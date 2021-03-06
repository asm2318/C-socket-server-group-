//
// Created by msnazarow on 14.08.2021.
//

#ifndef C_SOCKET_SERVER_GROUP__LOCATION_HPP
#define C_SOCKET_SERVER_GROUP__LOCATION_HPP
#include <vector>
#include <string>
#include <set>

enum Method{
    GET,
    POST,
    DELETE,
    PUT
};
enum BoolPlusNil{
    False,
    True,
    nil
};
struct Location {
    std::vector<std::string> location;
    std::string root;
    std::vector<std::string> index;
    BoolPlusNil autoindex;
    size_t client_max_body_size;
    std::string fastcgi_pass;
    std::string fastcgi_params;
    std::set<Method> methods;
    std::string cgi_pass;
    std::vector<std::string> try_files;
    std::string redirect;
    bool redirectIsTemp;
    friend std::ostream& operator<<(std::ostream &os, const Location& d);
    Location(): location(), root(), index(), autoindex(nil), client_max_body_size(-1), fastcgi_pass(), fastcgi_params(), redirect(""){}
};
std::ostream& operator<<(std::ostream &os, const Location& d);
std::ostream& operator<<(std::ostream& out, const BoolPlusNil value);
#endif //C_SOCKET_SERVER_GROUP__LOCATION_HPP
