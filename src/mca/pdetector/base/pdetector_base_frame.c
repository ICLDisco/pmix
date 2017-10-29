/* -*- Mode: C; c-basic-offset:4 ; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, Inc. All rights reserved.
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

#include <pthread.h>
#include PMIX_EVENT_HEADER

#include "src/mca/mca.h"
#include "src/mca/base/base.h"
#include "src/class/pmix_list.h"
#include "src/runtime/pmix_progress_threads.h"
#include "src/include/types.h"

#include "src/mca/pdetector/base/base.h"

/*
 * The following file was created by configure.  It contains extern
 * statements and the definition of an array of pointers to each
 * component's public mca_base_component_t struct.
 */

#include "src/mca/pdetector/base/static-components.h"

/*
 * Global variables
 */
pmix_pdetector_base_module_t pmix_pdetector = {
    pmix_pdetector_base_start,
    pmix_pdetector_base_stop
};
pmix_pdetector_base_t pmix_pdetector_base = {{{0}}};

static bool use_separate_thread = false;

static int pmix_pdetector_register(pmix_mca_base_register_flag_t flags)
{
    (void) pmix_mca_base_var_register("pmix", "pdetector", "base", "use_separate_thread",
                                      "Use a separate thread for monitoring local procs",
                                      PMIX_MCA_BASE_VAR_TYPE_BOOL, NULL, 0, 0,
                                      PMIX_INFO_LVL_9,
                                      PMIX_MCA_BASE_VAR_SCOPE_READONLY,
                                      &use_separate_thread);
    return PMIX_SUCCESS;
}


static int pmix_pdetector_base_close(void)
{
    PMIX_LIST_DESTRUCT(&pmix_pdetector_base.actives);

    if (use_separate_thread && NULL != pmix_pdetector_base.evbase) {
        (void)pmix_progress_thread_stop("PDETECTOR");
    }

    /* Close all remaining available components */
    return pmix_mca_base_framework_components_close(&pmix_pdetector_base_framework, NULL);
}

/**
 * Function for finding and opening either all MCA components, or the one
 * that was specifically requested via a MCA parameter.
 */
static int pmix_pdetector_base_open(pmix_mca_base_open_flag_t flags)
{
    /* construct the list of modules */
    PMIX_CONSTRUCT(&pmix_pdetector_base.actives, pmix_list_t);

    if (use_separate_thread) {
        /* create an event base and progress thread for us */
        if (NULL == (pmix_pdetector_base.evbase = pmix_progress_thread_init("PDETECTOR"))) {
            return PMIX_ERROR;
        }

    } else {
        pmix_pdetector_base.evbase = pmix_globals.evbase;
    }

    /* Open up all available components */
    return pmix_mca_base_framework_components_open(&pmix_pdetector_base_framework, flags);
}

PMIX_MCA_BASE_FRAMEWORK_DECLARE(pmix, pdetector, "PMIx Monitoring Sensors",
                                pmix_pdetector_register,
                                pmix_pdetector_base_open, pmix_pdetector_base_close,
                                mca_pdetector_base_static_components, 0);

PMIX_CLASS_INSTANCE(pmix_pdetector_active_module_t,
                    pmix_list_item_t,
                    NULL, NULL);
