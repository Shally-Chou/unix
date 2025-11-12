#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <utmp.h>
#include <errno.h>

#define MAX_USERS 20

struct user_info{
	char username[UT_NAMESIZE + 1];
	char tty[UT_LINESIZE + 1];
};

int main(){
	struct utmp ut_buf;
	struct user_info users[MAX_USERS];
	int user_count = 0;
	int utmp_fd;

	if((utmp_fd = open("/var/run/utmp", O_RDONLY)) == -1){
		perror("Error opening /var/run/utmp");
		return 1;
	}

	while(read(utmp_fd, &ut_buf, sizeof(struct utmp)) > 0){
		if(ut_buf.ut_type == USER_PROCESS){
			strncpy(users[user_count].username, ut_buf.ut_user, UT_NAMESIZE);
			strncpy(users[user_count].tty, ut_buf.ut_line, UT_LINESIZE);

			users[user_count].username[UT_NAMESIZE] = '\0';
			users[user_count].tty[UT_LINESIZE] = '\0';

			user_count++;

			if(user_count >= MAX_USERS){
				break;
			}
		}
	}
	close(utmp_fd);

	printf("--- on-line friends ---\n");
	if(user_count == 0){
		printf("no one is online.\n");
		return 0;
	}

	for(int i = 0; i < user_count; i++){
		printf("%d. %s (%s)\n", i+1, users[i].username, users[i].tty);
	}
	printf("-------------------------\n");

	int choice = 0;
	while(choice < 1 || choice > user_count){
		printf("whom to send message (1-%d)? ", user_count);
		if(scanf("%d", &choice) != 1){
			while(getchar() != '\n');
		}
	}

	while(getchar() != '\n');

	char message[256];
	printf("enter message to send: ");
	fgets(message, sizeof(message), stdin);

	char tty_path[256];
	sprintf(tty_path, "/dev/%s", users[choice - 1].tty);

	int tty_fd;
	if((tty_fd = open(tty_path, O_WRONLY)) == -1){
		perror("error opening TTY file");
		printf("failed to open %s. do u have permission?\n", tty_path);
		return 1;
	}

	char* sender = getlogin();
	if(sender == NULL){
		sender = "someone";
	}

	char formatted_message[512];
	sprintf(formatted_message, "\n*** message form %s ***\n%s", sender, message);

	if(write(tty_fd, formatted_message, strlen(formatted_message)) == -1){
		perror("error writing to TTY");
		close(tty_fd);
		return 1;
	}

	close(tty_fd);
	printf("message sent successfully to %s.\n", users[choice - 1].username);
	
	return 0;
}
