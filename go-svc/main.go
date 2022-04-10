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
	"net/http"
	"unsafe"

	"github.com/labstack/echo/v4"
)

func main() {
	e := echo.New()
	e.POST("/", func(c echo.Context) error {
		command := c.FormValue("command")
		res := C.external_main(C.CString(command))

		code := C.int(res.code)
		msg := C.GoString(res.msg)
		C.free(unsafe.Pointer(res.msg))

		return c.String(http.StatusOK, fmt.Sprintf("[%v]%v\n", code, msg))
	})
	e.Logger.Fatal(e.Start(":1323"))
}
