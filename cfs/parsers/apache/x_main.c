#include <stdlib.h>
#include "cfs.h"
#include "x.h"

struct cnfmodule x = {
    name: "x",
    aliasses: NULL,
    default_file: "./test.cfg",
    parser: x_parse,
    unparser: x_save
};

void reg(cfile opt_root){
    register_cnfmodule(&x, opt_root);
}