package goduino

import (
	"testing"
)

func NoTestRandString(t *testing.T) {
	token := RandomString(10)
	t.Log(token)
}
