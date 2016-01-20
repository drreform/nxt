// Devices /////////
#define PRINTER 1
#define CONVEYOR 2
#define LOADER 3
#define WEBSERVER 9
////////////////////

// Status Codes
#define	STT_METHOD 800
#define STT_JOB_SUBMITTED 801
#define STT_JOB_STARTED 802
#define STT_JOB_DONE 803

// Error Codes
#define ERR_METHOD  900
#define ERR_NO_BRICKS 910
#define ERR_BRICK_NOT_PICKED 911
#define ERR_BRICK_NOT_PLUGGED 912
#define ERR_BRICK_MISPLUGGED 913

// Commands
#define CONVEYOR_JOB_START 201
#define CONVEYOR_JOB_DONE 202
#define CONVEYOR_JOB_DISCARD 203
#define CONVEYOR_MOVE 210
#define PRINTER_PRINT 101
