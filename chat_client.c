/**    chat_client **/
 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<pthread.h>
#include<time.h>
 
#define BUF_SIZE 100
#define NORMAL_SIZE 20
 
void* send_msg(void* arg);
void* recv_msg(void* arg);
void error_handling(char* msg);
 
void menu();
void changeName();
void menuOptions(); 
 
char name[NORMAL_SIZE]="[DEFALT]";     // 이름
char msg_form[NORMAL_SIZE];            // msg form
char serv_time[NORMAL_SIZE];        // 서버 시간
char msg[BUF_SIZE];                    // msg
char serv_port[NORMAL_SIZE];        // 서버 PORT
char clnt_ip[NORMAL_SIZE];            // 클라이언트 IP
 
 
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    void* thread_return;
 
    if (argc!=4)
    {
        printf(" 입력 : %s <아이피> <포트번호> <닉네임>\n", argv[0]);
        exit(1);
    }
 
    /** 서버시간  **/
    struct tm *t;
    time_t timer = time(NULL);
    t=localtime(&timer);
    sprintf(serv_time, "%d-%d-%d %d:%d", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour,
    t->tm_min);
 
    sprintf(name, "[%s]", argv[3]);
    sprintf(clnt_ip, "%s", argv[1]);
    sprintf(serv_port, "%s", argv[2]);
    sock=socket(PF_INET, SOCK_STREAM, 0);
 
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_addr.sin_port=htons(atoi(argv[2]));
 
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
        error_handling(" 접속() 오류 : 포트번호를 확인해주세요");
 
    /** 메뉴 출력 **/
    menu();
 
    pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
    pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
    pthread_join(snd_thread, &thread_return);
    pthread_join(rcv_thread, &thread_return);
    close(sock);
    return 0;
}
 
void* send_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    char myInfo[BUF_SIZE];
    char* who = NULL;
    char temp[BUF_SIZE];
 
    /** send join messge **/
    printf(" >> 채팅에 접속 하셨습니다 !! \n");
    sprintf(myInfo, "%s'님이 접속하셨습니다. IP_%s\n",name , clnt_ip);
    write(sock, myInfo, strlen(myInfo));
 
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
 
        // 메뉴 접속 명령어 -> !메뉴
        if (!strcmp(msg, "!메뉴\n"))
        {
            menuOptions();
            continue;
        }
 
        else if (!strcmp(msg, "Quit\n") || !strcmp(msg, "Exit\n"))
        {
            close(sock);
            exit(0);
        }
 
        // 메세지 전송
        sprintf(name_msg, "%s %s", name,msg);
        write(sock, name_msg, strlen(name_msg));
    }
    return NULL;
}
 
void* recv_msg(void* arg)
{
    int sock=*((int*)arg);
    char name_msg[NORMAL_SIZE+BUF_SIZE];
    int str_len;
 
    while(1)
    {
        str_len=read(sock, name_msg, NORMAL_SIZE+BUF_SIZE-1);
        if (str_len==-1)
            return (void*)-1;
        name_msg[str_len]=0;
        fputs(name_msg, stdout);
    }
    return NULL;
}
 
 
void menuOptions() 
{
    int select;
    // 메뉴 출력
    printf("\n\t**** 메뉴 ****\n");
    printf("\t1. 이름 변경\n");
    printf("\t2. 채팅방 새로고침\n\n");
    printf("\t위 명령어 외에는 입력이 불가합니다.");
    printf("\n\t*******************");
    printf("\n\t엔터키를 누르면 메뉴가 닫힙니다. ");
    scanf("%d", &select);
    getchar();
    switch(select)
    {
        // change user name
        case 1:
        changeName();
        break;
 
        // console update(time, clear chatting log)
        case 2:
        menu();
        break;
 
        // menu error
        default:
        printf("\t취소.");
        break;
    }
}
 
 
/** 이름 변경 **/
void changeName()
{
    char nameTemp[100];
    printf("\n\t새로운 이름 입력  -> ");
    scanf("%s", nameTemp);
    sprintf(name, "[%s]", nameTemp);
    printf("\n\t설정완료.\n\n");
}
 
void menu()
{
    system("clear");
    printf(" **** LEE:LAP 채팅 클라이언트  ****\n");
    printf(" 서버 PORT  : %s \n", serv_port);
    printf(" 클라이언트 IP   : %s \n", clnt_ip);
    printf(" 채팅 이름   : %s \n", name);
    printf(" 서버 시간  : %s \n", serv_time);
    printf(" ************* 메뉴 ***************\n");
    printf(" 원하는 메뉴를 선택해주세요 -> !메뉴\n");
    printf(" 1. 이름 변경\n");
    printf(" 2. 채팅방 새로고침\n");
    printf(" **********************************\n");
    printf(" 채팅방 나가기 -> Quit & Exit\n\n");
}    
 
void error_handling(char* msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

