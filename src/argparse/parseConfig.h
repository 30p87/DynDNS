//
// Created by 30p87 on 3/6/23.
//

#ifndef DYNDNS_PARSECONFIG_H
#define DYNDNS_PARSECONFIG_H

#include "../libs/domain.h"
#include <string>
#include <json/json.h>
#include <fmt/core.h>
#include <fstream>
#include <list>

#define CONFIG_FILE_NAME "/DynDNS.json"

struct arguments {
    std::list<domain> domains;
    int loop = false;
    bool quiet = false;
};

std::string getConfigPath();

bool checkConfig(const Json::Value &root);

arguments parseConfig(std::string path, arguments argRet);

#endif //DYNDNS_PARSECONFIG_H
