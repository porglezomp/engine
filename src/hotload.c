#include "hotload.h"

#include <iso646.h>
#include <stdbool.h>

#include <pthread.h>
#include <CoreServices/CoreServices.h>

static bool started_thread = false;
static pthread_t event_thread;

static hotload_callback *callbacks = NULL;
static size_t callback_count = 0;
static size_t callback_capacity = 0;

static pthread_mutex_t event_queue_mutex;
// @Performance: Some other allocation scheme for performance reasons?
static char **event_queue = NULL;
static size_t event_count = 0;
static size_t event_capacity = 0;


static void
expand_queue_capacity(void)
{
    if (event_queue == NULL) {
        event_queue = calloc(4, sizeof(*event_queue));
        event_capacity = 4;
    } else {
        event_capacity *= 2;
        event_queue = realloc(event_queue, event_capacity * sizeof(*event_queue));
    }
}

static void
event_callback(ConstFSEventStreamRef stream_ref, void *client_callback_info,
               size_t num_events, void *event_paths,
               const FSEventStreamEventFlags event_flags[],
               const FSEventStreamEventId event_ids[])
{
    (void) stream_ref;
    (void) client_callback_info;
    (void) event_ids;

    pthread_mutex_lock(&event_queue_mutex);
    for (size_t evt = 0; evt < num_events; ++evt) {
        if (event_flags[evt] & kFSEventStreamEventFlagItemIsFile) {
            if (event_count >= event_capacity) {
                expand_queue_capacity();
            }
            size_t len = strlen(((char**)event_paths)[evt]) + 1;
            char *string = calloc(1, len);
            strncpy(string, ((char**)event_paths)[evt], len - 1);
            event_queue[event_count++] = string;
        }
    }
    pthread_mutex_unlock(&event_queue_mutex);
}

void
run_hotload_callbacks(void)
{
    pthread_mutex_lock(&event_queue_mutex);
    for (size_t evt = 0; evt < event_count; ++evt) {
        for (size_t i = 0; i < callback_count; ++i) {
            callbacks[i](event_queue[evt]);
        }
        free(event_queue[evt]);
    }
    event_count = 0;
    pthread_mutex_unlock(&event_queue_mutex);
}

static void
expand_callback_capacity(void)
{
    if (callbacks == NULL) {
        callbacks = calloc(1, sizeof(hotload_callback));
        callback_capacity = 1;
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
    if (callback_count >= callback_capacity) {
        expand_callback_capacity();
    }
    callbacks[callback_count++] = callback;

    if (not started_thread) {
        if (pthread_create(&event_thread, NULL, runloop, NULL)) {
            abort();
        }
    }
}

