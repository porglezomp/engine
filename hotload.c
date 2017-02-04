#include "hotload.h"

#include <iso646.h>
#include <stdbool.h>

#include <pthread.h>
#include <CoreServices/CoreServices.h>

static bool started_thread = false;
static pthread_t event_thread;
static pthread_mutex_t callback_mutex;
static hotload_callback *callbacks = NULL;
static size_t callback_count = 0;
static size_t callback_capacity = 0;

static void
event_callback(ConstFSEventStreamRef stream_ref, void *client_callback_info,
               size_t num_events, void *event_paths,
               const FSEventStreamEventFlags event_flags[],
               const FSEventStreamEventId event_ids[])
{
    (void) stream_ref;
    (void) client_callback_info;
    (void) event_ids;

    pthread_mutex_lock(&callback_mutex);
    for (size_t evt = 0; evt < num_events; ++evt) {
        for (size_t i = 0; i < callback_count; ++i) {
            if ( event_flags[evt] & kFSEventStreamEventFlagItemIsFile) {
                callbacks[i](((char**)event_paths)[evt]);
            }
        }
    }
    pthread_mutex_unlock(&callback_mutex);
}

static void
expand_callback_capacity(void)
{
    if (callbacks == NULL) {
        callbacks = calloc(1, sizeof(hotload_callback));
    } else {
        callback_capacity *= 2;
        callbacks = realloc(callbacks, callback_capacity * sizeof(hotload_callback));
    }
}

void*
runloop(void *unused)
{
    (void) unused;
    // @Cleanup: Clean up this when the program shuts down
    //
    // https://developer.apple.com/library/content/documentation/Darwin/Conceptual/
    // FSEvents_ProgGuide/UsingtheFSEventsFramework/UsingtheFSEventsFramework.html#//
    // apple_ref/doc/uid/TP40005289-CH4-SW4
    CFStringRef path = CFSTR("assets");
    CFArrayRef paths_to_watch = CFArrayCreate(NULL, (const void **)&path, 1, NULL);

    FSEventStreamRef stream =
        FSEventStreamCreate(NULL, event_callback, NULL, paths_to_watch,
                            kFSEventStreamEventIdSinceNow, 1,
                            kFSEventStreamCreateFlagFileEvents);
    FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(),
                                     kCFRunLoopDefaultMode);
    FSEventStreamStart(stream);
    CFRunLoopRun();
    return NULL;
}

void
register_hotload_callback(hotload_callback callback)
{
    pthread_mutex_lock(&callback_mutex);
    if (callback_count >= callback_capacity) {
        expand_callback_capacity();
    }
    callbacks[callback_count++] = callback;
    pthread_mutex_unlock(&callback_mutex);

    if (not started_thread) {
        if (pthread_create(&event_thread, NULL, runloop, NULL)) {
            abort();
        }
    }
}

