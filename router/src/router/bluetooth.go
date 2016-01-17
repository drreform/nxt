/*
	Based on RobotC's predefined message format:
	http://help.robotc.net/WebHelpArduino/index.htm#page=NXT_Functions_New/NXT_Bluetooth_Overview.htm
*/

package main

import (
	"fmt"
	"log"

	nxt "github.com/tonyheupel/go-nxt"
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

// Send raw message
func writeToBluetooth(b []byte, n *nxt.NXT) error {
	log.Printf("Writing to device: %s", n)
	replyChannel := make(chan *nxt.ReplyTelegram)
	n.CommandChannel <- nxt.NewDirectCommand(0x09, b, replyChannel)
	reply := <-replyChannel
	log.Println(reply)

	if reply.IsSuccess() {
		return nil
	}
	return fmt.Errorf(reply.String())
}

// Send a message in RobotC's sendMessageWithParm format. Readable via messageParm[].
func sendMessageWithParms(messageParm1, messageParm2, messageParm3 int, n *nxt.NXT) error {
	b, err := encodeMessageWithParm(messageParm1, messageParm2, messageParm3)
	if err != nil {
		return err
	}
	log.Println(b)
	b, err = formatMessage(0, b)
	if err != nil {
		return err
	}
	log.Println(b)

	return writeToBluetooth(b, n)
}
