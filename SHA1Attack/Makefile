all:	SHA1Attack.cpp
	@g++ -g -Wall -I openssl/include -I matplotplusplus/source SHA1Attack.cpp -Lopenssl -lssl -lcrypto -Lmatplotplusplus/build/source/matplot/ -lmatplot -Lmatplotplusplus/build/source/3rd_party -lnodesoup -o exe/SHA1Attack 


clean: 
	@rm exe/SHA1Attack

run:	all
	@./exe/SHA1Attack

output:	all
	@./exe/SHA1Attack > output.txt