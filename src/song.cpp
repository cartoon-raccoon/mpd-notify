extern "C" {
#include <mpd/client.h>
}

#include "mpdnotify.h"

namespace mpdnotify {

Song::Song(mpd_song *song) {
    this->song = song;
}

Song::~Song() {
    mpd_song_free(this->song);
}

}