# Session

Middleware support for echo, utilizing by
[gorilla/sessions](github.com/gorilla/sessions).

## Installation

```shell
go get github.com/hobo-go/echo-mw/session
```

## Usage

```go
package main

import (
    "net/http"

    "github.com/gorilla/context"
    "github.com/labstack/echo"
    "github.com/hobo-go/echo-mw/session"
)

func index(c *echo.Context) error {
    session := session.Default(c)

    var count int
    v := session.Get("count")

    if v == nil {
        count = 0
    } else {
        count = v.(int)
        count += 1
    }

    session.Set("count", count)
    session.Save()

    data := struct {
        Visit int
    }{
        Visit: count,
    }

    return c.JSON(http.StatusOK, data)
}

func main() {
    store := session.NewCookieStore([]byte("secret-key"))
    // store := session.NewFilesystemStore("", []byte("secret-key"))
    // store, err := session.NewRedisStore(32, "tcp", "localhost:6379", "", []byte("secret-key"))
    // if err != nil {
    //     panic(err)
    // }

    e := echo.New()

    // Attach middleware
    e.Use(session.Sessions("ESESSION", store))

    // Routes
    e.Get("/", index)

    // Wrap echo with a context.ClearHandler
    http.ListenAndServe(":8080", context.ClearHandler(e))
}
```
