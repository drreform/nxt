Encode and send messages to NXTs via Bluetooth

### Usage
```javascript
encodedString = Encode2Hex('B11', nxt.device.printer, nxt.type.request, nxt.method.print, '0001111111111111111111111111');
console.log(encodedString);

// TODO:
// sendBtMessage(encodedString);
```
