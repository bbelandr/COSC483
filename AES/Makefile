all:	AES.c
	@gcc -o exe/AES AES.c -Wall -g

run:	all
	@./exe/AES

clean:
	@rm -f exe/AES

test: all
	@./exe/AES > ./exe/output.txt
	diff -y ./exe/output.txt appendix_c.txt
