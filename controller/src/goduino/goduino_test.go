package goduino

import (
	"testing"
)

func TestGetStatus(t *testing.T) {
	g := NewGoduino()
	g.Connect("/dev/ttyS1", 9600)
	t.Log(g.IsReady())
}
