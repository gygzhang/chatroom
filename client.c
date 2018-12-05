
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>


static unsigned int count = 0;
static unsigned int uid = 10;

typedef struct client_node
{
	struct  ClientInfoStruct *pClientInfo;
	client_node* next;
}ClientInfoStruct_node;

struct ClientInfoStruct
{
	sockaddr_in c_addr;
	int clientfd;
	int uid;
	char name[36];
};


void print(void);

ClientInfoStruct_node* head;

void addClient(ClientInfoStruct **cli);

void deleteClientByUid(int uid);
//send a message to all the clent but the sender
void sendToAllExceptSender(char *s, int uid);

//send to all
void sendToAllTheLinkedList(char *s);//send to all
									 //send a message to the sender
void sendMessageToSelf(const char *s, int connfd);
//send message to a client by uid
void sendMessageByUid(char *s, int uid);
//see how many clients are online
void sendMessageToOnline(int connfd);
//get rid of the '\n'
void getRidOfTheNewLine(char *s);
//transform the integral to ip adress using bitwise
void getAndPrintTheIpAddress(sockaddr_in addr);

void welcome_info(ClientInfoStruct_node *p);

void sendByName(char *name, char*);
//the thread function to handle the communcation among all the client
void* handle_client(LPVOID arg);

void* handle_img(LPVOID arg);

int main()
{
	//CInitSock s;
	int listenfd = 0, connfd = 0;
	sockaddr_in serv_addr, cli_addr;
	head = NULL;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_addr.S_un.S_addr = inet_addr(INADDR_ANY);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(1234);

	if (bind(listenfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Socket binding failed");
		return 1;
	}

	if (listen(listenfd, 10) < 0) {
		printf("Socket listening failed");
		return 1;
	}

	printf("*****************************************************start*******************************************************\n");

	pthread_t tid;
    pthread_create(&tid,0,handle_client,p->pClientInfo);

	while (TRUE)
	{
		int cli_len = sizeof(cli_addr);
		//the accept function will using the listenfd to listen the new connect,and give the fd to connfd
		connfd = accept(listenfd, (sockaddr*)&cli_addr, &cli_len);
		//the information of the client
		ClientInfoStruct *u = (ClientInfoStruct *)malloc(sizeof(struct ClientInfoStruct));
		//ClientInfoStruct *u;
		//client information node malloc
		ClientInfoStruct_node *p = (ClientInfoStruct_node*)malloc(sizeof(ClientInfoStruct_node));
		p->next = NULL;
		//the fd to send with
		u->clientfd = connfd;
		u->c_addr = cli_addr;
		u->uid = uid++;
		sprintf(u->name, "%d", u->uid);
		strcat(u->name, "\0");

		p->pClientInfo = u;
		//puts(p->pClientInfo->name);
		//printf("***%s***", p->pClientInfo->name);

		addClient(&p->pClientInfo);
		pthread_create(&tid,0,handle_client,p->pClientInfo);

		Sleep(5);
	}
	return 0;
}


void print(void)
{
	ClientInfoStruct_node *p = head;
	while (p != NULL)
	{
		printf("UID:%d  pnode: %-10p  pdata: %-10p pnext:%p head: %-10p\r\n", p->pClientInfo->uid,p,p->pClientInfo,p->next,head);
		p = p->next;
	}
	putchar(10);
}


void addClient(ClientInfoStruct **cli)
{
	ClientInfoStruct_node* temp = head;
	ClientInfoStruct *u = (ClientInfoStruct *)malloc(sizeof(struct ClientInfoStruct));
	ClientInfoStruct_node *pClientInfoNode = (ClientInfoStruct_node*)malloc(sizeof(ClientInfoStruct_node));
	pClientInfoNode->next = NULL;

	u->clientfd = (*cli)->clientfd;
	u->c_addr = (*cli)->c_addr;
	u->uid = (*cli)->uid;
	//*u->name = *(*cli)->name;//！！！！A wrong strings copy way！！！！！！！！！！！！！！
	strcpy(u->name, (*cli)->name);
	//puts(u->name);

	pClientInfoNode->pClientInfo = u;
	/*pClientInfoNode->pClientInfo->clientfd = cli->clientfd;
	pClientInfoNode->pClientInfo->c_addr = cli->c_addr;
	*pClientInfoNode->pClientInfo->name = *cli->name;
	pClientInfoNode->pClientInfo->uid = cli->uid;*/
	if (head == NULL) {
		head = pClientInfoNode;
		///std::cout << "\nok\n";
	}

	else// if(head!=NULL)
	{// TODO: temp!=NULL can cause the linked-list not linked! why???
		while (temp->next != NULL) temp = ((temp)->next);
		temp->next = pClientInfoNode;
		//print();
	}
	print();

	//std::cout << head;
}

void deleteClientByUid(int uid)
{
	ClientInfoStruct_node *p = head;
	ClientInfoStruct_node *c = head;
	while (c != NULL)
	{
		if (c->pClientInfo->uid == uid)
		{
			puts("tes");
			p->next = c->next;
			//count--;
		}
		p = c;
		c = c->next;
	}
	
	
	
	
	
}

void sendToAllExceptSender(char *s, int uid)
{
	ClientInfoStruct_node *temp = head;
	while (temp != NULL)
	{
		if (temp->pClientInfo->uid != uid)
			send(temp->pClientInfo->clientfd, s, 4096, 0);
		temp = temp->next;
	}
}

void sendToAllTheLinkedList(char *s)
{
	ClientInfoStruct_node *temp = head;
	while (temp != NULL)
	{
		send(temp->pClientInfo->clientfd, s, 4096, 0);
		temp = temp->next;
	}
}

void sendMessageToSelf(const char *s, int connfd) {
	send(connfd, s, 4096, 0);
}

void sendMessageByUid(char *s, int uid)
{
	ClientInfoStruct_node *temp = head;
	while (temp != NULL)
	{
		if (temp->pClientInfo->uid == uid) send(temp->pClientInfo->clientfd, s, 4096, 0);
		temp = temp->next;
	}
}

void sendMessageToOnline(int connfd)
{
	ClientInfoStruct_node *temp = head;
	char *info=(char*)malloc(200);
	char *tot_str = (char*)malloc(4096);
	//memset(tot_str,'\0',4096);
	//memset(info,'\0',4096);	
	while (temp != NULL)
	{
		sprintf(info, "<< CLIENT UID: %d NAME: %s \r\n", temp->pClientInfo->uid, temp->pClientInfo->name);
		strcat(tot_str,info);
		temp = temp->next;
	}
	sendMessageToSelf(tot_str, connfd);
	free(info);
	free(tot_str);
}

void getRidOfTheNewLine(char *s) {
	while (*s != '\0') {
		if (*s == '\n') {
			*s = '\0';
		}
		s++;
	}
}

void getAndPrintTheIpAddress(sockaddr_in addr) {
	printf("%d.%d.%d.%d",
		addr.sin_addr.s_addr & 0xFF,
		(addr.sin_addr.s_addr & 0xFF00) >> 8,
		(addr.sin_addr.s_addr & 0xFF0000) >> 16,
		(addr.sin_addr.s_addr & 0xFF000000) >> 24);
}

void welcome_info(ClientInfoStruct_node *p)
{
	printf("<<欢迎:");
	getAndPrintTheIpAddress(p->pClientInfo->c_addr);
	printf(" With Uid: %d\n", p->pClientInfo->uid);
}

void sendByName(char *name, char *msg)
{
	ClientInfoStruct_node *temp = head;
	int m, n;
	//puts(msg);
	//puts(name);
	m = strlen(name);
	n = strlen(temp->pClientInfo->name);
	//name[m] = '\0';
	//(temp->pClientInfo->name)[n] = '\0';

	while (temp != NULL)
	{


		if (!strcmp(temp->pClientInfo->name, name))
		{
			send(temp->pClientInfo->clientfd, msg, 4096, 0);
			
		}
		temp = temp->next;
	}
}

void* handle_client(void* arg)
{
	char buff_out[4096];
	char buff_in[4096] = { 0 };
	int len;

	count++;
	ClientInfoStruct*p = (ClientInfoStruct*)arg;


	/*welcome */
	printf("<<welcome:");
	getAndPrintTheIpAddress(p->c_addr);
	printf("|%d\n", p->uid);

	sprintf(buff_out, "<< %s加入聊天!\r\n", p->name);
	sendToAllTheLinkedList(buff_out);


	while (len = recv(p->clientfd, buff_in, 4096, 0) > 0)
	{
		//std::cout << len;
		//buff_in[4096] = '\0';
		getRidOfTheNewLine(buff_in);
		puts(buff_in);

		if (!strlen(buff_in)) continue;

		if (buff_in[0] == '#')
		{
			char *command, *param;
			command = strtok(buff_in, " ");
			if (!strcmp(command, "#QUIT")) closesocket(p->clientfd);
			else if (!strcmp(command, "#TEST")) sendMessageToSelf("<<IT'S OK\r\n", p->clientfd);
			else if(!strcmp(command, "#CHECK")) 
			{
				char *stt=(char*)malloc(200);
				char *tol_str=(char*)malloc(4096);
				ClientInfoStruct_node *c = head;
				sprintf(stt,"总在线人数:  %d\r\n",count);
				//memset(stt,'\0',4096);
				strcat(tol_str,stt);
				while (c != NULL)
				{
				sprintf(stt,"UID:%d  pnode: %d  pdata: %d pnext:%0d head: %d\r\n", c->pClientInfo->uid,c,c->pClientInfo,c->next,head);						
				strcat(tol_str,stt);
				c = c->next;
				}
				sendMessageToSelf(tol_str,p->clientfd);
				free(stt);
				free(tol_str);
				putchar(10);
			
			}
			
			else if (!strcmp(command, "#GETIP"))
			{
				char ip_str[1000];
				memset(ip_str,'\0',1000);
				sprintf(ip_str,"%d.%d.%d.%d\r\n",
					p->c_addr.sin_addr.s_addr & 0xFF,
					(p->c_addr.sin_addr.s_addr & 0xFF00) >> 8,
					(p->c_addr.sin_addr.s_addr & 0xFF0000) >> 16,
					(p->c_addr.sin_addr.s_addr & 0xFF000000) >> 24);				
				sendMessageToSelf(ip_str,p->clientfd);
			}
			else if (!strcmp(command, "#RENAME"))
			{
				param = strtok(NULL, " ");
				if (param)
				{
					sprintf(buff_out, "<<RENAME %s to %s\n", p->name, param);
					strcpy(p->name, param);
					sendToAllTheLinkedList(buff_out);
				}
				else
				{
					sprintf(buff_out, "<<NAME CANNOT BE NULL\n");
					sendToAllTheLinkedList(buff_out);
				}


			}
			
			else if (!strcmp(command, "#TO"))
			{
				char *msg;
				char *name = strtok(NULL, " ");
				if (name) {
					//puts("name is:\n");
					//puts(name);
					msg = strtok(NULL, " ");
					sprintf(buff_out, "[%s]", p->name);
					strcat(buff_out, msg);
					if (msg)
					{
						msg = strtok(NULL, " ");
						while (msg)
						{
							strcat(buff_out, " ");
							strcat(buff_out, msg);
							msg = strtok(NULL, " ");
						}
						strcat(buff_out, "\n");

						sendByName(name, buff_out);
					}
					else
					{
						sendMessageToSelf("<<MESSAGE CANNOT BE NULL\r\n", p->clientfd);
					}
				}
				else
				{
					sendMessageToSelf("<<PERSON CANNOT BE NULL\r\n", p->clientfd);
				}
			}
			else if (!strcmp(command, "#ONLINE")) {
				sprintf(buff_out, "<<在线人数： %d 个\r\n", count);
				sendMessageToSelf(buff_out, p->clientfd);
				sendMessageToOnline(p->clientfd);
			}
			else if (!strcmp(command, "#HELP")) {
				strcat(buff_out, "#QUIT     Quit the chatroom\r\n");
				strcat(buff_out, "#TEST     Server test\r\n");
				strcat(buff_out, "#RENAME     [name] Change nickname\r\n");
				strcat(buff_out, "#TO  [name] [message] Send private message\r\n");
				strcat(buff_out, "#ONLINE   Show who is online \n");
				strcat(buff_out, "#HELP     Show help\r\n");
				sendMessageToSelf(buff_out, p->clientfd);
			}
			else
			{
				sendMessageToSelf("<<UNKOWN COMMAND\r\n", p->clientfd);
			}
		}
		else
		{
			sprintf(buff_out, "[%s] %s \r\n", p->name, buff_in);
			sendToAllExceptSender(buff_out, p->uid);
		}

	}

	closesocket(p->clientfd);
	sprintf(buff_out, "<< %s left\r\n", p->name);
	sendToAllTheLinkedList(buff_out);

	deleteClientByUid(p->uid);
	printf("<< leave");
	getAndPrintTheIpAddress(p->c_addr);
	printf(" ID %d\r\n", p->uid);
	count--;
	printf("count: %d\r\n", count);
	//print();
	//count--;
	//printf("count: %d\n",count);
	pthread_exit("done!");

	return NULL;

}

/*DWORD WINAPI handle_img(LPVOID arg)
{
	CInitSock s;
	int listenfd_1 = 0, connfd_1 = 0;
	SOCKADDR_IN serv_addr_1, cli_addr_1;
	head = NULL;
	char buf[99999];

	listenfd_1 = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr_1.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serv_addr_1.sin_family = AF_INET;
	serv_addr_1.sin_port = htons(8000);

	if (bind(listenfd_1, (SOCKADDR*)&serv_addr_1, sizeof(serv_addr_1)) < 0) {
		printf("Socket binding failed");
		return 1;
	}

	if (listen(listenfd_1, 10) < 0) {
		printf("Socket listening failed");
		return 1;
	}
	printf("\nnew thread(for img)\n");
	while (TRUE)
	{
		int cli_len = sizeof(cli_addr_1);
		connfd_1 = accept(listenfd_1, (SOCKADDR*)&cli_addr_1, &cli_len);

		recv(connfd_1, buf, 99999, 0);
		send(connfd_1, buf, 99999, 0);

		//Sleep(5);

	}
	return NULL;
}*/