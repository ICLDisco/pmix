/*
 * Copyright (c) 2009      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2012      Los Alamos National Security, Inc. All rights reserved.
 * Copyright (c) 2014-2017 Intel, Inc.  All rights reserved.
 *
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 * @file:
 *
 */

#ifndef PMIX_PDETECTOR_H_
#define PMIX_PDETECTOR_H_

#include <src/include/pmix_config.h>

#include "src/class/pmix_list.h"
#include "src/mca/mca.h"
#include "src/include/pmix_globals.h"

BEGIN_C_DECLS

/*
 * Component functions - all MUST be provided!
 */

/*
 * start a detection operation:
 */
typedef pmix_status_t (*pmix_pdetector_base_module_start_fn_t)(void);

/*
 * stop a detection operation:
 */
typedef pmix_status_t (*pmix_pdetector_base_module_stop_fn_t)(void);

/* API module */
/*
 * Ver 1.0
 */
typedef struct pmix_pdetector_base_module_1_0_0_t {
    pmix_pdetector_base_module_start_fn_t      start;
    pmix_pdetector_base_module_stop_fn_t       stop;
} pmix_pdetector_base_module_t;

/*
 * the standard component data structure
 */
typedef struct pmix_pdetector_base_component_1_0_0_t {
    pmix_mca_base_component_t base;
    pmix_mca_base_component_data_t data;
} pmix_pdetector_base_component_t;



/*
 * Macro for use in components that are of type sensor v1.0.0
 */
#define PMIX_PDETECTOR_BASE_VERSION_1_0_0 \
  PMIX_MCA_BASE_VERSION_1_0_0("pdetector", 1, 0, 0)

/* Global structure for accessing sensor functions
 */
PMIX_EXPORT extern pmix_pdetector_base_module_t pmix_pdetector;  /* holds API function pointers */

END_C_DECLS

#endif /* MCA_SENSOR_H */
