package goduino

import (
	"encoding/json"
	"github.com/tarm/goserial"
	"io"
	"message"
)

type Board struct {
	serial       io.ReadWriteCloser
	readDecoder  *json.Decoder
	writeEncoder *json.Encoder
}

func NewBoard(portName string, baud int) (*Board, error) {
	c0 := &goserial.Config{Name: portName, Baud: baud}
	s, err := goserial.OpenPort(c0)
	if err != nil {
		return nil, err
	}
	board := new(Board)
	board.serial = s
	board.readDecoder = json.NewDecoder(s)
	board.writeEncoder = json.NewEncoder(s)
	return board, nil
}

func (self *Board) Write(msg *message.Message) error {
	return self.writeEncoder.Encode(msg)
}

func (self *Board) Read() (*message.Message, error) {
	msg := new(message.Message)
	err := self.readDecoder.Decode(msg)
	return msg, err
}

func (self *Board) Close() error {
	return self.serial.Close()
}
