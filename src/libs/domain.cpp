//
// Created by 30p87 on 2/26/23.
//

#include "domain.h"

domain::domain(std::string domainName, std::string password, std::string subdomain, updatePointer updater,
               std::string ip)
        : domainName(std::move(domainName)), password(std::move(password)), subdomain(std::move(subdomain)),
          updater(updater), ip(std::move(ip)) {
    if (updater == nullptr) this->updater = namecheap::update;
    if (this->ip.empty()) {
        std::string ret = get("https://api4.ipify.org");
        if (ret.empty()) {
            fmt::print(stderr, "Could not get IP!\n");
            exit(-1);
        }
        this->ip = ret;
    }
}

bool domain::needsUpdate() {
    hostent *host = gethostbyname(getName().c_str());
    if (host == nullptr) return true;
    char **addresses = gethostbyname(getName().c_str())->h_addr_list;
    if (addresses == nullptr) return true;
    char *addr = addresses[0];
    if (addr == nullptr) return true;
    return fmt::format("{}.{}.{}.{}", (int) addr[0], (int) addr[1], (int) addr[2], (int) addr[3]) != ip;
}

bool domain::update() {
    if (!needsUpdate()) return true;
    if (!updater(domainName, password, subdomain, ip)) return false;
    fmt::print("Domain {} updated to IP {}.\n", getName(), ip);
    return true;
}

std::string domain::getName() {
    return fmt::format("{}{}", subdomain.empty() ? "" : fmt::format("{}.", subdomain), domainName);
}