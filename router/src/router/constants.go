package main


// Device codes
const (
	PRINTER   = 1
	CONVEYOR  = 2
	LOADER    = 3
	WEBSERVER = 9
)

// methods
const (
	METHOD_STATUS = 800 // 8xx
	METHOD_ERROR  = 900 // 9xx

	// Command
	JOB_START = 201

	// Status
	JOB_SUBMITTED = 801
	JOB_DONE      = 803

	// Error
	BLUETOOTH_ERROR = 901
	JOB_NOTSTARTED  = 902
)
