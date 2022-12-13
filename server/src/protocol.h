#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#define MAX_BUF 256
#define PORT 48000

void ErrorHandler(char *errorMessage)
{
	printf(errorMessage);
}

void ClearWinSock()
{
	#if defined WIN32
		WSACleanup();
	#endif
}

#endif /* PROTOCOL_H_ */
