Encode and send messages to NXTs via Bluetooth

## Encoder
The encoder encodes human readable data into hexadecimal, ready to be sent to an NXT device via Bluetooth.

An encoded message has the following format:

`<messageLength> <senderID> <receiverID> <messageType> <method> <data>`

where each `<>` represents 1 byte, that is two hexadecimal digits.

### Usage
```javascript
encodedString = Encode2Hex('B1', nxt.device.printer, nxt.type.request, nxt.method.print, '0001111111111111111111111111');
console.log(encodedString); // 0FB1A2A0B01FFFFFF

// TODO:
// sendBtMessage(encodedString);
```
