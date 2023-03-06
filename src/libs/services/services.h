//
// Created by 30p87 on 3/6/23.
//

#ifndef DYNDNS_SERVICES_H
#define DYNDNS_SERVICES_H

#include "namecheap.h"
#include <string>
#include <map>

typedef bool (*updatePointer)(std::string, std::string, std::string, std::string);

static std::map<std::string, updatePointer> updaterMap = {
        {"namecheap", namecheap::update}
};

#endif //DYNDNS_SERVICES_H
