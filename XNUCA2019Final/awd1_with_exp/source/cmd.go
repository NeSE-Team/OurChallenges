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
	"os"
	"regexp"
)

func ip2Long(ip string) uint32 {
	var long uint32
	binary.Read(bytes.NewBuffer(net.ParseIP(ip).To4()), binary.BigEndian, &long)
	return long
}

func isInternalIp(ip string) bool {

	address := ip2Long(ip)
	fmt.Println("This is address:", address)

	netSelf := ip2Long("127.0.0.1") >> 24
	netA := ip2Long("10.255.255.255") >> 24
	netB := ip2Long("172.31.255.255") >> 20
	netC := ip2Long("192.168.255.255") >> 16

	fmt.Println(netSelf, netA, netB, netC)
	fmt.Println(address >> 20)

	return address>>24 == netSelf || address>>24 == netA || address>>20 == netB || address>>16 == netC

}

func getPage(s string) string {

	ip, err := getIp(s)

	if err != nil {
		return err.Error()
	}

	fmt.Println(ip)
	if isInternalIp(ip) {
		return "you can't visit internal ip!"
	}

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
	fmt.Println("host", u.Host)

	ipstr, _, _ := net.SplitHostPort(u.Host)

	fmt.Println("this is ipstr:", ipstr)

	match, _ := regexp.MatchString("^([0-9]{1,3}\\.)+([0-9]{1,3})$", ipstr)

	fmt.Println("this is ")

	if match {

		fmt.Println("this is ipstr:", ipstr)
		return ipstr, nil
	}

	return "", errors.New("Must provide a ip, not a domain. ")

}

func main() {
	// client := &http.Client{}
	// host := "172.18.0.2:7777?a=1 HTTP/1.1\r\nX-injected: header\r\nTEST: 123"
	// url := "http://" + host + ":7777/test/?test=a"
	a := os.Getenv("GOPATH")
	if a == "" {
		fmt.Println(a)
	}

	// fmt.Println(net.LookupIP("blog.wonderkun.cc"))
	// request, err := http.NewRequest("GET", url, nil)
	// fmt.Println(getPage(url))

	// if err != nil {
	// 	fmt.Printf("request error\n")
	// }
	// resp, err := client.Do(request)
	// if err != nil {
	// 	fmt.Printf("response error\n")
	// }
	// resp.Body.Close()
}
