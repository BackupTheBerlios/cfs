#ifndef CFS_H
#define CFS_H

#include <stdio.h>
#include "nodes.h"
#include "entry.h"
#include "modules.h"

typedef struct cnfnode * cfile;

/*#define c_init() _c_init()
#define c_open(a) _c_open(a, currun)
#define c_close(a) _c_close(a)
#define c_read(a) _c_read(a)
#define c_create(a) _c_create(a, currun)
#define c_write(a,b) _c_write(a,b)
#define c_mkdir(a) _c_mkdir(a, currun)
#define c_errno _c_errno
#define c_rm(a) _c_rm(a, currun)
#define c_rename(a,b) _c_rename(a,b,currun)
#define c_chdir(a) _c_chdir(a, currun)
#define c_create_comment(a) _c_create_comment(a,currun)

#define cc_init() _c_init()
#define cc_open(a) _cc_open(a, currun)
#define cc_close(a) _cc_close(a)
#define cc_read(a) _cc_read(a)
#define cc_create(a) _cc_create(a, currun)
#define cc_create_comment(a) _cc_create_comment(a, currun)
#define cc_write(a,b) _cc_write(a,b)
#define cc_mkdir(a) _cc_mkdir(a, currun)
#define cc_errno _c_errno
#define cc_rm(a) _cc_rm(a, currun)
#define cc_rename(a,b) _cc_rename(a,b,currun)
#define cc_chdir(a) _cc_chdir(a, currun)
*/
#endif /* CFS_H */
