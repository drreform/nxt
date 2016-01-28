/*
	Based on RobotC's predefined message format:
	http://help.robotc.net/WebHelpArduino/index.htm#page=NXT_Functions_New/NXT_Bluetooth_Overview.htm

	NXT's Bluetooth Protocol:
	https://github.com/CSCWLab2015/nxt/blob/master/wiki/Appendix_2-LEGO_MINDSTORMS_NXT_Direct_commands.pdf

	Go driver used for NXT direct commands:
	https://github.com/heupel/go-nxt

	Raw message e.g.: 15 0 128 9 0 11 1 2 3 4 5 6 7 8 9 10 11

	// raw[0:2]: Bluetooth header: Length
	// raw[2]: Requires reply 0x00 or not 0x80
	// raw[3]: Message type. Code is 0x09 for text messages
	// raw[4]: Mailbox number 0x00-0x09 / Error > 0x00 [Appendix_2 pg.12]
*/

package main

import (
	"errors"
	"fmt"
	"log"

	nxt "github.com/farshidtz/go-nxt"
)

func calculateLSB(number int) byte {
	return byte(number & 0xff)
}

func calculateMSB(number int) byte {
	return byte((number >> 8) & 0xff)
}

func calculateIntFromLSBAndMSB(lsb byte, msb byte) int {
	return (int(msb) << 8) + int(lsb)
}

// Encode message to RobotC's sendMessageWithParm format
func encodeMessageWithParm(param1, param2, param3 int) ([]byte, error) {
	if param1 > 65535 || param2 > 65535 || param3 > 65535 {
		return nil, fmt.Errorf("Each message parameter must be smaller than 65535. Got %d, %d, and %d.",
			param1, param2, param3)
	}
	b := make([]byte, 6)
	b[0] = calculateLSB(param1)
	b[1] = calculateMSB(param1)
	b[2] = calculateLSB(param2)
	b[3] = calculateMSB(param2)
	b[4] = calculateLSB(param3)
	b[5] = calculateMSB(param3)
	return b, nil
}

// Decode message from RobotC's sendMessageWithParm format
func decodeMessageWithParm(message []byte) (int, int, int, error) {
	if len(message) != 6 {
		return 0, 0, 0, fmt.Errorf("MessageWithParm is %d bytes instead of 6.", len(message))
	}

	return calculateIntFromLSBAndMSB(message[0], message[1]),
		calculateIntFromLSBAndMSB(message[2], message[3]),
		calculateIntFromLSBAndMSB(message[4], message[5]),
		nil
}

// Format a raw message with  a specific mailbox ID
func formatMessage(mailboxID int, message []byte) ([]byte, error) {
	if len(message) >= 58 {
		return nil, fmt.Errorf("Message should be smaller than 58 bytes. Got %d.", len(message))
	}
	var formatted []byte
	formatted = append(formatted, byte(mailboxID&0xff))
	formatted = append(formatted, byte(len(message)&0xff))
	formatted = append(formatted, message...)
	return formatted, nil
}

const (
	TextMessage             byte = 0x09
	MessageRequiresResponse      = 0x00
	Reply                        = 0x02
	MessageNoResponse            = 0x80
	StopProgramReply             = 0x01
)

// Send raw message
func writeToBluetooth(b []byte, n *nxt.NXT) error {
	log.Printf("Writing to <%s> message: %v", n, b)

	//n.CommandChannel <- nxt.NewDirectCommand(0x09, b, nil)
	header := []byte{MessageRequiresResponse, TextMessage}

	reader := n.Connection()
	_, err := reader.Write(append(header, b...))
	if err != nil {
		return err
	}

	reply := nxt.ReplyStatus(<-AckChannel)
	log.Println("Status:", reply)

	if reply == nxt.Success {
		return nil
	}
	return fmt.Errorf(reply.String())

	// replyChannel := make(chan *nxt.ReplyTelegram)
	// n.CommandChannel <- nxt.NewDirectCommand(0x09, b, replyChannel)
	// reply := <-replyChannel
	// log.Println("Immediate reply:", reply)

	// if reply.IsSuccess() {
	// 	return nil
	// }
	// return fmt.Errorf(reply.String())

	//return nil
}

// Read raw message
func readFromBluetooth(n *nxt.NXT) ([]byte, int, error) {
	res := make([]byte, 64)

	reader := n.Connection()
	numRead, err := reader.Read(res)
	if err != nil {
		return nil, 0, err
	}
	return res, numRead, nil
}

// Send a message in RobotC's sendMessageWithParm format. Readable via messageParm[].
func sendMessageWithParms(messageParm1, messageParm2, messageParm3 int, n *nxt.NXT) error {
	b, err := encodeMessageWithParm(messageParm1, messageParm2, messageParm3)
	if err != nil {
		return err
	}
	//log.Println(b)
	b, err = formatMessage(0, b)
	if err != nil {
		return err
	}
	//log.Println(b)

	return writeToBluetooth(b, n)
}

var NotAMessage = errors.New("Not a message.")

// Read a message sent in RobotC's sendMessageWithParm format.
func readMessageWithParms(n *nxt.NXT) (int, int, int, error) {
	raw, size, err := readFromBluetooth(n)
	if err != nil {
		return 0, 0, 0, err
	}

	//log.Println("raw:", raw)
	if size == 3 &&
		raw[0] == Reply &&
		raw[1] == TextMessage || raw[1] == StopProgramReply {
		log.Println("Reply:", raw[:3])
		status := raw[2]
		AckChannel <- status
		return 0, 0, 0, NotAMessage
	}

	if size != 10 || raw[0] != MessageNoResponse || raw[1] != TextMessage {
		fmt.Println("ERROR: Bad message:", raw, size)
		return 0, 0, 0, fmt.Errorf("Header is invalid.")
	}
	res := raw[:size] // truncate trailing bytes
	//log.Println("Read:", size, res)

	if int(res[3]) != 6 {
		fmt.Printf("ERROR: Message is %v instead of 6 bytes: %v %v", size, res)
		return 0, 0, 0, fmt.Errorf("Message is invalid.")
	}

	msg := res[4:] // truncate the header
	//log.Println("Read params:", msg)

	param1, param2, param3, err := decodeMessageWithParm(msg)
	if err != nil {
		return 0, 0, 0, err
	}
	return param1, param2, param3, nil
}
