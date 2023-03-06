#include "libs/domain.h"
#include "libs/parseArgs.h"
#include <fmt/core.h>
#include <list>
#include <csignal>

arguments args;

void signalHandler(int) {
    args.loop = false;
}

int main(int argc, char *argv[]) {
    signal(SIGTERM, signalHandler);
    args = parseArgs(argc, argv);
    std::list<domain> domains = args.domains;

    if (domains.empty()) {
        fmt::print(stderr, "No domains provided!\n");
        return -1;
    }

    do {
        for (domain domain: domains) {
            if (domain.update()) {
                if (!args.quiet) fmt::print("Domains updated!\n");
            }
        }
        sleep(args.loop);
    } while(args.loop);
    return 0;
}
