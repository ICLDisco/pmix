/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2017      Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */

#include <src/include/pmix_config.h>
#include <pmix_common.h>

#include "src/mca/ptl/ptl.h"
#include "src/mca/pdetector/base/base.h"
#include "src/mca/pdetector/ring/pdetector_ring.h"

/*
 * Local functions
 */

static int ring_open(void);
static int ring_close(void);
static int ring_query(pmix_mca_base_module_t **module, int *priority);

pmix_pdetector_ring_component_t mca_pdetector_ring_component = {
    .super = {
        .base = {
            PMIX_PDETECTOR_BASE_VERSION_1_0_0,

            /* Component name and version */
            .pmix_mca_component_name = "ring",
            PMIX_MCA_BASE_MAKE_VERSION(component,
                                       PMIX_MAJOR_VERSION,
                                       PMIX_MINOR_VERSION,
                                       PMIX_RELEASE_VERSION),

            /* Component open and close functions */
            ring_open,  /* component open  */
            ring_close, /* component close */
            ring_query  /* component query */
        }
    }
};


/**
  * component open/close/init function
  */
static int ring_open(void)
{
    pmix_status_t rc;
    PMIX_CONSTRUCT(&mca_pdetector_ring_component.trackers, pmix_list_t);
    /* setup to receive rings */
    PMIX_PTL_RECV(rc, pmix_globals.mypeer, pmix_pdetector_ring_recv_beats, PMIX_PTL_TAG_RING_HEARTBEAT);

    return rc;
}


static int ring_query(pmix_mca_base_module_t **module, int *priority)
{
    *priority = 5;  // irrelevant
    *module = (pmix_mca_base_module_t *)&pmix_pdetector_ring_module;
    return PMIX_SUCCESS;
}

/**
 *  Close all subsystems.
 */

static int ring_close(void)
{
    pmix_status_t rc;
    /* cancel our persistent recv */
    PMIX_PTL_CANCEL(rc,pmix_globals.mypeer, PMIX_PTL_TAG_RING_HEARTBEAT);

    PMIX_LIST_DESTRUCT(&mca_pdetector_ring_component.trackers);

    return rc;
}
