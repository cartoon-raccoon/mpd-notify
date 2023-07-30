#ifndef MPD_NOTIFY_H
#define MPD_NOTIFY_H

extern "C" {
#include <mpd/client.h>
}

#include <libnotify/notification.h>
#include <libnotify/notify.h>

#include <cstdlib>

namespace mpdnotify {

class Song {
    private:

    mpd_song *song {nullptr};
    
    friend class MpdNotify;

    Song(mpd_song *song);
    ~Song();

    public:

};

class MpdNotify {
    private:

    mpd_connection *mpd {nullptr};
    mpd_song *previous_song {nullptr};
    NotifyNotification *notif {nullptr};

    friend class Song;

    /// Run the mpd idle command, waiting until
    /// a player event is received
    void run_idle();



    public:
    MpdNotify();
    ~MpdNotify();

    Song get_current_song();
    void construct_notif();
    void send_notif();
    bool run();
};

}

#endif