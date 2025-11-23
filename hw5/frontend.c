#include "common.h"

void print_menu() {
    printf("\n-------Options-------\n");
    printf("(1) Insert data record\n");
    printf("(2) Search data record\n");
    printf("(3) Delete data record\n");
    printf("(4) List all data records\n");
    printf("(5) Close the front-end\n");
    printf("---------------------\n");
    printf("Select an option: ");
    fflush(stdout);
}

int main() {
    int fd_req, fd_res;
    char buffer[BUFFER_SIZE];
    
    // 嘗試打開管道
    // 注意：一定要先開 Write 再開 Read，順序要跟 Server 相反或配合，避免 Deadlock
    // 這裡 Server 是先 Open Read，所以 Client 這裡 Open Write 會成功握手
    printf("[Frontend] Connecting to backend...\n");
    fd_req = open(FIFO_REQ, O_WRONLY);
    if (fd_req == -1) {
        printf("Failed to open FIFO. Is the backend running?\n");
        return 1;
    }
    fd_res = open(FIFO_RES, O_RDONLY);
    if (fd_res == -1) {
        perror("Failed to open Response FIFO");
        close(fd_req);
        return 1;
    }
    printf("[Frontend] Connected!\n");

    int choice;
    while (1) {
        print_menu();
        if (scanf("%d", &choice) != 1) break;

        if (choice == 1) { // INSERT
            char name[50], id[20];
            int deposit;
            
            printf("Enter Name: ");
            scanf("%s", name);
            printf("Enter ID: ");
            scanf("%s", id);
            printf("Enter Deposit: ");
            scanf("%d", &deposit);

            // 1. CHECK
            sprintf(buffer, "CHECK %s %s", name, id);
            write(fd_req, buffer, BUFFER_SIZE);
            
            memset(buffer, 0, BUFFER_SIZE);
	    read(fd_res, buffer, BUFFER_SIZE);
            int status = atoi(buffer);
            int mode = 0; // 0=Append, 1=Sort
            int proceed = 1;

            if (status == RES_ERR_BOTH) {
                printf("Error: Both Name and ID already exist.\n");
                proceed = 0;
            } else if (status == RES_WARN_NAME) {
                printf("Warning: Name exists! Recommend changing.\n");
                printf("Still proceed? (This will sort by name) (y/n): ");
                char c;
                scanf(" %c", &c);
                if (c == 'y' || c == 'Y') mode = 1;
                else proceed = 0;
            } else if (status == RES_WARN_ID) {
                printf("Warning: ID exists! Recommend changing.\n");
                printf("Still proceed? (This will use input order) (y/n): ");
                char c;
                scanf(" %c", &c);
                if (c == 'y' || c == 'Y') mode = 0;
                else proceed = 0;
            }

            // 2. INSERT
            if (proceed) {
                sprintf(buffer, "INSERT %d %s %s %d", mode, name, id, deposit);
                write(fd_req, buffer, BUFFER_SIZE);
                memset(buffer, 0, BUFFER_SIZE);
		read(fd_res, buffer, BUFFER_SIZE); // Wait Ack
                printf("Server: %s\n", buffer);
            }

        } else if(choice == 2){
		char name[50];
		printf("Enter Name to search: ");
		scanf("%s", name);

		sprintf(buffer, "SEARCH %s", name);
		write(fd_req, buffer, BUFFER_SIZE);

		memset(buffer, 0, BUFFER_SIZE);
		read(fd_res, buffer, BUFFER_SIZE);
		printf("\n%s\n", buffer);
	
	} else if (choice == 3) { // DELETE
            char name[50];
            printf("Enter Name to delete: ");
            scanf("%s", name);
            
            sprintf(buffer, "DELETE %s", name);
            write(fd_req, buffer, BUFFER_SIZE);
            memset(buffer, 0, BUFFER_SIZE);
	    read(fd_res, buffer, BUFFER_SIZE);
            printf("Server: %s\n", buffer);

        } else if (choice == 4) { // LIST
            sprintf(buffer, "LIST");
            write(fd_req, buffer, BUFFER_SIZE);
            memset(buffer, 0, BUFFER_SIZE);
	    read(fd_res, buffer, BUFFER_SIZE);
            printf("\n%s\n", buffer);

        } else if (choice == 5) { // EXIT
            printf("Closing frontend.\n");
            break;
        }
    }

    close(fd_req);
    close(fd_res);
    return 0;
}
