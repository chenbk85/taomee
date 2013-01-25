g++ -lcurl -c http_transfer.cpp 
g++ -o poster http_transfer.o -lcurl  main.cpp
