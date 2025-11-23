#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define FIFO_REQ "fifo_req_to_server"
#define FIFO_RES "fifo_res_to_client"
#define DATA_FILE "data.txt"
#define BUFFER_SIZE 1024

typedef struct{
	char name[50];
	char id[20];
	int deposit;
} Record;

#define RES_OK 0
#define RES_WARN_NAME 1
#define RES_WARN_ID 2
#define RES_ERR_BOTH 3

#endif
