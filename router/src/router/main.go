package main

import (
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"time"

	nxt "github.com/tonyheupel/go-nxt"
)

// Bluetooth message format
// FFFF FFFF FFFF
// ‭65535‬ ‭65535‬ ‭65535‬
// method payload error

const (
	NXT_PRINTER  = "NXT_1"
	NXT_CONVEYOR = "NXT_2"
	NXT_LOADER   = "NXT_3"
)

// Modular method headers
const (
	PRINTER   = 100
	CONVEYOR  = 200
	LOADER    = 300
	WEBSERVER = 900
)

const (
	JOB_START = 201
	JOB_DONE  = 901
)

func sendPrintingJob(job JobRequest) error {

	fmt.Println([]byte(job.Letter))

	i := int([]byte(job.Letter)[0])
	fmt.Println(i)

	err := sendMessageWithParms(JOB_START, i, 0, Devices[NXT_CONVEYOR].n)
	if err != nil {
		fmt.Println("Can't send:", err)
		return nil
	}

	return nil

}

func waitForDelivery() {
	<-Delivered
	log.Println("Job delivered.")
}

// Validate a job and send it to queue
func processJob(j JobRequest) error {
	switch j.Type {
	case "print":
		if len(j.Letter) != 1 {
			return fmt.Errorf("Requested letter must be 1 character, not %v.", len(j.Letter))
		}
	default:
		return fmt.Errorf("Job type missing or unsupported: %v", j.Type)
	}
	// add to job queue
	JobQueue <- j
	return nil
}

func jobQueue() {
	for j := range JobQueue {
		switch j.Type {
		case "print":
			sendPrintingJob(j)
			waitForDelivery()
		default:
		}
	}
}

// Validate a job and send it to queue
func processIncomingMessage(method, payload, errorcode int) {
	switch {
	case method/WEBSERVER == 1: // Method 9xx
		if method == JOB_DONE && Status.Method != JOB_DONE {
			Delivered <- true
		}
		// Just save. Webserver pulls the data periodically.
		Status = Message{method, payload, errorcode}

	case method/PRINTER == 1: // Method 1xx
		// forward to printer
		Devices[NXT_PRINTER].queue <- Message{method, payload, errorcode}

	case method/CONVEYOR == 1: // Method 2xx
		// forward to conveyor
		Devices[NXT_CONVEYOR].queue <- Message{method, payload, errorcode}

	case method/LOADER == 1: // Method 3xx
		// forward to loader
		Devices[NXT_LOADER].queue <- Message{method, payload, errorcode}

	default:
		fmt.Println("ERROR: Invalid method in message:", method, payload, errorcode)

	}
}

// Listen to all registered devices
func BT_Forwarder() {

	for _, d := range Devices {
		fmt.Println("Forwarding to", d.n)
		go func(d Device) {
			for m := range d.queue {
				log.Printf("Forwarding message `%v %v %v` to %v", m.Method, m.Payload, m.Errorcode, d.n)
				err := sendMessageWithParms(m.Method, m.Payload, m.Errorcode, d.n)
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
					fmt.Println(err)
					continue
				}
				log.Printf("%v: `%d %d %d`", n, p1, p2, p3)
				processIncomingMessage(p1, p2, p3)
				//time.Sleep(time.Millisecond * 50)
			}
		}(d.n)
	}
}

var (
	Status    Message
	JobQueue  chan JobRequest
	Delivered chan bool
	Devices   map[string]Device
)

type Message struct {
	Method    int `json:"method"`
	Payload   int `json:"payload"`
	Errorcode int `json:"error"`
}

type Device struct {
	n     *nxt.NXT
	queue chan Message
}

func NewDevice(name, port string) Device {
	return Device{
		n:     nxt.NewNXT(name, port),
		queue: make(chan Message),
	}
}

func main() {
	Devices = make(map[string]Device)
	Devices[NXT_PRINTER] = NewDevice("NXT_1", "COM13")
	Devices[NXT_CONVEYOR] = NewDevice("NXT_2", "COM8")

	for _, d := range Devices {
		fmt.Println(d.n)
		err := d.n.Connect()
		if err != nil {
			fmt.Println("Could not connect:", err)
			return
		}
		fmt.Println("Connected!")
	}

	JobQueue = make(chan JobRequest, 10)
	Delivered = make(chan bool)
	go jobQueue()
	BT_Listenner()
	BT_Forwarder()

	router := setupRouter()
	// start http server
	go func() {
		err := http.ListenAndServe(":9090", router)
		if err != nil {
			fmt.Println(err.Error())
			os.Exit(1)
		}
	}()

	// Ctrl+C handling
	handler := make(chan os.Signal, 1)
	signal.Notify(handler, os.Interrupt, os.Kill)
	<-handler // block the thread
	fmt.Println("^C Shutting down...")

	// Disconnect all devices
	for _, d := range Devices {
		fmt.Println("Disconnecting", d.n)
		err := d.n.Disconnect()
		if err != nil {
			fmt.Println("Could not disconnect:", err)
			return
		}
		time.Sleep(2 * time.Second)
	}
	time.Sleep(5 * time.Second)

	os.Exit(1)
}

func methodStyle(n *nxt.NXT) {
	// Normally use StartProgram but we want to see the name of the running program
	// so we need to wait
	startProgramReply, err := n.StartProgramSync("receiver.rxe")

	if err != nil {
		fmt.Println("Error starting a program:", err)
	}

	fmt.Println("Reply from StartProgram:", startProgramReply)

	runningProgram, err := n.GetCurrentProgramName()

	if err != nil {
		fmt.Println("Error getting current program name:", err)
	} else {
		fmt.Println("Current running program:", runningProgram)
	}

	time.Sleep(3 * time.Second) // Wait 3 seconds before trying to stop

	fmt.Println("Stopping running program...")
	_, err = n.StopProgramSync()

	if err != nil {
		fmt.Println("Error stopping the running program:", err)
	}

	batteryMillivolts, err := n.GetBatteryLevelMillivolts()

	if err != nil {
		fmt.Println("Error getting the battery level:", err)
	} else {
		fmt.Println("Battery level (mv):", batteryMillivolts)
	}
}

func channelStyle(n *nxt.NXT) {
	// All reply messages will be sent to this channel
	reply := make(chan *nxt.ReplyTelegram)

	// Normally would pass in nil for the reply channel and not wait,
	//but we want to see the name of the running program so we need to wait
	n.CommandChannel <- nxt.StartProgram("bt_server.rxe", reply)
	fmt.Println("Reply from StartProgram:", <-reply)

	n.CommandChannel <- nxt.GetCurrentProgramName(reply)
	runningProgramReply := nxt.ParseGetCurrentProgramNameReply(<-reply)
	fmt.Println("Current running program:", runningProgramReply.Filename)

	time.Sleep(3 * time.Second) // Wait 3 seconds before trying to stop

	fmt.Println("Stopping running program...")
	n.CommandChannel <- nxt.StopProgram(reply)

	stopProgramReply := <-reply

	if stopProgramReply.IsSuccess() {
		fmt.Println("Stopped running program successfully!")
	} else {
		fmt.Println("Was unable to stop the program.")
	}

	n.CommandChannel <- nxt.GetBatteryLevel(reply)
	batteryLevelReply := nxt.ParseGetBatteryLevelReply(<-reply)

	if batteryLevelReply.IsSuccess() {
		fmt.Println("Battery level (mv):", batteryLevelReply.BatteryLevelMillivolts)
	} else {
		fmt.Println("Was unable to get the current battery level")
	}
}
