//
// Created by 30p87 on 3/6/23.
//

#include "parseConfig.h"

std::string getConfigPath() {
    std::string confDir = "~";
    if (std::getenv("XDG_CONFIG_DIR") != nullptr)
        confDir = std::getenv("XDG_CONFIG_DIR");
    else if (std::getenv("HOME") != nullptr)
        confDir = fmt::format("{}/.config", std::getenv("HOME"));
    else
        fmt::print(stderr, "Warning: Neither XDG_CONFIG_DIR nor HOME specified, config dir is current dir.");

    return confDir + CONFIG_FILE_NAME;
}

bool checkConfig(const Json::Value &root) {
    if (root.type() != Json::objectValue ||
        root["domains"].type() != Json::arrayValue || !(
            root["loop"].type() == Json::intValue ||
            root["loop"].type() == Json::nullValue
    ) || !(
            root["quiet"].type() == Json::booleanValue ||
            root["quiet"].type() == Json::nullValue))
        return false;
    for (const Json::Value &domain: root["domains"]) {
        return !(domain.type() != Json::objectValue ||
                 domain["domain"].empty() ||
                 domain["password"].empty());
    }
    return true;
}

arguments parseConfig(std::string path, arguments argRet) {
    arguments ret;
    Json::Value root;
    std::ifstream ifs(path);
    JSONCPP_STRING err;
    Json::CharReaderBuilder builder;

    if (!ifs.good())
        return {};

    if (!parseFromStream(builder, ifs, &root, &err)) {
        fmt::print(stderr, "Error parsing config file! (PARSING)\n");
        fmt::print(stderr, "{}\n", err);
        exit(EXIT_FAILURE);
    }

    if (!checkConfig(root)) {
        fmt::print(stderr, "Error parsing config file {}! (VALUES)\n", path);
        exit(EXIT_FAILURE);
    }

    for (const Json::Value &domain: root["domains"]) {
        ret.domains.emplace_back(domain["domain"].asString(), domain["password"].asString(),
                                 domain["subdomain"].asString(),
                                 updaterMap[domain["service"].asString()], domain["ip"].asString());
    }

    ret.loop = root["loop"].type() == Json::intValue ? root["loop"].asInt() : argRet.loop;
    ret.quiet = root["quiet"].type() == Json::booleanValue ? root["quiet"].asBool() : argRet.quiet;

    return ret;
}