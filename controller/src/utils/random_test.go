package utils

import (
	"testing"
)

func TestRandString(t *testing.T) {
	token := RandomString(10)
	t.Log(token)
}
