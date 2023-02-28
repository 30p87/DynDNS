//
// Created by 30p87 on 2/26/23.
//

#ifndef DYNDNS_DOMAIN_H
#define DYNDNS_DOMAIN_H

#include <string>

typedef enum {
    NAMECHEAP = 0
} Service;

class Domain {
public:
    explicit Domain(std::string domain, std::string password, std::string subdomain = "", Service service = NAMECHEAP,
                    std::string ip = "");

    bool update();

private:
    std::string domain;
    std::string subdomain;
    std::string password;
    Service service;
    std::string ip;

    static std::string get(const std::string &url);

    std::string getName();
};


#endif //DYNDNS_DOMAIN_H
