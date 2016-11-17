default:
	g++ -std=c++11 eventBenchmark.cpp -luv -lz -lssl -luWS -o eventBenchmark
	g++ -std=c++11 eventBenchmarkSocketIO.cpp -luv -lz -lssl -luWS -o eventBenchmarkSocketIO
	g++ -std=c++11 eventServer.cpp -luv -lz -lssl -luWS -o eventServer
