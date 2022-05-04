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
	"os"
	"time"
	"unsafe"

	"github.com/go-zookeeper/zk"

	"github.com/labstack/echo/v4"
)

func main() {
	regionName := os.Args[1]
	serverAddr := os.Args[2]
	apiPort := os.Args[3]

	conn, _, err := zk.Connect([]string{serverAddr}, time.Second)
	if err != nil {
		panic(err)
	}

	tableNamesC := C.get_table_names()
	tableNames := C.GoString(tableNamesC)
	C.free(unsafe.Pointer(tableNamesC))

	println(tableNames)

	res, err := conn.Create("/db/"+regionName, []byte(tableNames), zk.FlagEphemeral, zk.WorldACL(zk.PermAll))
	if err != nil {
		panic(err)
	}

	println("create node :", res)

	e := echo.New()
	channel := make(chan int, 1)
	e.POST("/", func(c echo.Context) error {
		// mutex
		channel <- 1
		defer func() { <-channel }()

		command := c.FormValue("command")
		res := C.external_main(C.CString(command))

		code := C.int(res.code)
		msg := C.GoString(res.msg)
		C.free(unsafe.Pointer(res.msg))

		return c.String(http.StatusOK, fmt.Sprintf("[%v]%v\n", code, msg))
	})
	e.Logger.Fatal(e.Start(":" + apiPort))
}
