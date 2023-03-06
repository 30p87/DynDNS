//
// Created by 30p87 on 2/26/23.
//

#ifndef DYNDNS_DOMAIN_H
#define DYNDNS_DOMAIN_H

#include "services/services.h"
#include <string>
#include <fmt/core.h>
#include <map>
#include <utility>
#include <netdb.h>

class domain {
public:
    explicit domain(std::string domainName, std::string password, std::string subdomain = "",
                    updatePointer updater = nullptr,
                    std::string ip = "");

    bool update();

private:
    std::string domainName;
    std::string subdomain;
    std::string password;
    updatePointer updater;
    std::string ip;

    bool needsUpdate();

    std::string getName();
};


#endif //DYNDNS_DOMAIN_H
