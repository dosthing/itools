all: 
	gcc -c cJSON.c -o cJSON.o
	ar rcs -o libcJSON.a cJSON.o
	gcc -o myjson example.c -L. -lcJSON


clean:
	rm -rf *.a *.o



