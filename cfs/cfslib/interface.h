#ifndef INTERFACE_H
#define INTERFACE_H

#ifdef FUSE_INTERFACE
#include "fuse_interface.h"
#define cfs_main(a,b,c) cfs_fuse_main(a,b,c)
#endif

#endif /* INTERFACE_H */
