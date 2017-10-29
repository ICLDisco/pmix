/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 *
 * Copyright (c) 2017      Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/**
 * @file
 *
 * Heartbeat sensor
 */
#ifndef PMIX_PDETECTOR_RING_H
#define PMIX_PDETECTOR_RING_H

#include <src/include/pmix_config.h>
#include <src/include/types.h>

#include "src/class/pmix_list.h"
#include "src/include/pmix_globals.h"
#include "src/mca/pdetector/pdetector.h"

BEGIN_C_DECLS

typedef struct {
    pmix_event_t fd_event; /* to trigger timeouts with pmix_events */
    int hb_observing;      /* the server vpid of the process we observe */
    int hb_observer;       /* the server vpid of the process that observes us */
    double hb_rstamp;      /* the date of the last hb reception */
    double hb_timeout;     /* the timeout before we start suspecting observed process as dead (delta) */
    double hb_period;      /* the time spacing between heartbeat emission (eta) */
    double hb_sstamp;      /* the date at which the last hb emission was done */
} pmix_pdetector_ring_t;
PMIX_CLASS_DECLARATION(pmix_pdetector_ring_t);
static pmix_pdetector_ring_t pmix_pdetector_world_ring;


typedef struct {
    pmix_pdetector_base_component_t super;
    pmix_list_t trackers;
} pmix_pdetector_ring_component_t;

PMIX_EXPORT extern pmix_pdetector_ring_component_t mca_pdetector_ring_component;
extern pmix_pdetector_base_module_t pmix_pdetector_ring_module;

void pmix_pdetector_ring_recv_beats(struct pmix_peer_t *peer,
                                       pmix_ptl_hdr_t *hdr,
                                       pmix_buffer_t *buf, void *cbdata);

END_C_DECLS

#endif
