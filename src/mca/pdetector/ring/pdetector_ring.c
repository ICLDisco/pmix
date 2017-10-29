/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011-2012 Los Alamos National Security, LLC.  All rights
 *                         reserved.
  *
 * Copyright (c) 2017      Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/pmix_config.h>
#include <pmix_common.h>

#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif  /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif  /* HAVE_STRING_H */
#include <stdio.h>
#include <pthread.h>
#include PMIX_EVENT_HEADER

#include "src/util/argv.h"
#include "src/util/error.h"
#include "src/util/output.h"
#include "src/util/show_help.h"
#include "src/include/pmix_globals.h"
#include "src/mca/ptl/ptl.h"

#include "src/mca/pdetector/base/base.h"
#include "pdetector_ring.h"

/* declare the API functions */
static pmix_status_t ring_start();
static pmix_status_t ring_stop();

/* instantiate the module */
pmix_pdetector_base_module_t pmix_pdetector_ring_module = {
    .start = ring_start,
    .stop = ring_stop
};

/*
 * Local functions
 */
static int fd_heartbeat_request(pmix_pdetector_ring_t* ring);
static int fd_heartbeat_send(pmix_pdetector_ring_t* ring);

static int fd_heartbeat_request_cb(struct pmix_peer_t *peer,
                                   pmix_ptl_hdr_t *hdr,
                                   pmix_buffer_t *buf, void *cbdata);

static void fd_event_cb(int fd, short flags, void* pdetector);

void fd_heartbeat_recv_cb(struct pmix_peer_t *peer,
                                pmix_ptl_hdr_t *hdr,
                                pmix_buffer_t *buf, void *cbdata);

static double Wtime();
static double pmix_ring_heartbeat_period = 2e-1;
static double pmix_ring_heartbeat_timeout = 5e-1;
static pmix_event_base_t* fd_event_base = NULL;
static void fd_event_cb(int fd, short flags, void* pring);

/* define a local caddy */
typedef struct {
    pmix_object_t super;
    pmix_event_t ev;
    pmix_peer_t *requestor;
    char *id;
} ring_caddy_t;

typedef struct {
    pmix_object_t super;
    pmix_event_t ev;
    pmix_peer_t *peer;
} pmix_pdetector_beat_t;
PMIX_CLASS_INSTANCE(pmix_pdetector_ring_t,
        pmix_object_t,
        NULL, NULL);
static pmix_status_t ring_start()
{
    pmix_status_t rc;
    pmix_pdetector_ring_t *ft_detector = &pmix_pdetector_world_ring;
    fd_event_base = pmix_globals.evbase;
    PMIX_OUTPUT_VERBOSE((1, pmix_pdetector_base_framework.framework_output,
                         "[%s:%d] starting the detector for heartbeat",
                         pmix_globals.myid.nspace, pmix_globals.myid.rank));

    PMIX_PTL_RECV(rc,pmix_globals.mypeer, fd_heartbeat_recv_cb, PMIX_PTL_TAG_RING_HEARTBEAT);
    PMIX_PTL_RECV(rc,pmix_globals.mypeer, fd_heartbeat_recv_cb, PMIX_PTL_TAG_RING_HEARTBEAT_REQUEST);
    ft_detector = PMIX_NEW(pmix_pdetector_ring_t);

    ft_detector->hb_period = pmix_ring_heartbeat_period;
    ft_detector->hb_timeout = pmix_ring_heartbeat_timeout;
    /* set a timer event to send heartbeat */
    pmix_event_assign(&ft_detector->fd_event, pmix_globals.evbase, -1, PMIX_EV_TIMEOUT | PMIX_EV_PERSIST, fd_event_cb, ft_detector);

    struct timeval tv;
    tv.tv_sec = (int)(ft_detector->hb_period / 10.);
    tv.tv_usec =1;// (-tv.tv_sec + (ft_detector->hb_period / 10.)) * 1e6;
    pmix_event_evtimer_add(&ft_detector->fd_event, &tv);

    return PMIX_SUCCESS;
}

static pmix_status_t ring_stop(pmix_peer_t *requestor, char *id)
{
    return PMIX_SUCCESS;
}

/*
 * event loop and thread
 */
static void fd_event_cb(int fd, short flags, void* pdetector) {
    //PMIX_OUTPUT_VERBOSE((0,pmix_pdetector_base_framework.framework_output,"event callback for HB"));
}

void fd_heartbeat_recv_cb(struct pmix_peer_t *peer,
        pmix_ptl_hdr_t *hdr,
        pmix_buffer_t *buf, void *cbdata)
{
    PMIX_OUTPUT_VERBOSE((0,pmix_pdetector_base_framework.framework_output,"heartbeat received callback"));
}

void pmix_pdetector_ring_recv_beats(struct pmix_peer_t *peer,
        pmix_ptl_hdr_t *hdr,
        pmix_buffer_t *buf, void *cbdata)
{
}
