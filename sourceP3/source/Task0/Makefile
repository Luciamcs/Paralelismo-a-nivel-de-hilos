compile_suma_vectores: suma_vectores_float.cpp
	g++ -fopenmp suma_vectores_float.cpp -o suma_vectores_float

run_suma_vectores: compile_suma_vectores
	./suma_vectores_float

compile_async:
	g++ -fopenmp -lpthread async_functions.cpp -o async_functions

run_async: compile_async
	./async_functions

compile_initialize_vectors: initialize_vectors.cpp
	g++ -fopenmp initialize_vectors.cpp -o initialize_vectors

run_initialize_vectors: compile_initialize_vectors
	./initialize_vectors

clean:
	rm -f suma_vectores_float async_functions initialize_vectors