package rkgolib

/*
#cgo CXXFLAGS: -Wall -DNDEBUG -DROUTING_KIT_NO_ALIGNED_ALLOC -march=native -ffast-math -std=c++11 -O3 -fPIC -I../../include
#cgo LDFLAGS: -L../../lib -lroutingkit -lstdc++ -lm -lz -pthread
*/
import "C"
import "fmt"

func main() {
	fmt.Println("dummy-go")
}
