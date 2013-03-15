package goduino

import ()

type Handler interface {
	GetHandleCh() chan *Message
	Filter(*Message) bool
	IsOnce() bool
}

type MessageIdHandler struct {
	handlCh   chan *Message
	messageId string
}

func NewMessageIdHandler(messageId string) Handler {
	return &MessageIdHandler{make(chan *Message), messageId}
}

func (self *MessageIdHandler) GetHandleCh() chan *Message {
	return self.handlCh
}

func (self *MessageIdHandler) Filter(msg *Message) bool {
	return msg.Id == self.messageId
}

func (self *MessageIdHandler) IsOnce() bool {
	return true
}
