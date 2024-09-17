all:	SHA1Attack.cpp
	@g++ -g -Wall -I openssl/include SHA1Attack.cpp -Lopenssl -lssl -lcrypto -o exe/SHA1Attack 


clean: 
	@rm exe/SHA1Attack

run:	all
	@./exe/SHA1Attack

output:	all
	@./exe/SHA1Attack > output.txt