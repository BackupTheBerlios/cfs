#include <stdlib.h>
#include "cfs.h"
#include "phpini.h"

struct cnfmodule phpini = {
    name: "phpini",
    default_file: "./test.cfg",
    parser: phpini_parse,
    unparser: phpini_save
};

void reg(cfile opt_root){
    register_cnfmodule(&phpini, opt_root);
}
