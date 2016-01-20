package main

import (
	"flag"
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
// receiver method payload

const (
	NXT_PRINTER  = "NXT_1"
	NXT_CONVEYOR = "NXT_2"
	NXT_LOADER   = "NXT_3"
)

var (
	// Registered NXTs
	Devices map[string]Device
	// Status of current job
	Status StatusReply
	// Queue for job requests
	JobQueue chan JobRequest
	// Track job delivery
	JobDone chan int
	// Acknowledge delivery of bluetooth message
	AckChannel chan byte
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

// Send new printing job to the conveyor
func sendPrintingJob(job JobRequest) error {

	i := int([]byte(job.Letter)[0])
	log.Println("Sending job:", i)

	err := sendMessageWithParms(CONVEYOR, JOB_START, i, Devices[NXT_CONVEYOR].n)
	if err != nil {
		fmt.Println("ERROR: Can't send:", err)
		JobDone <- -1
		Status = StatusReply{BLUETOOTH_ERROR, 0, err.Error()}
		return nil
	}
	Status = StatusReply{JOB_SUBMITTED, 0, ""}

	return nil

}

// Wait for job to be delivered (finished production)
func waitForDelivery() {
	status := <-JobDone
	if status == 1 {
		log.Println("Job is finished.")
		Status = StatusReply{JOB_DONE, 0, ""}
	}
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

// Queue for requested jobs
func jobQueue() {
	for j := range JobQueue {
		switch j.Type {
		case "print":
			go sendPrintingJob(j)
			waitForDelivery()
		default:
		}
	}
}

// Validate a job and send it to queue
func processIncomingMessage(receiver, method, payload int) {
	switch {
	case receiver == WEBSERVER:
		if method == JOB_DONE && Status.Method != JOB_DONE {
			JobDone <- 1
		}
		// Just save. Webserver pulls the data periodically.
		if method/METHOD_ERROR == 1 {
			Status = StatusReply{method, payload, fmt.Sprintf("Error ", method)}
		} else if method/METHOD_STATUS == 1 {
			Status = StatusReply{method, payload, ""}
		} else {
			fmt.Println("ERROR: Unknown method for Web Server.")
		}

	case receiver == PRINTER:
		// forward to printer
		Devices[NXT_PRINTER].queue <- Message{receiver, method, payload}

	case receiver == CONVEYOR:
		// forward to conveyor
		Devices[NXT_CONVEYOR].queue <- Message{receiver, method, payload}

	case receiver == LOADER:
		// forward to loader
		Devices[NXT_LOADER].queue <- Message{receiver, method, payload}

	default:
		fmt.Println("ERROR: Invalid receiver in message:", receiver, method, payload)

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
			}
		}(d.n)
	}
}

var (
	nxt1 = flag.String("nxt1", "", "Printer's serial port")
	nxt2 = flag.String("nxt2", "", "Conveyor's serial port")
	nxt3 = flag.String("nxt3", "", "Loader's serial port")
)

func main() {
	flag.Parse()

	Devices = make(map[string]Device)
	if *nxt1 != "" {
		Devices[NXT_PRINTER] = NewDevice("NXT_1", *nxt1)
	}
	if *nxt2 != "" {
		Devices[NXT_CONVEYOR] = NewDevice("NXT_2", *nxt2)
	}
	if *nxt3 != "" {
		Devices[NXT_LOADER] = NewDevice("NXT_3", *nxt3)
	}
	if len(Devices) == 0 {
		flag.Usage()
		fmt.Println("Hint: On Windows, outgoing `Dev B` COM Port should be used.")
		os.Exit(1)
	}

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
	JobDone = make(chan int)
	AckChannel = make(chan byte)
	Status = StatusReply{METHOD_ERROR, 0, "No records of previous jobs."}
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
	fmt.Println("Started HTTP Server.")

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
	}
	//time.Sleep(5 * time.Second)

	os.Exit(1)
}
