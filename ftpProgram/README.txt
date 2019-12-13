INSTRUCTIONS

***********************
SERVER
***********************
1. Compile ftserver.c
	 gcc -o ftserver ftserver.c
2. Run ftserver
	 ./ftserver <port number>
***********************
CLIENT
***********************
1. Create executable for ftclient.c
	 chmod +x ftclient.c
2. Run ftclient
	 ./ftclient <host(flip1, flip2, etc.)> <source port> <command> <destination port>
Commands:
	-l (list files)
	-g <file name> (transfer specific file)
Examples:
	./ftclient flip1 1515 -l 1516
	./ftclient flip3 1515 -g file.txt 1516

NOTES:
	1. ftserver.c and ftclient.py must be located in different directories.
	2. ftserver.c and ftclient.py must be executed in different flip instances