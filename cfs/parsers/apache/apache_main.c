#include <stdlib.h>
#include "cfs.h"
#include "apache.h"

struct cnfmodule apache = {
    name: "apache",
    aliasses: NULL,
    default_file: "./test.cfg",
    parser: apache_parse,
    unparser: apache_save
};

void reg(cfile opt_root){
    register_cnfmodule(&apache, opt_root);
}