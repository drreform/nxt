**Encode and send messages to NXT-based machines via Bluetooth**

## Encoder
The encoder encodes human readable data into hexadecimal digits in a format readable by the [Bluetooth API](https://github.com/CSCWLab2015/NXT/wiki/Bluetooth-API).


### Usage
```javascript
encodedString = Encode2Hex('B1', nxt.device.printer, nxt.type.request, nxt.method.print, '0001111111111111111111111111');
console.log(encodedString); // 0FB1A2A0B01FFFFFF

// TODO:
// sendBtMessage(encodedString);
```
