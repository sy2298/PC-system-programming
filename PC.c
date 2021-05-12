#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 100
#include<stdio.h>
#include <stdlib.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<unistd.h>

char *check_time(void) {
	static char getTime[BUFSIZE] = "";
	char tmp[40] = "";
	time_t timer = time(NULL); // 현재 시각을 초 단위로 얻기
	struct tm *t = localtime(&timer);
	//printf("%d-%d-%d %d:%d ", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min);
	sprintf(tmp, "%d", t->tm_year + 1900);
	strcat(getTime, tmp);
	strcat(getTime, "-");
	sprintf(tmp, "%d", t->tm_mon + 1);
	strcat(getTime, tmp);
	strcat(getTime, "-");
	sprintf(tmp, "%d", t->tm_mday);
	strcat(getTime, tmp);
	strcat(getTime, " ");
	sprintf(tmp, "%d", t->tm_hour);
	strcat(getTime, tmp);
	strcat(getTime, ":");
	sprintf(tmp, "%d", t->tm_min);
	strcat(getTime, tmp);
	strcat(getTime, "/");
	return getTime;
}

void make_string(char *buf, char *stime, char *uid, char *ctime) {
	int charge = atoi(ctime) * 1000;
	char temp[BUFSIZE] = { 0 };
	strcat(buf, stime);
	strcat(buf, uid);
	strcat(buf, "/");
	strcat(buf, ctime);
	strcat(buf, "/");
	sprintf(temp, "%d", charge);
	strcat(buf, temp);
	strcat(buf, "\n");
}

void time_up(char *ctime) {
	clock_t start, end;
	long int timecheck;
	start = clock();
	timecheck = atoi(ctime);
	while (clock() - start < timecheck * 10000000);
	exit(0);
}

int main(int argc, char* argv[]) {
	char uid[BUFSIZE], ctime[BUFSIZE];//계산 여부
	strcpy(uid, argv[2]);
	strcpy(ctime, argv[3]);

	int fd;
	char buf[BUFSIZE] = { 0 };//buf
	char fname[BUFSIZE] = "./pc";
	char *stime;
	strcat(fname, argv[1]);
	strcat(fname, ".txt");
	stime = check_time();	//PC 사용시간
	make_string(buf, stime, uid, ctime);

	if ((fd = open(fname, O_RDWR|O_APPEND)) >= 0)
	{
		write(fd, buf, strlen(buf));
		close(fd);
	}
	else printf("No File\n");
	time_up(ctime);
}