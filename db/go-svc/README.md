# Golang Service Shell

This component is coded in Golang. It starts an HTTP service, and communicate with Zookeeper.  

## Response to master

The response struct is defined as:  

```go
type Response struct {
	Code int    `json:"code"`
	Msg  string `json:"msg"`
}
```

Here is a table, which shows the meaning of each code:  

| Status Code | Meaning                                                      |
| ----------- | ------------------------------------------------------------ |
| -1          | Error occurs when executing SQL statement                    |
| 0           | Success executing SQL statement                              |
| 1           | Successfully exit because of the execution of EXIT statement |

## Report table message to Zookeeper

We connect with Zookeeper via a golang zk client. Then, we create a ephemeral node, and put the table info in it:  

```go
// get args from command line args
regionName := os.Args[1]
serverAddr := os.Args[2]
apiPort := os.Args[3]

// connect to zk client
conn, _, err := zk.Connect([]string{serverAddr}, time.Second)
if err != nil {
    panic(err)
}

// get table names via CGO interface
tableNamesC := C.get_table_names()
tableNames := C.GoString(tableNamesC)
C.free(unsafe.Pointer(tableNamesC))

// register zk ephemeral node
res, err := conn.Create("/db/"+regionName, []byte(tableNames), zk.FlagEphemeral, zk.WorldACL(zk.PermAll))
```
