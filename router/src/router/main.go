package main

import (
	"fmt"
	"net/http"
	"os"
	"os/signal"
	"time"

	nxt "github.com/tonyheupel/go-nxt"
)

const (
	NXT_PRINTER  = "NXT_1"
	NXT_CONVEYOR = "NXT_2"
	NXT_LOADER   = "NXT_3"
)

// Methods
const (
	PLATE_LOAD = 10
	PRINT      = 20
)

func sendPrintingJob(job JobRequest) error {

	fmt.Println([]byte(job.Letter))

	i := int([]byte(job.Letter)[0])
	fmt.Println(i)

	err := sendMessageWithParms(PRINT, i, 0, Devices[NXT_PRINTER])
	if err != nil {
		fmt.Println("Can't send:", err)
		return nil
	}

	return nil

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
		default:
		}
	}
}

func receiveStatusUpdates() {
	ticker := time.NewTicker(time.Millisecond * 500)
	for t := range ticker.C {
		fmt.Println("Tick at", t)
	}
}

var (
	Status   StatusReply
	JobQueue chan JobRequest
	Devices  map[string]*nxt.NXT
)

func main() {
	Devices = make(map[string]*nxt.NXT)
	Devices["NXT_1"] = nxt.NewNXT("NXT_1", "COM13")
	//Devices["NXT_2"] = nxt.NewNXT("NXT_2", "COM8")

	// // n := nxt.NewNXT("NXT_1", "COM13")

	for _, d := range Devices {
		fmt.Println(d)
		err := d.Connect()
		if err != nil {
			fmt.Println("Could not connect:", err)
			return
		}
		fmt.Println("Connected!")
	}

	// for _, d := range devices {
	// 	err := sendMessageWithParms(1, 2, 3, d)
	// 	if err != nil {
	// 		fmt.Println("Can't send:", err)
	// 		return
	// 	}
	// }

	// bs := make([]byte, 10)

	// bs[0] = 0x00 // mailbox number
	// bs[1] = 0x07 // message length including null terminator
	// bs[2] = 0x01
	// bs[3] = 0x02
	// bs[4] = 10
	// bs[5] = 11
	// bs[6] = 0x05
	// bs[7] = 0x06

	// 6 bytes
	// FF FF FF FF FF FF
	// type method payload payload payload error

	// FFFF FFFF FFFF
	// method payload error

	// for _, d := range devices {
	// 	err := sendMessageWithParms(1, 2, 3, d)
	// 	if err != nil {
	// 		fmt.Println("Can't send:", err)
	// 		return
	// 	}
	// }

	Status.Status = "idle"
	JobQueue = make(chan JobRequest)
	go jobQueue()
	go receiveStatusUpdates()

	router := setupRouter()
	// start http server
	go func() {
		err := http.ListenAndServe(":9090", router)
		if err != nil {
			fmt.Println(err.Error())
			os.Exit(1)
		}
	}()

	// err = sendMessageWithParms(255, 256, 257, n)
	// if err != nil {
	// 	fmt.Println("Can't send:", err)
	// 	return
	// }

	// fmt.Println("lets read something..")
	// reader := n.Connection()
	// reading := true
	// go func() {
	// 	for reading {
	// 		res := make([]byte, 64)

	// 		numRead, err := reader.Read(res)
	// 		if err != nil {
	// 			fmt.Println("Could not read:", err)
	// 			break
	// 		}
	// 		if res[0] == 128 {
	// 			fmt.Println("Reply:", numRead, res)
	// 			length := int(res[3])
	// 			msg := res[4:]

	// 			for i := 0; i < length; i++ {
	// 				fmt.Printf("%v ", msg[i])
	// 			}
	// 			fmt.Printf("\n")
	// 			fmt.Println(calculateIntFromLSBAndMSB(msg[0], msg[1]), calculateIntFromLSBAndMSB(msg[2], msg[3]), calculateIntFromLSBAndMSB(msg[4], msg[5]))

	// 		}
	// 		// 15 0 128 9 0 11 1 2 3 4 5 6 7 8 9 10 11

	// 		//fmt.Println("Reply:", numRead, res)

	// 	}
	// }()

	// Use a more traditional-looking method/check-for-error style
	//methodStyle(n)

	// Pause in between styles to ensure the old commands are done executing
	//time.Sleep(5 * time.Second)

	// Use the raw channels style
	//channelStyle(n)

	// Ctrl+C handling
	handler := make(chan os.Signal, 1)
	signal.Notify(handler, os.Interrupt, os.Kill)
	<-handler // block the thread
	fmt.Println("^C Shutting down...")
	//reading = false
	time.Sleep(2 * time.Second)

	for _, d := range Devices {
		fmt.Println("Disconnecting", d)
		err := d.Disconnect()
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
