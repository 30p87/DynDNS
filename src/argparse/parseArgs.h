//
// Created by 30p87 on 3/6/23.
//

#ifndef DYNDNS_PARSEARGS_H
#define DYNDNS_PARSEARGS_H

#include "parseConfig.h"
#include <getopt.h>
#include <regex>


#define DNS_VALID_REGEX     R"(^(?!:\/\/)(?:(?!-)[a-zA-Z0-9-]{1,63}\.)+[a-zA-Z]{2,}$)"
#define BASE_DOMAIN_REGEX   R"((?!:\/\/)(?:(?!-)[a-zA-Z0-9-]{1,63})\.[a-zA-Z]{2,}$)"
#define IP_REGEX            R"(^(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$)"

#define HELP_TEXT \
"Usage: {} [OPTION]...\n" \
"Update records on DNS services via DynDNS.\n" \
"Mandatory arguments to long options are mandatory for short options too.\n" \
"   -d, --domainName <str>  the domainName for which the IP should be updated\n" \
"   -p, --password <str>    the password/token to use to access the API\n"           \
"   -s, --updater [<str>]   the updater on which the domainName is registered on. Nothing defaults to namecheap\n"                 \
"   -i, --ip [<str>]        the ip to which it should be updated. Can be left blank for automatic detection\n"                 \
"   -c, --config <str>      the config file to use. Defaults to $XDG_CONFIG_DIR/dyndns.json if no other parameters are provided\n" \
"   -l, --loop [<int>]      loops and checks every n-th second, and updates if required\n"                  \
"   -q, --quiet             no info output. Used for the systemd updater\n"                  \
"   -h, --help              display this help and exit\n"                            \
"\n"               \
"You can specify multiple domains per command. They are processed in order (left to right).\n"         \
"Constant parameters, eg. password and ip, will carry over to the next domainName.\n"                      \
"Example:\n"       \
"{} -s namecheap -p example_password -i 8.8.8.8 -d test.de play.xyz.de -i 1.1.1.1 -d abc.com\n"                        \
"This will forceUpdate test.de and play.xyz.de to be 8.8.8.8 using the password \"example_password\" on namecheap, and also abc.com on namecheap using \"example_password\", but to the IP 1.1.1.1.\n" \
"Note that the parameter password needs to be specified before any domainName.\n"        \
"Example:\n"       \
"{} -d test.com -s namecheap -p password\n"    \
"This will exit with -1 due to the missing parameter \"password\".\n" \
"\n" \
"Available updater options are: namecheap\n"


static struct option longOptions[]{
        {"domainName", required_argument, nullptr, 0},
        {"password",   required_argument, nullptr, 0},
        {"updater",    optional_argument, nullptr, 0},
        {"ip",         optional_argument, nullptr, 0},
        {"config",     required_argument, nullptr, 0},
        {"loop",       optional_argument, nullptr, 0},
        {"quiet",      no_argument,       nullptr, 0},
        {"help",       no_argument,       nullptr, 0},
        {nullptr, 0,                      nullptr, 0}
};

static const char *optString = "d:p:s::i::c:l::qh";

arguments parseArgs(int argc, char *argv[]);

#endif //DYNDNS_PARSEARGS_H
