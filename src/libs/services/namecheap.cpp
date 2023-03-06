//
// Created by 30p87 on 3/6/23.
//

#include "namecheap.h"

namespace namecheap {
    bool update(std::string domainName, std::string password, std::string subdomain, std::string ip) {
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
        return true;
    }
}