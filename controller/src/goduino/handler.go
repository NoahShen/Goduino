package goduino

import (
	"message"
)

type Handler interface {
	GetHandleCh() chan *message.Message
	Filter(*message.Message) bool
	IsOnce() bool
}

type MessageIdHandler struct {
	handlCh   chan *message.Message
	messageId string
}

func NewMessageIdHandler(messageId string) Handler {
	return &MessageIdHandler{make(chan *message.Message), messageId}
}

func (self *MessageIdHandler) GetHandleCh() chan *message.Message {
	return self.handlCh
}

func (self *MessageIdHandler) Filter(msg *message.Message) bool {
	return msg.Id == self.messageId
}

func (self *MessageIdHandler) IsOnce() bool {
	return true
}
