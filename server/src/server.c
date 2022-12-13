#if defined WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "protocol.h"

/* This function checks the correct number format:
 * number can't contain letters or characters other than digits;
 * number cant't be NULL */
int check_number(char* number)
{
	int error=0;
	if(number==NULL)
	{
		error++;
	}
	else
	{
		for(unsigned int i=0; i<strlen(number); i++)
		{
			if(isalnum(number[i]))
			{
				if(isalpha(number[i]))
				{
					error++;
					break;
				}
			}
			else
			{
				error++;
				break;
			}
		}
		if(number[0]=='-' || number[0]=='+')
				error--;
	}

	if(error!=0)
		return 0;
	else
		return 1;
}

// This function divides string
int token(char* item, int* num1, int* num2)
{
	item=strtok(NULL, " ");	//item now contains first number
	if(check_number(item))
	{
		*num1=atoi(item);
		item=strtok(NULL, "\0"); //item now contains second number
		if(check_number(item))
		{
			*num2=atoi(item);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}

}

int add(int num1, int num2)
{
	return num1+num2;
}

double division(int num1, int num2)
{
	return (double)num1/num2;
}

int sub(int num1, int num2)
{
	return num1-num2;
}

int mult(int num1, int num2)
{
	return num1*num2;
}

int main()
{
	#if defined WIN32
		WSADATA wsaData;
		int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0)
		{
			ErrorHandler("Error at WSASturtup\n");
			system("pause");
			ClearWinSock();
			return EXIT_FAILURE;
		}
	#endif
	int sock;
	struct sockaddr_in echoServAddr;
	struct sockaddr_in echoClntAddr;
	unsigned int cliAddrLen;
	struct hostent *remoteHost;
	char operation[MAX_BUF];
	char* operationwithoutnewline=NULL;
	int recvMsgSize;
	int num1, num2, result;
	double result1;	//result1 contains division result: it might be a double type number

	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		ErrorHandler("Socket() failed\n");
		system("pause");
		ClearWinSock();
		exit(EXIT_FAILURE);
	}

	memset(&echoServAddr, 0, sizeof(echoServAddr));
	echoServAddr.sin_family = AF_INET;
	echoServAddr.sin_port = htons(PORT);
	echoServAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if ((bind(sock, (struct sockaddr*) &echoServAddr, sizeof(echoServAddr)))< 0)
	{
		ErrorHandler("Bind() failed\n");
		system("pause");
		closesocket(sock);
		ClearWinSock();
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		memset(operation, '\0', MAX_BUF);
		cliAddrLen = sizeof(echoClntAddr);
		if((recvMsgSize = recvfrom(sock, operation, MAX_BUF, 0, (struct sockaddr*) &echoClntAddr, &cliAddrLen))<0)
		{
			ErrorHandler("Reception failed\n");
			system("pause");
			closesocket(sock);
			ClearWinSock();
			exit(EXIT_FAILURE);
		}

		remoteHost = gethostbyaddr((char *) &echoClntAddr.sin_addr, 4, AF_INET);
		operationwithoutnewline=strtok(operation, "\n");
		printf("Operation request '%s' from client %s, ip %s\n", operationwithoutnewline, remoteHost->h_name, inet_ntoa(echoClntAddr.sin_addr));
		char* error="Incorrect formatting or unknown operator\n";	//string if there are some errors
		int errorLen=strlen(error);
		while(strcmp(operation, "=")!=0)
		{
			if(strlen(operation)!=0)
			{
				char* item=strtok(operation, " ");	//item now contains operator
				if(item!=NULL) //item is null if the input contains only spaces
				{
					char operator=item[0];
					switch(operator)
					{
						case '+':
							if(token(item, &num1, &num2))
							{
								result=add(num1, num2);
								sprintf(operation, "%d %c %d = %d", num1, operator, num2, result);
								if (sendto(sock, operation, strlen(operation), 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr))!= strlen(operation))
								{
									ErrorHandler("sendto() sent different number of bytes than expected\n");
									system("pause");
									closesocket(sock);
									ClearWinSock();
									return EXIT_FAILURE;
								}
							}
							else //incorrect formatting
							{
								sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
							}
							break;

						case '-':
							if(token(item, &num1, &num2))
							{
								result=sub(num1, num2);
								sprintf(operation, "%d %c %d = %d", num1, operator, num2, result);
								if (sendto(sock, operation, strlen(operation), 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr))!= strlen(operation))
								{
									ErrorHandler("sendto() sent different number of bytes than expected\n");
									system("pause");
									closesocket(sock);
									ClearWinSock();
									return EXIT_FAILURE;
								}
							}
							else //incorrect formatting
							{
								sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
							}
							break;


						case 'x':
							if(token(item, &num1, &num2))
							{
								result=mult(num1, num2);
								sprintf(operation, "%d %c %d = %d", num1, operator, num2, result);
								if (sendto(sock, operation, strlen(operation), 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr))!= strlen(operation))
								{
									ErrorHandler("sendto() sent different number of bytes than expected\n");
									system("pause");
									closesocket(sock);
									ClearWinSock();
									return EXIT_FAILURE;
								}
							}
							else //incorrect formatting
							{
								sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
							}
							break;

						case '/':
							if(token(item, &num1, &num2))
							{
								result1=division(num1, num2);
								sprintf(operation, "%d %c %d = %.2f", num1, operator, num2, result1);
								if (sendto(sock, operation, strlen(operation), 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr))!= strlen(operation))
								{
									ErrorHandler("sendto() sent different number of bytes than expected\n");
									system("pause");
									closesocket(sock);
									ClearWinSock();
									return EXIT_FAILURE;
								}
							}
							else //incorrect formatting
							{
								sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
							}
							break;

						default:	//unknown operator
							sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
					}	//end switch
				}
			else	//only spaces
			{
				sendto(sock, error, errorLen, 0, (struct sockaddr*) &echoClntAddr, sizeof(echoClntAddr));
			}
		}

			memset(operation, '\0', MAX_BUF);
			if((recvMsgSize = recvfrom(sock, operation, MAX_BUF, 0, (struct sockaddr*) &echoClntAddr, &cliAddrLen))<0)
			{
				ErrorHandler("Reception failed\n");
				system("pause");
				closesocket(sock);
				ClearWinSock();
				exit(EXIT_FAILURE);
			}

			remoteHost = gethostbyaddr((char *) &echoClntAddr.sin_addr, 4, AF_INET);
			operationwithoutnewline=strtok(operation, "\n");
			printf("Operation request '%s' from client %s, ip %s\n", operationwithoutnewline, remoteHost->h_name, inet_ntoa(echoClntAddr.sin_addr)); //cambiare remoteHost

		} //end while
	}
}
