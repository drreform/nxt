package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
	"time"

	"github.com/gorilla/mux"
)

type JobRequest struct {
	Type   string `json:"type"`
	Letter string `json:"letter"`
}

type StatusReply struct {
	Time    int64 `json:"time"`
	Method  int   `json:"method"`
	Payload int   `json:"payload"`
}

type ErrorReply struct {
	Error string `json:"error"`
}

func setupRouter() *mux.Router {
	router := mux.NewRouter().StrictSlash(true)
	router.Path("/").HandlerFunc(homeHandler)
	router.Methods("POST").Path("/job").HandlerFunc(jobHandler)

	return router
}

func homeHandler(w http.ResponseWriter, r *http.Request) {
	if r.URL.Path != "/" {
		http.NotFound(w, r)
		return
	}
	fmt.Fprintf(w, "Welcome to NXT's RESTFul API.")
}

func jobHandler(w http.ResponseWriter, r *http.Request) {
	log.Printf("Received job request.")
	body, err := ioutil.ReadAll(r.Body)
	r.Body.Close()

	var job JobRequest
	err = json.Unmarshal(body, &job)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)

		Error := ErrorReply{fmt.Sprintf("Bad Request: %v", err.Error())}
		b, _ := json.Marshal(&Error)
		w.Header().Set("Content-Type", "application/json")
		w.Write(b)
		return
	}
	err = processJob(job)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)

		Error := ErrorReply{err.Error()}
		b, _ := json.Marshal(&Error)
		w.Header().Set("Content-Type", "application/json")
		w.Write(b)
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

func postStatus(t time.Time, method, payload int) error {
	s := &StatusReply{
		Time:    t.UnixNano(),
		Method:  method,
		Payload: payload,
	}
	body, err := json.Marshal(s)
	if err != nil {
		return err
	}

	res, err := http.Post(WEBSERVER_URL, "application/json", bytes.NewReader(body))
	if err != nil {
		return err
	}
	if res.StatusCode != http.StatusOK {
		return fmt.Errorf("Status is %v:%v", res.Status, res.StatusCode)
	}
	return nil
}
