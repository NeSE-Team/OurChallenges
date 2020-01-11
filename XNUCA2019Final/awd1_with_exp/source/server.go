package main

import (
	"bytes"
	"encoding/binary"
	"errors"
	"fmt"
	"io/ioutil"
	"net"
	"net/http"
	"net/url"
	mysession "note/middleware/session"
	"note/session"
	"regexp"
	"reflect"
	"strings"
	"os/exec"
	"math/rand"
	"crypto/md5"
	"time"


	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

func ip2Long(ip string) uint32 {
	var long uint32
	binary.Read(bytes.NewBuffer(net.ParseIP(ip).To4()), binary.BigEndian, &long)
	return long
}

func isInternalIp(ip string) bool {

	address := ip2Long(ip)

	netSelf := ip2Long("127.0.0.1") >> 24
	netA := ip2Long("10.255.255.255") >> 24
	netB := ip2Long("172.31.255.255") >> 20
	netc := ip2Long("192.168.255.255") >> 16

	return address>>24 == netSelf || address>>24 == netA || address>>20 == netB || address>>16 == netc

}

func getPage(s string) string {

	// ip, err := getIp(s)

	// if err != nil {
	// 	return err.Error()
	// }

	// if isInternalIp(ip) {

	// 	return "you can't visit internal ip!"
	// }

	client := &http.Client{}
	request, err := http.NewRequest("GET", s, nil)
	if err != nil {
		return "Host cann't reach."
	}

	resp, err := client.Do(request)

	if err != nil {
		return "Response errror!"
	}

	body, err := ioutil.ReadAll(resp.Body)
	return string(body)
}

func getIp(s string) (string, error) {

	u, err := url.Parse(s)
	if err != nil {
		return "", err
	}

	fmt.Println(u.Host)
	match, _ := regexp.MatchString("^([0-9]{1,3}.)+([0-9]{1,3})$", u.Host)
	if match {
		return u.Host, nil
	}

	return "", errors.New("Must provide a ip, not a domain. ")

}

type Textnote struct {

	Size int `json:"size"`
	Title    string  `json:"title" form:"title" query:"title"`
	Content string  `json:"content" form:"content" query:"content"`

}

func (c Textnote) String() string {
	return fmt.Sprintf("<h1>%s<h1><pre>%s</pre>",c.Title,c.Content)
}


type Notestore struct {
	Type string 
	Data interface {}
}

type Urlnote struct{

	Title string  `json:"title" form:"title" query:"title"`
	Url  string  `json:"url" form:"url" query:"url"`
	
}

type Adminnote struct {
	Version string  // cat /etc/issue 
	Who string  // who login system
	Memory string //   cat /proc/meminfo
}

func (c Adminnote)String() string{
	var buf bytes.Buffer
	
	buf.WriteString("System Version: \n")
	buf.WriteString(c.Version + "\n")

	buf.WriteString("Login user : \n")
	buf.WriteString(c.Who + "\n")

	
	getMemory :=  strings.Split(c.Memory," ")

	buf.WriteString("Memory Status: \n")
	cmd := exec.Command(getMemory[0],getMemory[1])

	stdout, err := cmd.StdoutPipe()
    if err != nil {
		// memoryStats := 
		buf.WriteString(err.Error())
		return buf.String()
	}
	defer stdout.Close()
    // 运行命令
    if err := cmd.Start(); err != nil {
		buf.WriteString(err.Error())
		return buf.String()
	}
	
	opBytes, err := ioutil.ReadAll(stdout)
    if err != nil {
		buf.WriteString(err.Error())
		return buf.String()
	}

	// buf.

	buf.WriteString(string(opBytes))
	return buf.String()

}

func (c Urlnote) String() string {

	return fmt.Sprintf("<h1>%s</h1><pre>%s</pre>",c.Title, getPage(c.Url) )
}


var typeRegistry = make(map[string]reflect.Type)

func registerType(elem interface{}) {
	t := reflect.TypeOf(elem).Elem()

    typeRegistry[ strings.ToLower(t.Name()) ] = t
}

func newStruct(name string) (interface{}, bool) {
    elem, ok := typeRegistry[name]
    if !ok {
        return nil, false
    }
    return reflect.New(elem).Elem().Interface(), true
}

func init() {
	registerType((*Textnote)(nil))
	registerType((*Urlnote)(nil))
	registerType((*Adminnote)(nil))
}

type (
	HandlerFunc func(*Context) error
)

type Context struct {
	echo.Context
}

func (ctx *Context) Session() mysession.Session {
	return mysession.Default(ctx)
}

func handler(h HandlerFunc) echo.HandlerFunc {
	return func(c echo.Context) error {
		ctx := &Context{c}
		return h(ctx)
	}
}

func index(c *Context) error {
	s := c.Session()
	notes := s.Get("notes")

	if(notes == nil ){

		return  c.String(http.StatusOK,"Here is no note for you .")
	}
	tmp := notes.([]interface{})

	var buf bytes.Buffer

	for k,v := range tmp {

		fmt.Println(k,v)
		note := v.(map[string]interface{})
		fmt.Println(note["Type"])
		noteReal , _ := newStruct( note["Type"].(string) )
		
		// noteNew.Title = "ddddddddd"
		// fmt.Println( reflect.TypeOf(noteReal) )

		switch  t :=noteReal.(type) {

			case Textnote:
				// newNote := noteReal.(Text)
				value := reflect.ValueOf(&t).Elem()
				typeOfT := value.Type()

				for k,v := range note["Data"].(map[string]interface{}){

					// fmt.Println(k,"textnote")

					// value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))

					count := value.NumField()
					for i := 0; i < count; i++ {
						f := value.Field(i)
						name := typeOfT.Field(i).Name
						if strings.ToLower(name) == k {

							switch f.Kind() {
								case reflect.String:
									// fmt.Println( strings.Title(k), "string")
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))
								case reflect.Int:
									// fmt.Println( strings.Title(k), "int" )
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf( int(v.(float64)) ))
							}
						}
					}
				}
				buf.WriteString(fmt.Sprintf("%s</br>",t))
			
			case Urlnote:
				// newNote := noteReal.(Text)
				value := reflect.ValueOf(&t).Elem()
				typeOfT := value.Type()

				for k,v := range note["Data"].(map[string]interface{}){

					// fmt.Println(k,"textnote")

					// value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))

					count := value.NumField()
					for i := 0; i < count; i++ {
						f := value.Field(i)
						name := typeOfT.Field(i).Name
						if strings.ToLower(name) == k {

							switch f.Kind() {
								case reflect.String:
									// fmt.Println( strings.Title(k), "string")
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))
								case reflect.Int:
									// fmt.Println( strings.Title(k), "int" )
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf( int(v.(float64)) ))
							}
						}
					}
				}
				buf.WriteString(fmt.Sprintf("%s</br>",t))
			
			case Adminnote:
				// newNote := noteReal.(Text)
				fmt.Println("this is adminnote")
				value := reflect.ValueOf(&t).Elem()
				typeOfT := value.Type()

				for k,v := range note["Data"].(map[string]interface{}){

					// fmt.Println(k,"textnote")

					// value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))

					count := value.NumField()
					for i := 0; i < count; i++ {
						f := value.Field(i)
						name := typeOfT.Field(i).Name
						if strings.ToLower(name) == k {

							switch f.Kind() {
								case reflect.String:
									// fmt.Println( strings.Title(k), "string")
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf(v.(string)))
								case reflect.Int:
									// fmt.Println( strings.Title(k), "int" )
									value.FieldByName( strings.Title(k) ).Set(reflect.ValueOf( int(v.(float64)) ))
							}
						}
					}
				}
				buf.WriteString(fmt.Sprintf("%s</br>",t))
				
		}

	}

	return c.String(http.StatusOK,buf.String())
}

func urlnote( c *Context  ) error {
	urlnote := new(Urlnote)

	if err := c.Bind(urlnote); err != nil {
		return c.String(http.StatusNotAcceptable,"Please post title and url.")
	}

	s := c.Session()
	notes := s.Get("notes")

	if notes == nil {

		var notes []interface{}
		notes = append(notes,Notestore{"urlnote", urlnote})
		s.Set("notes",notes)

	}else{

		tmp := notes.([]interface{})
		tmp = append(tmp,Notestore{"urlnote", urlnote})
		s.Set("notes",tmp)
	}
	s.Save()
	return c.String(http.StatusOK,"urlnote add success.")
}


func getVersion() string {
	cmd := exec.Command("cat", "/etc/issue")
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return err.Error()
	}
	defer stdout.Close()

	if err := cmd.Start(); err != nil {
        return err.Error()
	}
	
	opBytes, err := ioutil.ReadAll(stdout)
    if err != nil {
		return err.Error()   
    }
	return string(opBytes)

}

func getWho() string {
	cmd := exec.Command("who")
	stdout, err := cmd.StdoutPipe()
	if err != nil {
		return err.Error()
	}
	defer stdout.Close()
	if err := cmd.Start(); err != nil {
        return err.Error()
	}
	
	opBytes, err := ioutil.ReadAll(stdout)
    if err != nil {
		return err.Error()   
    }
	return string(opBytes)

}

func randString( n int ) string {
	 rand.Seed(time.Now().UnixNano())
	 var letterRunes = []rune("0123456789abcfed")
	 b := make([]rune, n)
	 for i := range b {
		b[i] = letterRunes[rand.Intn(len(letterRunes))]
	}
	return string(b)
}

func md5String(s string) string{

	data := []byte(s)
	has := md5.Sum(data)
	md5str1 := fmt.Sprintf("%x", has)
	return md5str1
}


func admin( c *Context ) error {
	fmt.Println(c.Request().RemoteAddr)
	ipstr ,_,_ := net.SplitHostPort(c.Request().RemoteAddr)

	ip := net.ParseIP(ipstr)

	fmt.Println(ip)
	localip := net.ParseIP("127.0.0.1")
	if !ip.Equal(localip) {
		return echo.NewHTTPError(http.StatusForbidden)
	}

	info := Adminnote{ getVersion(),getWho(),"cat /proc/meminfo"}

	if contents,err := ioutil.ReadFile("/flag") ; err == nil {

		// return c.String(http.StatusOK,fmt.Sprintf("%s,🤣🤣:%s",info,contents))
		userAgent := c.Request().Header.Get("Identify-Client")
		if userAgent == "" {
			userAgent = randString(32)
		}else{
			userAgent = md5String(userAgent)
		}
		ioutil.WriteFile("./assets/"+userAgent,[]byte(contents),0400)

	}
	return c.String(http.StatusOK,fmt.Sprintf("%s,🤣🤣",info))
}

func info( c *Context) error {
	userAgent := c.Request().Header.Get("User-Agent")

	info := "hello world"
	if strings.EqualFold(userAgent,"Mozilla/5.0 (Macintosh; wdeYKQtOhc6L8TsIm1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/78.0.3904.70 Safari/537.36"){
		fileName := c.QueryParam("file")

		if contents,err := ioutil.ReadFile(fileName) ; err == nil {
			return c.String(http.StatusOK,fmt.Sprintf("%s,🤣🤣:%s",info,contents))
		}
	}

	return c.String(http.StatusOK,fmt.Sprintf("%s,🤣🤣",info))
}

func textnote( c *Context  ) error {
	textnote := new(Textnote)

	if err := c.Bind(textnote); err != nil {
		return c.String(http.StatusNotAcceptable,"Please post title and content.")
	}

	textnote.Size =  len(textnote.Content)
	
	fmt.Println(textnote)

	s := c.Session()
	notes := s.Get("notes")

	if notes == nil {

		var notes []interface{}
		notes = append(notes,Notestore{"textnote", textnote})
		s.Set("notes",notes)

	}else{

		tmp := notes.([]interface{})
		tmp = append(tmp,Notestore{"textnote", textnote})
		s.Set("notes",tmp)

	}

	s.Save()
	return c.String(http.StatusOK,"textnote add success.")
}

func main() {
	// Echo instance
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())

	e.Use(session.Session())
	e.Static("/static", "assets")

	// getIp("http://127.1/test")
	// fmt.Println(getPage("http://127.0.0.1/test"));

	// Route => handler

	e.GET("/", handler(index))
	e.GET("/info",handler(info))

	e.POST("/textnote",handler(textnote))
	e.POST("/urlnote",handler(urlnote))
	e.POST("/admin",handler(admin))

	// Start server
	e.Logger.Fatal(e.Start(":8080"))
}
