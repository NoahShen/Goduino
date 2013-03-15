package goduino

import (
	"encoding/json"
	"testing"
)

func NoTestCreateBoard(t *testing.T) {
	board, err := NewBoard("/dev/ttyS1", 9600)
	if err != nil {
		t.Log(err)
		t.FailNow()
	}
	request := CreateRequest(Status)
	wErr := board.Write(request)
	if wErr != nil {
		t.Fatal(wErr)
	}
	resp, rErr := board.Read()
	if rErr != nil {
		t.Fatal(rErr)
	}
	bytes, mErr := json.Marshal(resp)
	if mErr != nil {
		t.Fatal(mErr)
	}
	t.Log("json:", string(bytes))
}
