package message

import (
	"encoding/json"
	"testing"
)

func TestCreateRequest(t *testing.T) {
	for i := 0; i < 10; i++ {
		request := CreateRequest(Status)
		bytes, err := json.Marshal(request)
		if err != nil {
			t.Log(err)
			t.FailNow()
		}
		//t.Log("request:", request)
		t.Log("json:", string(bytes))
	}
}
