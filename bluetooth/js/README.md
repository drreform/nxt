### Javascript Message Encoder

**Encode messages to the format specified at [Bluetooth API](https://github.com/CSCWLab2015/NXT/wiki/Bluetooth-API).**

For convenience, the caller can use the predefined constants for `device`, message `type`, and message `method`.

### Usage
```javascript
encodedString = Encode2Hex('B1', nxt.device.printer, nxt.type.request, nxt.method.print, '0001111111111111111111111111');
console.log(encodedString); // 0FB1A2A0B01FFFFFF

```
