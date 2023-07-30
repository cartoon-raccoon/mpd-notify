extern "C" {
#include <mpd/client.h>
}

#include <libnotify/notification.h>
#include <libnotify/notify.h>

#include <cstdlib>

#include "mpdnotify.h"

namespace mpdnotify {

MpdNotify::MpdNotify() {
    // initialize libnotify
    notify_init("Music");

    // connect to mpd
    this->mpd = mpd_connection_new(NULL, 0, 0); // use library defaults

    // do mpd negotiation here
}

void MpdNotify::run_idle() {
    enum mpd_idle events {};
    
    for (;;){
        events = mpd_run_idle(this->mpd);
        if (events & MPD_IDLE_PLAYER) return;
    }
}

MpdNotify::~MpdNotify() {
    mpd_connection_free(this->mpd);

    notify_uninit();
}

Song MpdNotify::get_current_song() {
    mpd_song *song = mpd_run_current_song(this->mpd);

    return Song {song};
}

void MpdNotify::construct_notif() {
    this->notif = notify_notification_new("", "", "");
}

void MpdNotify::send_notif() {

}

bool MpdNotify::run() {
    this->run_idle();
    return true;
}

}