#include <cstdlib>
#include <libnotify/notify.h>
#include <mpd/client.h>

#include "mpdnotify.h"

using namespace mpdnotify;

int main(int argc, char** argv) {
    MpdNotify app = MpdNotify{};

    app.run();

    return EXIT_SUCCESS;
}