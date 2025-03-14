<<<<<<< HEAD
/* 
 * server program of 52_3
 */
#include <unistd.h>
#include <mqueue.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MSG_SIZE 2048
#define SERVER_MQ_NAME "/server_mq"
#define CLIENT_MQ_NAME "/client_mq"


struct request_msg{
	pid_t pid;
	char pathname[PATH_MAX];
};

struct response_msg{
	char data[MSG_SIZE];
};
int main(int argc, char *argv[]){
	mqd_t mq_server, mq_client;
	char mq_client_name[1024] = {'\0'};

	int req_file_fd, read_size;
	struct request_msg server_req;
	struct response_msg server_res;
	struct mq_attr attr;

	memset(&attr, 0, sizeof(struct mq_attr));	
	attr.mq_msgsize = sizeof(struct request_msg);
	attr.mq_maxmsg = 4;

	if((mq_server = mq_open(SERVER_MQ_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr)) == (mqd_t)-1){
		perror("mq_open");
		exit(1);
	}

	//fprintf(stderr, "mq_server opened\n");

	while(1){
		if(mq_receive(mq_server, (char *)&server_req, sizeof(struct request_msg), 0) == -1){
			perror("mq_receive");
			exit(1);
		}

		/* 
		 * get the actual client mq name
		 */
		snprintf(mq_client_name, 1024, "%s%d", CLIENT_MQ_NAME, server_req.pid);
		fprintf(stderr, "mq_client_name: %s\n", mq_client_name);
		if((mq_client = mq_open(mq_client_name, O_WRONLY)) == (mqd_t)-1){
			perror("mq_open");
			continue;
		}
		
		fprintf(stderr, "received file name: %s\n", server_req.pathname);

		if((req_file_fd = open(server_req.pathname, O_RDONLY)) == -1){
			perror("open request file");
			continue;
		}

		/* 
		 * send the content of file to client by mq_send
		 */
		while((read_size = read(req_file_fd, server_res.data, MSG_SIZE - 1)) > 0){
			if(mq_send(mq_client, (char *)&server_res, sizeof(struct response_msg), 0) == -1){
				perror("mq_send");
				break;
			}
			memset(&server_res, 0, sizeof(struct response_msg));
		}

		if(close(req_file_fd) == -1){
			perror("close");
			exit(1);
		}

		/*
		 * signify the client with NULL message
		 */
		// memset(server_res.data, '\0', MSG_SIZE);
		server_res.data[0] = '\0';
		if(mq_send(mq_client, (char *)&server_res, sizeof(struct response_msg), 0) == -1){
			perror("mq_send");
			continue;
		}
		if(mq_close(mq_client) == -1){
			perror("mq_close");
			continue;
		}
	}
	
	if(mq_close(mq_server) == -1){
		perror("mq_close");
		exit(1);
	}

	if(mq_unlink(SERVER_MQ_NAME) == -1){
		perror("mq_unlink");
		exit(1);
	}

	exit(EXIT_SUCCESS);

=======
#include "seqnum.h"

#define MAX_MSG_SIZE 1024

void errExit(const char *err) {
    perror(err);
    exit(EXIT_FAILURE);
}

// server發送當前seqnum，並更新加上client傳來的seqnum
int main() {
    mqd_t serverQueue;
    char clientQueueName[128];
    struct mq_attr attr;
    struct request req;
    struct response resp;
    int seqNum = 0;

    // 設定訊息佇列屬性
    attr.mq_flags = 0;               // 阻塞模式
    attr.mq_maxmsg = 10;             // 最大訊息數
    attr.mq_msgsize = MAX_MSG_SIZE;  // 每則訊息的最大大小
    attr.mq_curmsgs = 0;             // 當前訊息數

    // 創建伺服器的 POSIX 訊息佇列
    if ((serverQueue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr)) == (mqd_t)-1) {
        errExit("mq_open");
    }

    for (;;) {
        // 從伺服器佇列中接收請求
        if (mq_receive(serverQueue, (char *)&req, MAX_MSG_SIZE, NULL) == -1) {
            errExit("mq_receive");
        }

        // 客戶端訊息佇列名稱
        snprintf(clientQueueName, sizeof(clientQueueName), "/seqnum_cl.%ld", (long)req.pid);

        // 回應client
        resp.seqNum = seqNum;
        seqNum += req.seqLen;

        // 發送回應到客戶端的 POSIX 訊息佇列
        mqd_t clientQueue;
        if ((clientQueue = mq_open(clientQueueName, O_WRONLY)) == (mqd_t)-1) {
            fprintf(stderr, "Error opening client queue: %s\n", clientQueueName);
            continue;
        }

        if (mq_send(clientQueue, (char *)&resp, sizeof(resp), 0) == -1) {
            fprintf(stderr, "Error sending response to client\n");
        }

        mq_close(clientQueue);
    }

    mq_close(serverQueue);
    mq_unlink(SERVER_QUEUE_NAME);
    return 0;
>>>>>>> 65e707a (提交專案)
}