#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>	//open 때문에 include
#include <time.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>

#define ComNum 5
#define BUFSIZE 100

pid_t pcid[ComNum]; // PC 프로세스 id 값 저장하는 배열

void makeFile() 
{
	for (int i = 0; i < ComNum; i++)
	{
		char fpath[BUFSIZE] = "./pc";
		char temp[5] = { 0 };
		sprintf(temp, "%d", i + 1);
		strcat(fpath, temp);
		strcat(fpath, ".txt");
		int fd = open(fpath, O_RDWR | O_CREAT, 0777);
		close(fd);
	}
}

int menu() {
	int op;
	while (1) {

		printf("\n============================\n");
		printf("\n<Luxury PC Manager>\n\n");
		printf("1) Charge time \n");
		printf("2) Terminate PC\n");
		printf("3) Check pc usage \n");
		printf("4) Check total sales \n");
		printf("5) Exit \n");
		printf("\n============================\n");
		printf("Choice menu : ");
		scanf("%d", &op);
		if (op == 1 || op == 2 || op == 3 || op == 4 || op == 5) {
			break;
		}
		else {
			printf("※ Invalid input. Typed only 1,2,3,4,5..");
		}
	}
	return op;
}

int seat_assignment()
{
	int i;
	for (i = 0; i < ComNum; i++)
	{
		if (pcid[i] == -1) // 초기화 되어있으면 (==비어있으면)
		{
			return i;
		}
		else if (kill(pcid[i], 0) == 0) //존재 : 0
		{
			continue;
		}
		else {
			return i;
		}
	}
	return -1; // 남는 자리를 찾지 못했을 때
}

int kill_pc(int index)
{
	if (pcid[index] == -1) {
		return -1;	//프로세스 존재x
	}
	if (kill(pcid[index], 0) == 0) {	//프로세스가 존재하면(alive 하면)
		if (kill(pcid[index], 9) != -1)
		{
			pcid[index] = -1;
			return 0;	//정상적인 강제종료
		}
		else {
			return 1;	//비정상적 종료
		}
	}
	else {
		//존재x
		return -1;
	}

	return -1;	//에러
}

int readline(int fd, char *buf, int nbyte) {
	int numread = 0;
	int returnval;
	while (numread < nbyte - 1) {
		returnval = read(fd, buf + numread, 1);
		if ((returnval == -1) && (errno == EINTR)) continue;
		if ((returnval == 0) && (numread == 0)) return 0;
		if (returnval == 0) break;
		if (returnval == -1) return -1;
		numread++;
		if (buf[numread - 1] == '\n') {
			buf[numread] = '\0';
			return numread;
		}
	}
	errno = EINVAL;
	return -1;
}

void showpcinfo(int index)
{
	int fd;
	char PATH[BUFSIZE] = "./pc";
	char temp[5];
	char buf[BUFSIZE];
	sprintf(temp, "%d", index + 1);
	strcat(PATH, temp);
	strcat(PATH, ".txt");
	fd = open(PATH, O_RDONLY);
	int rv;	//return value
	printf(" Access Time / User ID / Hour / Money(Won) \n\n");
	while ((rv = readline(fd, buf, sizeof(buf))) != -1) {
		if (rv == 0) {
			break;
		}
		printf("%s", buf);
	}
	close(fd);
}

void show_total()
{
	int i,j, k, rv;
	int total = 0;
	for (i = 0; i < ComNum; i++) {
		int one_total = 0;
		char fPath[BUFSIZE] = "./pc";
		char pcnum[5] = { 0 };
		sprintf(pcnum, "%d", (i + 1));
		strcat(fPath, pcnum);
		strcat(fPath, ".txt");
		int fd;
		char* arr[4] = { NULL, };
		if ((fd = open(fPath, O_RDONLY)) != -1) {
			char buf[BUFSIZE];
			while ((rv=readline(fd, buf, sizeof(buf))) != -1) {
				if (rv == 0) {
					printf("----------------\n"); break;
				}
				j = 0;
				char* tok = strtok(buf, "/");
				while (1)               // 자른 문자열이 나오지 않을 때까지 반복
				{
					arr[j] = tok;
					j++;
					tok = strtok(NULL, "/");      // 다음 문자열을 잘라서 포인터를 반환
					if (tok == NULL) break;
				}
				//arr[4]가 도임
				one_total += atoi(arr[3]);
			}
			printf("pc%d 매출 : %d원\n", i+1, one_total);
			total += one_total;
			close(fd);
		}
		else {
			printf("Error: File open failed\n");
		}
	}
	printf("\n=> Total : %d won\n", total);

}

void clean_up() {
	int i;
	for (i = 0; i < ComNum; i++) {
		char PATH[BUFSIZE] = "./pc";
		char temp[5] = { 0 };
		sprintf(temp, "%d", (i + 1));
		strcat(PATH, temp);
		strcat(PATH, ".txt");
		remove(PATH);	//프로그램 종료시 txt 파일 다 삭제
	}
}

int is_time(char* time) {
	if (atoi(time) <= 0) {
		printf("※ Input only positive number. \n");
		return -1;
	}
	for (int i = 0; i < strlen(time); i++) {

		if (time[i] == '.') {
			printf("※  Input only hours.\n");
			return -1;
		}
	}
	return 1;
}

void regiinfo(char *uid, char *ctime, char *check) {
	printf("User Id :\t");
	scanf("%s", uid);
	while (1) {
		printf("Charging Time (hour) :\t");
		scanf("%s", ctime);
		if (is_time(ctime) != -1) break;
		else continue;
	}
	//getchar();
	printf("Did you charge? (Y/N) :\t");
	scanf("%s", check);
}


int main(void)
{
	atexit(clean_up);	//끝날 때 열고있는 파일 다 닫기
	makeFile(); // 파일 만들기
	int i;
	for (i = 0; i < ComNum; i++) // pcid 배열 초기화
	{
		pcid[i] = -1;
	}
	int totalsales = 0;
	while (1) {
		int op = menu();
		int input = 0;
		char successstr[5];
		int success;
		pid_t pid;
		int status;	//wait의 아웃풋 파라미터
		char uid[BUFSIZE], ctime[BUFSIZE], check[BUFSIZE]; //아이디, 충전시간, 계산 여부
		switch (op) {
		case 1:
			//좌석배정
			printf("\n");
			if ((success = seat_assignment()) == -1)
			{
				printf("-> There is no seat. Please wait.\n");
			}
			else
			{
				printf("-> Your pc is number %d.\n", (success + 1));

				regiinfo(uid, ctime, check);
				if (!strcmp(check, "Y") || !strcmp(check, "y")) {

					sprintf(successstr, "%d", success + 1);
					if ((pcid[success] = fork()) == -1) {
						printf("Error : Fork Error\n");
						exit(1);
					}
					else if (pcid[success] == 0) {	//자식프로세스
						execl("./PC", "PC", successstr, uid, ctime, (char *)0);	//PC 번호 넘겨줌
					}

				}
				else { printf("-> Please Charge\n"); }
			}
			break;
		case 2:
			//kill(강제종료)
			//돌고있는 pc프로세스 id출력
			printf("\n");
			for (i = 0; i < ComNum; i++) {
				printf("#PC%d id : %d\n", (i + 1), pcid[i]);
			}
			printf("\n<Executing pc id>\n");

			if ((pid = fork()) == -1) {
				printf("Error : Fork Error\n");
				exit(1);
			}
			else if (pid == 0) {	//자식 프로세스면
				execlp("ps", "ps", (char *)0);
			}
			while (wait(&status) != pid);

			printf("\nNumber of pc to terminate : ");
			scanf("%d", &input);
			if (input > ComNum && input <= 0)
			{
				printf("-> No exist.\n");
			}
			else
			{
				if ((success = kill_pc(input - 1)) == 0)
				{
					printf("-> PC %d terminated normally.\n", input);
				}
				else if (success == 1)
				{
					printf("-> PC %d terminated abnormally.\n", input);
				}
				else {
					printf("-> PC %d do not execute.\n", input);
				}
			}
			break;
		case 3:
			//PC 사용내역 (번호입력) txt파일 열어주기
			printf("\nNumber of pc to check usage  : ");
			scanf("%d", &input);
			printf("\n");
			if (input > ComNum && input <= 0)
				printf("-> No exist.\n");
			else
				showpcinfo(input - 1);
			break;
		case 4:
			//총매출확인
			printf("\n");
			show_total();
			break;
		case 5:
			printf("\nExit Program.\n");
			exit(1);
			break;
		}


	}
	return 0;
}