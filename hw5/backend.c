#include "common.h"

typedef struct Node {
    Record data;
    struct Node* next;
} Node;

Node* head = NULL;

void save_data(int sig) {
    FILE* fp = fopen(DATA_FILE, "w");
    if (fp) {
        Node* current = head;
        while (current != NULL) {
            fprintf(fp, "%s %s %d\n", current->data.name, current->data.id, current->data.deposit);
            current = current->next;
        }
        fclose(fp);
        // 使用 write 避免 printf 在 signal handler 中可能的 reentrancy 問題，不過作業通常 printf 沒關係
        const char* msg = "\n[Backend] Terminating. Data saved automatically.\n";
        write(STDOUT_FILENO, msg, strlen(msg));
    }
    exit(0);
}

// 讀檔函數：啟動時恢復資料 (符合「下次再開東西不能丟」)
void load_data() {
    FILE* fp = fopen(DATA_FILE, "r");
    if (!fp) {
        printf("[Backend] No previous data found. Starting fresh.\n");
        return;
    }
    
    char name[50], id[20];
    int deposit;
    // 簡單的讀檔邏輯
    while (fscanf(fp, "%s %s %d", name, id, &deposit) != EOF) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        strcpy(newNode->data.name, name);
        strcpy(newNode->data.id, id);
        newNode->data.deposit = deposit;
        newNode->next = NULL;
        
        // Append to tail
        if (head == NULL) {
            head = newNode;
        } else {
            Node* temp = head;
            while (temp->next) temp = temp->next;
            temp->next = newNode;
        }
    }
    fclose(fp);
    printf("[Backend] Data loaded from disk successfully.\n");
}

// 檢查重複
int check_duplicate(char* name, char* id) {
    Node* current = head;
    int name_dup = 0;
    int id_dup = 0;

    while (current != NULL) {
        if (strcmp(current->data.name, name) == 0) name_dup = 1;
        if (strcmp(current->data.id, id) == 0) id_dup = 1;
        current = current->next;
    }

    if (name_dup && id_dup) return RES_ERR_BOTH;
    if (name_dup) return RES_WARN_NAME;
    if (id_dup) return RES_WARN_ID;
    return RES_OK;
}

// 插入節點
void insert_node(char* name, char* id, int deposit, int mode) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    strcpy(newNode->data.name, name);
    strcpy(newNode->data.id, id);
    newNode->data.deposit = deposit;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }

    if (mode == 1) { 
        // Sort by Name (abcde...)
        if (strcmp(name, head->data.name) < 0) {
            newNode->next = head;
            head = newNode;
        } else {
            Node* current = head;
            while (current->next != NULL && strcmp(current->next->data.name, name) <= 0) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    } else { 
        // Append (ID duplicate or normal)
        Node* current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

void delete_node_by_name(char* name) {
    Node* temp = head;
    Node* prev = NULL;

    if (temp != NULL && strcmp(temp->data.name, name) == 0) {
        head = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL && strcmp(temp->data.name, name) != 0) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) return;
    prev->next = temp->next;
    free(temp);
}

// 轉成字串回傳給 Frontend
void list_all(int fd_res) {
    char buffer[BUFFER_SIZE] = "";
    char line[100];
    
    if (head == NULL) {
        strcpy(buffer, "No records found.\n");
    } else {
        Node* current = head;
        strcat(buffer, "Name\tID\tDeposit\n----------------------\n");
        while (current != NULL) {
            sprintf(line, "%s\t%s\t%d\n", current->data.name, current->data.id, current->data.deposit);
            strcat(buffer, line);
            current = current->next;
        }
    }
    write(fd_res, buffer, BUFFER_SIZE);
}

void search_node(int fd_res, char*name){
	Node* current = head;
	char buffer[BUFFER_SIZE] = "";
	int found = 0;

	while(current != NULL){
		if(strcmp(current->data.name, name) == 0){
			sprintf(buffer, "Name: %s, ID: %s, Deposit: %d", current->data.name, current->data.id, current->data.deposit);
			found = 1;
			break;
		}
		current = current->next;
	}
	if(!found){
		strcpy(buffer, "Record not found.");
	}
	write(fd_res, buffer, BUFFER_SIZE);
}

// --- Main ---

int main() {
    int fd_req, fd_res;
    char buffer[BUFFER_SIZE];

    signal(SIGPIPE, SIG_IGN);

    // 1. Auto-Recovery: 註冊信號處理 (Ctrl+C & Kill/Shutdown)
    signal(SIGINT, save_data);
    signal(SIGTERM, save_data);

    // 2. 建立 FIFO
    if (access(FIFO_REQ, F_OK) == -1) mkfifo(FIFO_REQ, 0666);
    if (access(FIFO_RES, F_OK) == -1) mkfifo(FIFO_RES, 0666);

    // 3. Load Data
    load_data();
    printf("[Backend] Server started (FIFO mode). Waiting for frontend...\n");

    // 4. Always Alive Loop
    while (1) {
        // 打開管道。open 會在此阻塞 (Block)，直到 Frontend 打開寫入端
        fd_req = open(FIFO_REQ, O_RDONLY);
        if (fd_req == -1) {
            perror("Open FIFO_REQ error");
            exit(1);
        }
        // 打開回應管道
        fd_res = open(FIFO_RES, O_WRONLY);
        if (fd_res == -1) {
            perror("Open FIFO_RES error");
            close(fd_req);
            continue;
        }

        printf("[Backend] Frontend connected.\n");

        // 內層迴圈：處理單次連線的所有指令
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);
            int n = read(fd_req, buffer, BUFFER_SIZE);
            
            if (n <= 0) { 
                // EOF: 表示 Frontend 關閉了連線
                printf("[Backend] Frontend disconnected.\n");
                break; 
            }

            char command[10], name[50], id[20];
            int deposit, mode;
            
            // 解析指令 (簡單字串解析)
            sscanf(buffer, "%s", command);

            if (strcmp(command, "CHECK") == 0) {
                sscanf(buffer, "%s %s %s", command, name, id);
                int status = check_duplicate(name, id);
                char res[10];
                sprintf(res, "%d", status);
                write(fd_res, res, BUFFER_SIZE);
            } 
            else if (strcmp(command, "INSERT") == 0) {
                sscanf(buffer, "%s %d %s %s %d", command, &mode, name, id, &deposit);
                insert_node(name, id, deposit, mode);
                write(fd_res, "Inserted", BUFFER_SIZE);
            }
            else if (strcmp(command, "LIST") == 0) {
                list_all(fd_res);
            }
            else if (strcmp(command, "DELETE") == 0) {
                sscanf(buffer, "%s %s", command, name);
                delete_node_by_name(name);
                write(fd_res, "Deleted", BUFFER_SIZE);
            }
	    else if (strcmp(command, "SEARCH") == 0){
		    sscanf(buffer, "%s %s", command, name);
		    search_node(fd_res, name);
	    }
            
        }

        // 清理本次連線的 fd，準備下一次 open
        close(fd_req);
        close(fd_res);
    }

    return 0;
}
