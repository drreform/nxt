package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"net/http"

	"github.com/gorilla/mux"
)

type JobRequest struct {
	Type   string `json:"type"`
	Letter string `json:"letter"`
}

type StatusReply struct {
	Method  int    `json:"method"`
	Payload int    `json:"payload"`
	Error   string `json:"error"`
}

func setupRouter() *mux.Router {
	router := mux.NewRouter().StrictSlash(true)
	router.Path("/").HandlerFunc(homeHandler)
	router.Methods("GET").Path("/status/{id}").HandlerFunc(statusHandler)
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
		fmt.Fprintf(w, "Bad Request: %v", err.Error())
		return
	}
	err = processJob(job)
	if err != nil {
		w.WriteHeader(http.StatusBadRequest)
		fmt.Fprintf(w, "Bad Job: %v", err.Error())
		return
	}

	w.WriteHeader(http.StatusAccepted)
}

func statusHandler(w http.ResponseWriter, r *http.Request) {
	params := mux.Vars(r)
	id := params["id"]
	log.Printf("Getting status of item %v", id)

	// Currently, item id is ignored. Assume only one item is processed at a time.
	b, _ := json.Marshal(&Status)
	w.Header().Set("Content-Type", "application/json")
	w.Write(b)
}
