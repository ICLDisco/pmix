/*
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2014-2017 Intel, Inc.  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */


#include <src/include/pmix_config.h>
#include <pmix_common.h>

#include "src/util/error.h"

#include "src/mca/pdetector/base/base.h"

pmix_status_t pmix_pdetector_base_start(void)
{
    pmix_pdetector_active_module_t *mod;
    pmix_status_t rc;

    pmix_output_verbose(5, pmix_pdetector_base_framework.framework_output,
                        "%s:%d sensor:base: starting sensors",
                        pmix_globals.myid.nspace, pmix_globals.myid.rank);

    /* call the start function of all modules in priority order */
    PMIX_LIST_FOREACH(mod, &pmix_pdetector_base.actives, pmix_pdetector_active_module_t) {
        if (NULL != mod->module->start) {
            rc = mod->module->start();
            if (PMIX_SUCCESS != rc && PMIX_ERR_TAKE_NEXT_OPTION != rc) {
                return rc;
            }
        }
    }

    return PMIX_SUCCESS;
}

pmix_status_t pmix_pdetector_base_stop(void)
{
    pmix_pdetector_active_module_t *mod;
    pmix_status_t rc;

    pmix_output_verbose(5, pmix_pdetector_base_framework.framework_output,
                        "%s:%d sensor:base: stopping sensors",
                        pmix_globals.myid.nspace, pmix_globals.myid.rank);

    /* call the stop function of all modules in priority order */
    PMIX_LIST_FOREACH(mod, &pmix_pdetector_base.actives, pmix_pdetector_active_module_t) {
        if (NULL != mod->module->stop) {
            rc = mod->module->stop();
            if (PMIX_SUCCESS != rc && PMIX_ERR_TAKE_NEXT_OPTION != rc) {
                return rc;
            }
        }
    }

    return PMIX_SUCCESS;
}
