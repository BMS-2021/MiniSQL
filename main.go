package main

/*
#cgo CFLAGS: -I./
#cgo LDFLAGS: -L${SRCDIR} -lMiniSQL
#include "external_main.h"
*/
import "C"

import (
    "fmt"
)

func main() {
    C.external_main(C.CString("exit;"));
}
