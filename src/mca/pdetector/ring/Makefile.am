#
# Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
#
# Copyright (c) 2017      Intel, Inc.  All rights reserved.
# $COPYRIGHT$
#
# Additional copyrights may follow
#
# $HEADER$
#

#dist_pmixdata_DATA = help-pmix-pdetector-ring.txt

sources = \
        pdetector_ring.c \
        pdetector_ring.h \
        pdetector_ring_component.c

# Make the output library in this directory, and name it either
# mca_<type>_<name>.la (for DSO builds) or libmca_<type>_<name>.la
# (for static builds).

if MCA_BUILD_pmix_pdetector_ring_DSO
component_noinst =
component_install = mca_pdetector_ring.la
else
component_noinst = libmca_pdetector_ring.la
component_install =
endif

mcacomponentdir = $(pmixlibdir)
mcacomponent_LTLIBRARIES = $(component_install)
mca_pdetector_ring_la_SOURCES = $(sources)
mca_pdetector_ring_la_LDFLAGS = -module -avoid-version

noinst_LTLIBRARIES = $(component_noinst)
libmca_pdetector_ring_la_SOURCES =$(sources)
libmca_pdetector_ring_la_LDFLAGS = -module -avoid-version
