package main

/*
#cgo CFLAGS: -I./
#cgo LDFLAGS: -L${SRCDIR}/../ -lminisql
char* external_main(char* str);
#include <stdlib.h>
*/
import "C"
import (
	"fmt"
	"unsafe"
)

func main() {
	res := C.external_main(C.CString("select * from foo;"))
	output := C.GoString(res)
	C.free(unsafe.Pointer(res))

	fmt.Println(output)
}
