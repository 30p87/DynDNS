//
// Created by 30p87 on 3/6/23.
//

#ifndef DYNDNS_NAMECHEAP_H
#define DYNDNS_NAMECHEAP_H

#include "../network.h"
#include <string>
#include <fmt/core.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#define BASE_URL "https://dynamicdns.park-your-domain.com/update?domain={}&password={}&ip={}"
#define SUB_URL "&host={}"

namespace namecheap {
    bool update(std::string domainName, std::string password, std::string subdomain, std::string ip);
}

#endif //DYNDNS_NAMECHEAP_H
