//
// Created by 30p87 on 2/26/23.
//

#include "domain.h"
#include <curlcpp/curl_easy.h>
#include <curlcpp/curl_header.h>
#include <fmt/core.h>
#include <utility>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <netdb.h>

#define BASE_URL "https://dynamicdns.park-your-domainName.com/update?domain={}&password={}&ip={}"
#define SUB_URL "&host={}"

domain::domain(std::string domainName, std::string password, std::string subdomain, Service service, std::string ip)
        : domainName(std::move(domainName)), password(std::move(password)), subdomain(std::move(subdomain)), service(service),
          ip(std::move(ip)) {
    if (this->ip.empty()) {
        std::string ret = get("https://api4.ipify.org");
        if (ret.empty()) {
            fmt::print(stderr, "Could not get IP!\n");
            exit(-1);
        }
        this->ip = ret;
    }
}

std::string domain::get(const std::string &url) {
    std::ostringstream str;
    curl::curl_ios<std::ostringstream> writer(str);
    curl::curl_easy easy(writer);
    easy.add<CURLOPT_URL>(url.c_str());
    easy.add<CURLOPT_CUSTOMREQUEST>("GET");

    try {
        easy.perform();
    } catch (curl::curl_easy_exception &error) {
        fmt::print(stderr, "{}\n", error.what());
        error.print_traceback();
        return {};
    }

    return str.str();
}

bool domain::forceUpdate() {
    if (service != NAMECHEAP) return false;
    std::string out;
    std::string url;
    url = fmt::format(BASE_URL, domainName, password, ip);
    if (!subdomain.empty()) url += fmt::format(SUB_URL, subdomain);
    out = get(url);

    LIBXML_TEST_VERSION
    xmlDocPtr doc = xmlReadMemory(out.c_str(), (int) out.length(), "noname.html", "UTF-8", 0);
    if (doc == nullptr) {
        fmt::print(stderr, "Failed to parse XML of server response, DNS is probably not updated (PARSING).\n");
        fmt::print(stderr, "Dumping raw XML:\n{}\n", out);
        return false;
    }
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression((xmlChar *) "/interface-response/ErrCount", xpathCtx);
    xmlNodeSetPtr nodes = xpathObj->nodesetval;

    if (nodes->nodeNr != 1) {
        fmt::print(stderr, "Failed to parse XML of server response, DNS maybe updated (XPATH).\n");
        fmt::print(stderr, "Dumping raw XML:\n{}\n", out);
        return false;
    }

    if (!xmlStrEqual(nodes->nodeTab[0]->children->content, (xmlChar *) "0")) {
        xpathObj = xmlXPathEvalExpression((xmlChar *) "/interface-response/errors", xpathCtx);
        nodes = xpathObj->nodesetval;
        if (nodes->nodeNr != 1) {
            fmt::print(stderr,
                       "Something has gone wrong, and failed to parse XML with error information, DNS is probably not updated (PARSING).\n");
            fmt::print(stderr, "Dumping raw XML:\n{}\n", out);
            return false;
        }

        if (xmlChildElementCount(nodes->nodeTab[0]) < 1) {
            fmt::print(stderr,
                       "Something has gone wrong, and failed to parse XML with error information, DNS is probably not updated (PARSING/errors).\n");
            fmt::print(stderr, "Dumping raw XML:\n{}\n", out);
            return false;
        }

        for (xmlNodePtr child = nodes->nodeTab[0]->children; child; child = child->next) {
            if (child->type != XML_ELEMENT_NODE) continue;
            fmt::print("{}\n", (char *) child->children->content);
        }
        return false;
    }
    fmt::print("Domain {} updated to IP {}.\n", getName(), ip);
    return true;
}

bool domain::needsUpdate() {
    char **addresses = gethostbyname(getName().c_str())->h_addr_list;
    char *addr = addresses[0];
    return !(fmt::format("{}.{}.{}.{}", (int)addr[0], (int)addr[1], (int)addr[2], (int)addr[3]) == ip);
}

bool domain::update() {
    return !needsUpdate() || forceUpdate();
}

std::string domain::getName() {
    return fmt::format("{}{}", subdomain.empty() ? "" : fmt::format("{}.", subdomain), domainName);
}