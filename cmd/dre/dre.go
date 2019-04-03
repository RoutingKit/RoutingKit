package main

import "routingkit/pkg/rkgolib"

import "fmt"

func main() {
	rkgolib.Init_routingkit("./data/new-york-latest.osm.pbf", true, true)
	from := rkgolib.Find_nearest_node(40.7121, -73.997, 200.0)
	fmt.Printf("[from] id: %d, distance: %f\n", from.GetId(), from.GetDistance())

	to := rkgolib.Find_nearest_node(40.7742, -73.9897, 200.0)
	fmt.Printf("[to] id: %d, distance: %f\n", to.GetId(), to.GetDistance())

	res := rkgolib.Find_shortest_path(from.GetId(), to.GetId())
	fmt.Printf("[result] total cost: %d msecs\n", res.GetTotal_distance())

	fmt.Println("[result] node path: ")
	for i := 0; int64(i) < res.GetNode_path().Size(); i++ {
		fmt.Printf("%d ", res.GetNode_path().Get(i))
	}

	fmt.Println("\n[result] arc path: ")
	for i := 0; int64(i) < res.GetArc_path().Size(); i++ {
		fmt.Printf("%d ", res.GetArc_path().Get(i))
	}
	fmt.Println("")

}
