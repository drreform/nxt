*RESTFul NXT API*

### Binary Release
Binary releases for several system can be downloaded from [here](https://github.com/CSCWLab2015/nxt/releases).

### Usage
```
Windows:
router.exe -nxt1 COM_PORT -nxt2 COM_PORT -nxt3 COM_PORT
```

### Compile
You must have [Go](https://golang.org/) 1.5 or above to compile the code. For Go 1.6 and above, setting [GO15VENDOREXPERIMENT](https://docs.google.com/document/d/1Bz5-UB7g2uPBdOx-rw5t9MxJwkfpx90cqG9AFL0JAYo) variable might not be necessary.
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
