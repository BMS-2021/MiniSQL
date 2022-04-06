package main

/*
#cgo CFLAGS: -I./
#cgo LDFLAGS: -L${SRCDIR}/CMakeFiles -lMiniSQL
#include "main.h"
*/
import "C"

import (
    "fmt"
)

func main() {
    C.external_main(C.CString("exit;"));
}
