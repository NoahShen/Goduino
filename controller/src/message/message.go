package message

import (
	"strconv"
	"utils"
)

type Action string

const (
	Status       = Action("status")
	Mode         = Action("mode")
	DigitalWrite = Action("digitalwrite")
	DigitalRead  = Action("digitalread")
	AnalogWrite  = Action("analogwrite")
	AnalogRead   = Action("analogread")
)

type Result string

const (
	Success = Result("success")
	Error   = Result("error")
)

type PinMode string

const (
	Output = PinMode("OUTPUT")
	Input  = PinMode("INPUT")
)

type Message struct {
	Id      string   `json:"id,omitempty"`
	Action  Action   `json:"action,omitempty"`
	Result  Result   `json:"result,omitempty"`
	Status  string   `json:"status,omitempty"`
	Message string   `json:"message,omitempty"`
	PayLoad *PayLoad `json:"payload,omitempty"`
}

type PayLoad struct {
	Pin   string  `json:"pin,omitempty"`
	Mode  PinMode `json:"mode,omitempty"`
	Value string  `json:"value,omitempty"`
}

var i = 0

func CreateRequest(action Action) *Message {
	id := utils.RandomString(10) + "-" + strconv.Itoa(i)
	i++
	message := &Message{Id: id, Action: action}
	return message
}
