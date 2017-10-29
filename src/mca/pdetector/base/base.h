/*
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2013      Los Alamos National Security, LLC.  All rights reserved.
 *
 * Copyright (c) 2017      Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 */
/** @file:
 */

#ifndef PMIX_PDETECTOR_BASE_H_
#define PMIX_PDETECTOR_BASE_H_

#include <src/include/pmix_config.h>

#include "src/class/pmix_list.h"
#include "src/mca/mca.h"
#include "src/mca/base/pmix_mca_base_framework.h"

#include "src/mca/pdetector/pdetector.h"

BEGIN_C_DECLS

/*
 * MCA Framework
 */
PMIX_EXPORT extern pmix_mca_base_framework_t pmix_pdetector_base_framework;

PMIX_EXPORT int pmix_pdetector_base_select(void);

/* define a struct to hold framework-global values */
typedef struct {
    pmix_list_t actives;
    pmix_event_base_t *evbase;
} pmix_pdetector_base_t;

typedef struct {
    pmix_list_item_t super;
    pmix_pdetector_base_component_t *component;
    pmix_pdetector_base_module_t *module;
    int priority;
} pmix_pdetector_active_module_t;
PMIX_CLASS_DECLARATION(pmix_pdetector_active_module_t);

PMIX_EXPORT extern pmix_pdetector_base_t pmix_pdetector_base;

PMIX_EXPORT pmix_status_t pmix_pdetector_base_start(void);

PMIX_EXPORT pmix_status_t pmix_pdetector_base_stop(void);

END_C_DECLS
#endif
