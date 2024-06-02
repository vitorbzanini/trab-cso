#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_LENGTH 256

int main(){
	int ret, fd;
	char receive[BUFFER_LENGTH];
	char stringToSend[BUFFER_LENGTH];
	fd = open("/dev/simple_driver", O_RDWR);		// Open the device with read/write access
	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}

	printf("Registre-se: reg <nome do process>\n");

	while(1){
				
		fgets(stringToSend, BUFFER_LENGTH - 1, stdin);
		
		stringToSend[BUFFER_LENGTH - 1] = '\0';
		stringToSend[strnlen(stringToSend, BUFFER_LENGTH) - 1] = '\0';

		if(!strcmp(stringToSend, "exit")){
			break;
		}
		else if(!strcmp(stringToSend, "read")){
				printf("Press ENTER to read back from the device...\n");
				printf("Reading from the device...\n");
	
				ret = read(fd, receive, BUFFER_LENGTH);			// Read the response from the kernel module
				if (ret < 0) {
					perror("Failed to read the message from the device.");
					return errno;
				}

				printf("The received message is: [%s]\n", receive);
				printf("End of the program\n");
		}
		else {
			printf("Registering ... [%s].\n", stringToSend);

			ret = write(fd, stringToSend, strlen(stringToSend));	// Send the string to the kernel module

			if (ret) {
				printf("Registered\n");
			} else {
				perror("Failed to write the message to the device.");
				return errno;
			}

			if (ret < 0) {
				perror("Failed to write the message to the device.");
				return errno;
			}
		}
	}


	/*fgets(stringToSend, BUFFER_LENGTH - 1, stdin);
	stringToSend[BUFFER_LENGTH - 1] = '\0';
	stringToSend[strnlen(stringToSend, BUFFER_LENGTH) - 1] = '\0';

	printf("Registering ... [%s].\n", stringToSend);

	ret = write(fd, stringToSend, strlen(stringToSend));	// Send the string to the kernel module

	if (ret) {
		printf("Registered\n");
	} else {
		perror("Failed to write the message to the device.");
		return errno;
	}

	if (ret < 0) {
		perror("Failed to write the message to the device.");
		return errno;
	}*/

	//printf("%ld e %ld\n", (long)getpid(), (long)getppid());

}

/*int main()
{
	int ret, fd;
	char receive[BUFFER_LENGTH];
	char stringToSend[BUFFER_LENGTH];
	
	printf("Starting device test code example...\n");

	fd = open("/dev/simple_driver", O_RDWR);		// Open the device with read/write access
	if (fd < 0) {
		perror("Failed to open the device...");
		return errno;
	}
	
	printf("Type in a short string to send to the kernel module:\n");

	fgets(stringToSend, BUFFER_LENGTH - 1, stdin);
	stringToSend[BUFFER_LENGTH - 1] = '\0';
	stringToSend[strnlen(stringToSend, BUFFER_LENGTH) - 1] = '\0';

	printf("Writing message to the device [%s].\n", stringToSend);
	
	ret = write(fd, stringToSend, strlen(stringToSend));	// Send the string to the kernel module
	
	printf("Type in a short string to send to the kernel module:\n");

	fgets(stringToSend, BUFFER_LENGTH - 1, stdin);
	stringToSend[BUFFER_LENGTH - 1] = '\0';
	stringToSend[strnlen(stringToSend, BUFFER_LENGTH) - 1] = '\0';

	printf("Writing message to the device [%s].\n", stringToSend);
	
	ret = write(fd, stringToSend, strlen(stringToSend));	// Send the string to the kernel module

	if (ret < 0) {
		perror("Failed to write the message to the device.");
		return errno;
	}

	printf("Press ENTER to read back from the device...\n");
	getchar();

	printf("Reading from the device...\n");
	
	ret = read(fd, receive, BUFFER_LENGTH);			// Read the response from the kernel module
	if (ret < 0) {
		perror("Failed to read the message from the device.");
		return errno;
	}

	printf("The received message is: [%s]\n", receive);
	printf("End of the program\n");
	
	
	return 0;
}*/
