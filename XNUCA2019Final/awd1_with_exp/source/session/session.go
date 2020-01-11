package session

import (
	"fmt"
	es "note/middleware/session"
	"os"

	"github.com/labstack/echo/v4"
)

func Session() echo.MiddlewareFunc {

	SECRET_KEY := os.Getenv("SECRET_KEY")
	if SECRET_KEY == "" {
		SECRET_KEY = "secret-key"
	}

	fmt.Println(SECRET_KEY)
	store, err := es.NewRedisStore(10, "tcp", "redis:6379", "redis123456aB", []byte(SECRET_KEY))
	if err != nil {
		panic(err)
	}

	return es.New("sid", store)
}
