#include <stdlib.h>
#include "cfs.h"
#include "ifupdown.h"

struct cnfmodule ifupdown = {
    name: "ifupdown",
    default_file: "/etc/network/interfaces",
    parser: ifupdown_parse,
    unparser: ifupdown_save
};

void reg(cfile opt_root){
    register_cnfmodule(&ifupdown, opt_root);
}
