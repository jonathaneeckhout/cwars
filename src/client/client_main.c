#include <stdio.h>
#include "common/logging.h"

int main()
{
    logging_init("client", LOG_DEBUG);

    log_info("Starting the client");

    logging_cleanup();

    return 0;
}
