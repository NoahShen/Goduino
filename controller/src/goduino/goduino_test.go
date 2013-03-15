package goduino

import (
	"testing"
)

func createGoduino() *Goduino {
	g := NewGoduino()
	g.Connect("/dev/ttyS1", 9600)
	return g
}

func NoTestGetStatus(t *testing.T) {
	g := createGoduino()
	t.Log(g.IsReady())
}

func NoTestSetPinMode(t *testing.T) {
	g := createGoduino()
	g.SetPinMode("13", Output)
}

func NoTestDigitalRead(t *testing.T) {
	g := createGoduino()
	value, e := g.DigitalRead("13")
	if e != nil {
		t.Fatal(e)
	}
	t.Log("pin value:", value)
}

func NoTestDigitalWrite(t *testing.T) {
	g := createGoduino()
	e := g.DigitalWrite("13", HIGH)
	if e != nil {
		t.Fatal(e)
	}
	t.Log("Digital write success!")
}

func NoTestAnalogRead(t *testing.T) {
	g := createGoduino()
	value, e := g.AnalogRead("13")
	if e != nil {
		t.Fatal(e)
	}
	t.Log("pin value:", value)
}

func TestAnalogWrite(t *testing.T) {
	g := createGoduino()
	e := g.AnalogWrite("13", 87)
	if e != nil {
		t.Fatal(e)
	}
	t.Log("Analog write success!")
}
