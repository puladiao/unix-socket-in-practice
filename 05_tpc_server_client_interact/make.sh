g++ lib.cpp client.cpp -o client -I../inc
g++ -c lib.cpp -o lib.o -I../inc
g++ -Wl,--as-needed server.cpp lib.o -o server -I../inc
