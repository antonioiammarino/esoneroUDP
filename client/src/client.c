#if defined WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h> //for isalnum()//
#include "protocol.h"

int main(int argc, char* argv[])
{
	#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			ErrorHandler("error at WSASturtup\n");
			system("pause");
			return EXIT_FAILURE;
		}
	#endif
	int sock;
	struct sockaddr_in server_addr;
	struct sockaddr_in fromAddr;
	struct in_addr *ina;
	struct hostent *host;
	struct hostent *remoteHost;
	unsigned int fromSize;
	char operation[MAX_BUF];
	char result[MAX_BUF];
	int operationlen;
	int respStringLen;
	if (argc != 2)
	{
		ErrorHandler("Incorrect parameters\n");
		system("pause");
	    exit(EXIT_FAILURE);
	}
	char* temp=strtok(argv[1], ":");
	char hostname[MAX_BUF];
	memset(hostname, '\0', MAX_BUF);
	strcpy(hostname, temp);
	int host_port = atoi((temp=strtok(NULL, ":")));

	for (int i=0; i<strlen(hostname); i++)
	{
		if(!(isalnum(hostname[i])))
		{
			if(hostname[i]!='-' && hostname[i]!='.')
			{
				ErrorHandler("Incorrect hostname\n");//hostname can contain letters, digits, '-' and '.'
				system("pause");
				exit(EXIT_FAILURE);
			}
		}
	}

	host=gethostbyname(hostname);
	if (host!=NULL)
	{
		ina=(struct in_addr*) host->h_addr_list[0];
	}
	else
	{
		ErrorHandler("gethostbyname() failed.\n");
		system("pause");
		exit(EXIT_FAILURE);
	}

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		ErrorHandler("socket() failed\n");
		system("pause");
		exit(EXIT_FAILURE);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(host_port);
	server_addr.sin_addr = *ina;

	memset(operation, '\0', MAX_BUF);
	puts("Enter the operation like this ----> operator[space]number[space]number[Enter] (Es. + 23 45)");
	fgets(operation, MAX_BUF, stdin);	//fgets reads the whole line including spaces
	operationlen=strlen(operation);
	if (sendto(sock, operation, operationlen, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) != operationlen)
	{
		ErrorHandler("sendto() sent different number of bytes than expected\n");
		system("pause");
		closesocket(sock);
		ClearWinSock();
		exit(EXIT_FAILURE);
	}
	while(strcmp(operation, "=\n")!=0)
	{
		memset(result, '\0', MAX_BUF);
		fromSize = sizeof(fromAddr);
		respStringLen=recvfrom(sock, result, MAX_BUF, 0, (struct sockaddr*) &fromAddr, &fromSize);
		if (server_addr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
		{
			ErrorHandler("Error: received a packet from unknown source.\n");
			system("pause");
			closesocket(sock);
			ClearWinSock();
			exit(EXIT_FAILURE);
		}
		if (respStringLen > 0)
		{
			remoteHost=gethostbyaddr((char *) &fromAddr.sin_addr, 4, PF_INET);
			printf("Result from server %s, ip %s: %s\n\n", remoteHost->h_name, inet_ntoa(fromAddr.sin_addr), result);
		}
		else
		{
			ErrorHandler("Result unknown: run the server or check the server parameters\n");
			system("pause");
			closesocket(sock);
			ClearWinSock();
			exit(EXIT_FAILURE);
		}

		memset(operation, '\0', MAX_BUF);	//new operation
		puts("Enter the operation like this ----> operator[space]number[space]number[Enter] (Es. + 23 45)");
		fgets(operation, MAX_BUF, stdin);	//fgets reads the whole line including spaces
		operationlen=strlen(operation);
		if (sendto(sock, operation, operationlen, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) != operationlen)
		{
			ErrorHandler("sendto() sent different number of bytes than expected\n");
			system("pause");
			closesocket(sock);
			ClearWinSock();
			exit(EXIT_FAILURE);
		}
	}

	closesocket(sock);
	ClearWinSock();
	system("pause");
	exit(EXIT_SUCCESS);
}
