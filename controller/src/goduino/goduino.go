package goduino

import (
	"errors"
	"log"
	"message"
	"sync"
)

type Goduino struct {
	board      *Board
	writeQueue chan *message.Message
	stopSendCh chan int
	mutex      sync.Mutex
	handlers   []Handler
}

func NewGoduino() *Goduino {
	g := new(Goduino)
	g.writeQueue = make(chan *message.Message, 10)
	g.stopSendCh = make(chan int)
	return g
}

func (self *Goduino) Connect(portName string, baud int) error {
	b, err := NewBoard(portName, baud)
	if err != nil {
		return err
	}
	self.board = b
	go self.startSendMessage()
	go self.startReadMessage()
	return nil
}

func (self *Goduino) Disconnect() error {
	self.stopSendCh <- 1
	return self.board.Close()
}

func (self *Goduino) SendMessage(msg *message.Message) {
	self.writeQueue <- msg
}

func (self *Goduino) startSendMessage() {
	for {
		select {
		case msg := <-self.writeQueue:
			self.board.Write(msg)
		case <-self.stopSendCh:
			break
		}
	}
}

func (self *Goduino) startReadMessage() {
	for {
		msg, err := self.board.Read()
		if err != nil {
			log.Println("Read message error:", err)
			break
		}
		self.fireHandler(msg)
	}
}

func (self *Goduino) AddHandler(handler Handler) {
	self.mutex.Lock()
	defer self.mutex.Unlock()
	self.handlers = append(self.handlers, handler)
}

func (self *Goduino) RemoveHandler(handler Handler) {
	self.mutex.Lock()
	defer self.mutex.Unlock()
	for i, oldHandler := range self.handlers {
		if oldHandler == handler {
			self.handlers = append(self.handlers[0:i], self.handlers[i+1:]...)
			break
		}
	}
}

func (self *Goduino) RemoveHandlerByIndex(i int) {
	self.mutex.Lock()
	defer self.mutex.Unlock()
	self.handlers = append(self.handlers[0:i], self.handlers[i+1:]...)
}

func (self *Goduino) fireHandler(msg *message.Message) {
	for i := len(self.handlers) - 1; i >= 0; i-- {
		h := self.handlers[i]
		if h.Filter(msg) {
			h.GetHandleCh() <- msg
			if h.IsOnce() {
				self.RemoveHandlerByIndex(i)
			}
		}
	}
}

func (self *Goduino) IsReady() bool {
	request := message.CreateRequest(message.Status)
	h := NewMessageIdHandler(request.Id)
	self.AddHandler(h)
	self.SendMessage(request)
	resp := <-h.GetHandleCh()
	if resp.Result == message.Error {
		log.Println("Get status error:", errors.New(resp.Message))
		return false
	}
	return resp.Status == "started"
}
