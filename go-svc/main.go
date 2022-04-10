package main

/*
#cgo CFLAGS: -I./
#cgo LDFLAGS: -L${SRCDIR}/../ -lminisql
#include "../external_main/external_main.h"
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func main() {
	res := C.external_main(C.CString("select * from foo;"))

	code := C.int(res.code)
	msg := C.GoString(res.msg)
	C.free(unsafe.Pointer(res.msg))

	fmt.Printf("[%v]%v\n", code, msg)
}
