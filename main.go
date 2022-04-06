package main

/*
#cgo CFLAGS: -I./
#cgo LDFLAGS: -L${SRCDIR}/ -lminisql
int external_main(char * str);
*/
import "C"

func main() {
	C.external_main(C.CString("exit;"))
}
