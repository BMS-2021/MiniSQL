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

type Request struct {
	Command string `json:"command"`
}

type Response struct {
	Code int    `json:"code"`
	Msg  string `json:"msg"`
}

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

	fmt.Println(tableNames)

	res, err := conn.Create("/db/"+regionName, []byte(tableNames), zk.FlagEphemeral, zk.WorldACL(zk.PermAll))
	if err != nil {
		panic(err)
	}

	fmt.Println("created node:", res)

	e := echo.New()
	channel := make(chan int, 1)
	e.POST("/", func(c echo.Context) error {
		// mutex
		channel <- 1
		defer func() { <-channel }()

		var request Request
		if err := c.Bind(&request); err != nil {
			return c.JSON(http.StatusBadRequest, Response{
				Code: -1,
				Msg:  err.Error(),
			})
		}

		res := C.external_main(C.CString(request.Command))

		var response Response
		response.Code = int(res.code)
		response.Msg = C.GoString(res.msg)
		C.free(unsafe.Pointer(res.msg))

		return c.JSON(http.StatusOK, response)
	})
	e.Logger.Fatal(e.Start(":" + apiPort))
}
