package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/signal"
	"time"

	nxt "github.com/farshidtz/go-nxt"
)

// Bluetooth message format
// FFFF FFFF FFFF (decimal: -32767 to +32767)‬
// receiver method payload

type Config struct {
	WebServer  WebServerConf      `json:"webserver"`
	NXTs       map[string]NXTConf `json:"nxts"`
	JobHandler JobHandlerConf     `json:"job_handling"`
}

type WebServerConf struct {
	ID             int    `json:"device_id"`
	StatusEndpoint string `json:"status_endpoint"`
}

type NXTConf struct {
	ID      int    `json:"device_id"`
	Name    string `json:"bluetooth_name"`
	Port    string `json:"bluetooth_port"`
	Program string `json:"program_name"`
}

type JobHandlerConf struct {
	DeviceID     int `json:"nxt_device_id"`
	JobStartCode int `json:"job_code"`
}

var (
	// Configurations
	c Config
	// Registered NXTs
	Devices map[int]Device
	// Acknowledge delivery of bluetooth message
	AckChannel chan byte
	// Stop Listenning
	Shutdown bool
)

// Format of bluetooth message
type Message struct {
	receiver int
	method   int
	payload  int
}

type Device struct {
	n *nxt.NXT
	// Queue for bluetooth messages
	queue chan Message
}

func NewDevice(name, port string) Device {
	return Device{
		n:     nxt.NewNXT(name, port),
		queue: make(chan Message),
	}
}

// Send new printing job to JobHandler device
func sendPrintingJob(job Request) error {
	if len(job.Letter) != 1 {
		return fmt.Errorf("Requested letter must be 1 character, not %v.", len(job.Letter))
	}

	i := int([]byte(job.Letter)[0])
	log.Println("Sending job:", i)

	err := sendMessageWithParms(c.JobHandler.DeviceID, c.JobHandler.JobStartCode, i, Devices[c.JobHandler.DeviceID].n)
	if err != nil {
		fmt.Printf("ERROR: Couldn't send job over bluetooth: %v\n", err.Error())
		return fmt.Errorf("Couldn't send job over bluetooth: %v", err.Error())
	}

	return nil
}

// Validate a job and send it to queue
func processHttpRequest(r Request) error {
	switch r.Type {
	case "print":
		return sendPrintingJob(r)
	case "stop":
		return stopAllPrograms()
	case "start":
		return startAllPrograms()
	default:
		return fmt.Errorf("Request/Job type missing or unsupported: %v", r.Type)
	}
	return nil
}

// Stop programs on all NXTs
func stopAllPrograms() error {
	for _, d := range Devices {
		fmt.Println("Stopping running program on", d.n)
		d.n.StopProgram()
	}
	return nil
}

// Start programs on all NXTs
func startAllPrograms() error {
	for _, nxt := range c.NXTs {
		fmt.Printf("Starting program %v on %v\n", nxt.Program, nxt.Name)
		reply, err := Devices[nxt.ID].n.StartProgramSync(nxt.Program)
		if err != nil {
			fmt.Println("Error starting a program:", err)
		}
		fmt.Printf("StartProgram Reply from %v: %v\n", nxt.Name, reply)
	}
	return nil
}

// Validate a job and send it to queue
func processIncomingMessage(receiver, method, payload int) {
	switch {
	case receiver == c.WebServer.ID:
		log.Printf("Forwarding to Webserver message `%v %v %v`", receiver, method, payload)
		// forward to webserver
		err := postStatus(time.Now(), method, payload)
		if err != nil {
			fmt.Println("ERROR: Unable to post to webserver: ", err.Error())
		}

	default:
		Devices[receiver].queue <- Message{receiver, method, payload}
	}
}

// Forward messages to designated devices.
// Messages are throttle for each receiver to ensure delivery
func BT_Forwarder() {

	for _, d := range Devices {
		fmt.Println("Forwarding to", d.n)
		go func(d Device) {
			for m := range d.queue {
				log.Printf("Forwarding to <%v> message `%v %v %v`", d.n, m.receiver, m.method, m.payload)
				err := sendMessageWithParms(m.receiver, m.method, m.payload, d.n)
				if err != nil {
					fmt.Println("ERROR: Can't send:", err)
				}
				time.Sleep(time.Millisecond * 1000)
			}
		}(d)
	}
}

// Listen to all registered devices
func BT_Listenner() {

	for _, d := range Devices {
		fmt.Println("Listening to", d.n)
		go func(n *nxt.NXT) {
			for {
				//fmt.Println("Waiting for", n)
				p1, p2, p3, err := readMessageWithParms(n)
				if err != nil {
					if err != NotAMessage {
						fmt.Println("ERROR: Can't read:", err.Error())
					}
					continue
				}
				log.Printf("<%v>: `%d %d %d`", n, p1, p2, p3)
				processIncomingMessage(p1, p2, p3)
				//time.Sleep(time.Millisecond * 50)
				if Shutdown {
					return
				}
			}
		}(d.n)
	}
}

var confPath = flag.String("conf", "config.json", "Router configuration file path")

func main() {
	flag.Parse()

	// Load configuration file
	confFile, err := ioutil.ReadFile(*confPath)
	if err != nil {
		fmt.Println("Error loading confing file:", err)
		os.Exit(1)
	}
	err = json.Unmarshal(confFile, &c)
	if err != nil {
		fmt.Println("Error parsing config file:", err)
		os.Exit(1)
	}

	// Import NXTs from conf
	Devices = make(map[int]Device)
	for _, nxt := range c.NXTs {
		Devices[nxt.ID] = NewDevice(nxt.Name, nxt.Port)
	}

	// Connect to devices
	for _, d := range Devices {
		fmt.Println(d.n)
		err := d.n.Connect()
		if err != nil {
			fmt.Println("Could not connect:", err)
			return
		}
		fmt.Println("Connected!")
	}

	// Restart all programs
	stopAllPrograms()
	time.Sleep(1 * time.Second)
	startAllPrograms()

	// Start Bluetooth forwarder
	AckChannel = make(chan byte)
	BT_Listenner()
	BT_Forwarder()

	router := setupRouter()
	// Start HTTP server
	go func() {
		err := http.ListenAndServe(":9090", router)
		if err != nil {
			fmt.Println(err.Error())
			os.Exit(1)
		}
	}()
	fmt.Println("Started HTTP Server.")

	// Ctrl+C handling
	handler := make(chan os.Signal, 1)
	signal.Notify(handler, os.Interrupt, os.Kill)
	<-handler // block the thread
	fmt.Println("^C Shutting down...")
	Shutdown = true

	stopAllPrograms()
	time.Sleep(1 * time.Second)

	// Disconnect all devices
	for _, d := range Devices {
		fmt.Println("Disconnecting", d.n)
		err := d.n.Disconnect()
		if err != nil {
			fmt.Println("Could not disconnect:", err)
			return
		}
	}

	// Apparently, OS will help disconnecting bluetooth is exit code is 1
	os.Exit(1)
}
