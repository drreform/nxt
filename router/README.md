*RESTFul NXT API*

### Usage
```
Windows:
router.exe -nxt1 COM_PORT -nxt2 COM_PORT inxt3 COM_PORT
```

### Compile
You must have [Go](https://golang.org/) 1.5 or above to compile the code:  
*Go 1.5*
```
Windows:
set GO15VENDOREXPERIMENT=1
set GOPATH=%cd%
go install ./...

Linux:
export GO15VENDOREXPERIMENT=1
export GOPATH=`pwd`
go install ./... 
```
