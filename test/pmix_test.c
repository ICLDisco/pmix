/*
 * Copyright (c) 2004-2010 The Trustees of Indiana University and Indiana
 *                         University Research and Technology
 *                         Corporation.  All rights reserved.
 * Copyright (c) 2004-2011 The University of Tennessee and The University
 *                         of Tennessee Research Foundation.  All rights
 *                         reserved.
 * Copyright (c) 2004-2005 High Performance Computing Center Stuttgart,
 *                         University of Stuttgart.  All rights reserved.
 * Copyright (c) 2004-2005 The Regents of the University of California.
 *                         All rights reserved.
 * Copyright (c) 2006-2013 Los Alamos National Security, LLC. 
 *                         All rights reserved.
 * Copyright (c) 2009-2012 Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2011      Oak Ridge National Labs.  All rights reserved.
 * Copyright (c) 2013-2014 Intel, Inc.  All rights reserved.
 * $COPYRIGHT$
 *
 * Additional copyrights may follow
 *
 * $HEADER$
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <event.h>

#include "pmix_server.h"
#include "src/class/pmix_list.h"
#include "src/util/argv.h"
#include "src/util/pmix_environ.h"
#include "src/util/output.h"

#include "test_common.h"

/* setup the PMIx server module */
static int authenticate(char *credential);
static int terminated(const char namespace[], int rank);
static int abort_fn(int status, const char msg[]);
static int fencenb_fn(const pmix_range_t ranges[], size_t nranges,
                      int collect_data,
                      pmix_modex_cbfunc_t cbfunc, void *cbdata);
static int store_modex_fn(pmix_scope_t scope, pmix_modex_data_t *data);
static int get_modexnb_fn(const char namespace[], int rank,
                          pmix_modex_cbfunc_t cbfunc, void *cbdata);
static int get_job_info_fn(const char namespace[], int rank,
                           pmix_info_t *info[], size_t *ninfo);
static int publish_fn(pmix_scope_t scope, const pmix_info_t info[], size_t ninfo);
static int lookup_fn(pmix_scope_t scope,
                     pmix_info_t info[], size_t ninfo,
                     char *namespace[]);
static int unpublish_fn(pmix_scope_t scope, char **keys);
static int spawn_fn(const pmix_app_t apps[],
                    size_t napps,
                    pmix_spawn_cbfunc_t cbfunc, void *cbdata);
static int connect_fn(const pmix_range_t ranges[], size_t nranges,
                      pmix_connect_cbfunc_t cbfunc, void *cbdata);
static int disconnect_fn(const pmix_range_t ranges[], size_t nranges,
                         pmix_connect_cbfunc_t cbfunc, void *cbdata);

static pmix_server_module_t mymodule = {
    authenticate,
    terminated,
    abort_fn,
    fencenb_fn,
    store_modex_fn,
    get_modexnb_fn,
    get_job_info_fn,
    publish_fn,
    lookup_fn,
    unpublish_fn,
    spawn_fn,
    connect_fn,
    disconnect_fn
};

typedef struct {
    pmix_list_item_t super;
    pmix_modex_data_t data;
} pmix_test_data_t;
static OBJ_CLASS_INSTANCE(pmix_test_data_t,
                          pmix_list_item_t,
                          NULL, NULL);

static bool test_complete = false;
static pmix_list_t modex;

static void errhandler(int error)
{
    test_complete = true;
}

int main(int argc, char **argv)
{
    char **client_env=NULL;
    char **client_argv=NULL;
    int rc;
    pid_t pid;

    OBJ_CONSTRUCT(&modex, pmix_list_t);
    
    /* setup the server library */
    if (PMIX_SUCCESS != (rc = PMIx_server_init(&mymodule, NULL, NULL, "1234"))) {
        fprintf(stderr, "Init failed with error %d\n", rc);
        return rc;
    }

    /* register the errhandler */
    PMIx_Register_errhandler(errhandler);
    
    client_env = pmix_argv_copy(environ);
    
    /* fork/exec the test */
    pmix_argv_append_nosize(&client_argv, "pmix_client2");
    if (PMIX_SUCCESS != (rc = PMIx_server_setup_fork(TEST_NAMESPACE, 0, &client_env))) {
        fprintf(stderr, "Server fork setup failed with error %d\n", rc);
        PMIx_server_finalize();
        return rc;
    }
    
    pid = fork();    
    if (pid < 0) {
        fprintf(stderr, "Fork failed\n");
        PMIx_server_finalize();
        return -1;
    }
    
    if (pid == 0) {
        execve("pmix_client2", client_argv, client_env);
        /* Does not return */
    } 

    /* hang around until the client finalizes */
    while (!test_complete) {
        usleep(10000);
    }

    /* deregister the errhandler */
    PMIx_Deregister_errhandler();
    
    /* finalize the server library */
    if (PMIX_SUCCESS != (rc = PMIx_server_finalize())) {
        fprintf(stderr, "Finalize failed with error %d\n", rc);
    }
    PMIX_LIST_DESTRUCT(&modex);
    
    return rc;
}

static int authenticate(char *credential)
{
    if (0 == strcmp(credential, TEST_CREDENTIAL)) {
        return PMIX_SUCCESS;
    }
    return PMIX_ERROR;
}

static int terminated(const char namespace[], int rank)
{
    test_complete = true;
    return PMIX_SUCCESS;
}

static int abort_fn(int status, const char msg[])
{
    return PMIX_SUCCESS;
}

static int fencenb_fn(const pmix_range_t ranges[], size_t nranges, int barrier,
                      pmix_modex_cbfunc_t cbfunc, void *cbdata)
{
    if (NULL != cbfunc) {
        cbfunc(PMIX_SUCCESS, NULL, 0, cbdata);
    }
    return PMIX_SUCCESS;
}

static int store_modex_fn(pmix_scope_t scope, pmix_modex_data_t *data)
{
    pmix_test_data_t *mdx;

    pmix_output(0, "Storing data for %s:%d", data->namespace, data->rank);
    
    mdx = OBJ_NEW(pmix_test_data_t);
    (void)strncpy(mdx->data.namespace, data->namespace, PMIX_MAX_NSLEN);
    mdx->data.rank = data->rank;
    mdx->data.size = data->size;
    if (0 < mdx->data.size) {
        mdx->data.blob = (uint8_t*)malloc(mdx->data.size);
        memcpy(mdx->data.blob, data->blob, mdx->data.size);
    }
    pmix_list_append(&modex, &mdx->super);
    return PMIX_SUCCESS;
}

static int get_modexnb_fn(const char namespace[], int rank,
                          pmix_modex_cbfunc_t cbfunc, void *cbdata)
{
    pmix_test_data_t *mdx;
    pmix_modex_data_t *mdxarray;
    size_t size, n;

    pmix_output(0, "Getting data for %s:%d", namespace, rank);

    size = pmix_list_get_size(&modex);
    if (0 < size) {
        mdxarray = (pmix_modex_data_t*)malloc(size * sizeof(pmix_modex_data_t));
        n = 0;
        PMIX_LIST_FOREACH(mdx, &modex, pmix_test_data_t) {
            (void)strncpy(mdxarray[n].namespace, mdx->data.namespace, PMIX_MAX_NSLEN);
            mdxarray[n].rank = mdx->data.rank;
            mdxarray[n].size = mdx->data.size;
            if (0 < mdx->data.size) {
                mdxarray[n].blob = (uint8_t*)malloc(mdx->data.size);
                memcpy(mdxarray[n].blob, mdx->data.blob, mdx->data.size);
            }
            n++;
        }
    } else {
        mdxarray = NULL;
    }
    if (NULL != cbfunc) {
        cbfunc(PMIX_SUCCESS, mdxarray, size, cbdata);
    }
    /* free the array */
    for (n=0; n < size; n++) {
        if (NULL != mdxarray[n].blob) {
            free(mdxarray[n].blob);
        }
    }
    if (NULL != mdxarray) {
        free(mdxarray);
    }
    return PMIX_SUCCESS;
}

static int get_job_info_fn(const char namespace[], int rank,
                           pmix_info_t *info[], size_t *ninfo)
{
    *info = NULL;
    *ninfo = 0;
    return PMIX_SUCCESS;
}

static int publish_fn(pmix_scope_t scope, const pmix_info_t info[], size_t ninfo)
{
    return PMIX_SUCCESS;
}

static int lookup_fn(pmix_scope_t scope,
                     pmix_info_t info[], size_t ninfo,
                     char *namespace[])
{
    *namespace = NULL;
    return PMIX_SUCCESS;
}

static int unpublish_fn(pmix_scope_t scope, char **keys)
{
    return PMIX_SUCCESS;
}

static int spawn_fn(const pmix_app_t apps[],
                    size_t napps,
                    pmix_spawn_cbfunc_t cbfunc, void *cbdata)
{
   if (NULL != cbfunc) {
        cbfunc(PMIX_SUCCESS, "foobar", cbdata);
    }
    return PMIX_SUCCESS;
}

static int connect_fn(const pmix_range_t ranges[], size_t nranges,
                      pmix_connect_cbfunc_t cbfunc, void *cbdata)
{
    if (NULL != cbfunc) {
        cbfunc(PMIX_SUCCESS, cbdata);
    }
   return PMIX_SUCCESS;
}

static int disconnect_fn(const pmix_range_t ranges[], size_t nranges,
                         pmix_connect_cbfunc_t cbfunc, void *cbdata)
{
    if (NULL != cbfunc) {
        cbfunc(PMIX_SUCCESS, cbdata);
    }
    return PMIX_SUCCESS;
}

