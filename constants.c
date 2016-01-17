// Devices
#define PLATES 0xA0
#define BRICKS 0xA1
#define PRINTER 0xA2
#define CONVEYOR 0xA3
#define DELIVERY 0xA4

// Method types
#define REQUEST 0x01
#define REPLY 0x02

// Methods

#define CONVEYOR_MOVE 0x02

// Errors

#define ERROR1 0xE1
#define ERROR2 0xE2
#define ERROR3 0xE3
#define ERROR4 0xE4

// Status
#define POGRESS 0xB1
#define PLOADED 0xB2 	// "Plate loaded on conveyor"
#define STARTPRINT 0xB3 // "Printing Start"
#define PRINTDONE 0xB4 // "Printing Done"
#define PCOLLECTION 0xB5 // "Plate read for collection"
