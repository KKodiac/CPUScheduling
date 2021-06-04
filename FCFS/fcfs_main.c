#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <malloc.h>

#define MAX_SIZE 20
#define INT_MAX  2147483647
#define TIME_QUANTUM 20 
typedef struct process {
	int sequence;
	int process_id;
	int priority;
	int computing_time;
	struct process* next;
}process;

typedef struct queue {
	process* head;
	process* tail;
	int size;
}queue;

void initialize_queue();
int insert_queue(int, int, int);
int scheduling();
int check_qLevel(int);
void time_quantum();

queue ready_queue[4];
int seq = 0;

int count = 0;
int time[10] = { 0, };
int j = 1;

float ntt;
float ntt_sum;
float final_ntt;
unsigned int OStime = 0;

int main() {
	FILE* stream;
	int file_state;
	int type = 0, process_id = 0, priority = 0, computing_time = 0;

	initialize_queue(); // 큐 초기화

	//File open
	stream = fopen("input_file.txt", "r"); // 파일 열기
	if (stream == NULL)
		printf("파일 열기 에러\n");

	printf("%-12s%-10s%-10s%-16s%-16s\n", "Process_id", "Queue_id", "priority", "computing_time", "turn_around time");

	while (1) {
		fscanf(stream, "%d %d %d %d", &type, &process_id, &priority, &computing_time); //프로세스 데이터 읽기
		switch (type) { // 타입에 따라서
		case 0: // 타입이 0인경우			
			insert_queue(process_id, priority, computing_time); // 큐에 추가
			count = count + 1;
			break;
		case 1:	// 타입이 1인경우
			count = count + 1;
			time[j] = count - j;
			j++;

			break;
		case -1: // 타입이 -1인경우
			scheduling(); // 큐에 있는 프로세스 스케쥴링

			break;
		default:
			printf("type값 오류\n");
			break;
		}

		if (type == -1) // 타입이 -1인경우 입력 종료
			break;
	}

	final_ntt = ntt_sum / 100;

	printf("\n(First Come First Served Algorithm) normalized turn_around_time ->  %.2f \n", final_ntt);
	file_state = fclose(stream); // 파일 닫기

	if (file_state == EOF)
		printf("파일 닫기 에러\n");

	return 0;
}

/* 큐 초기화 함수*/
void initialize_queue() {
	int i;
	for (i = 0; i < 4; i++) {
		ready_queue[i].head = NULL;
		ready_queue[i].tail = NULL;
		ready_queue[i].size = 0;
	}
}

int insert_queue(int process_id, int priority, int computing_time) { //type = 0 일때 queue 삽입

	int qLevel = 0;

	/* 프로세스 생성 및 데이터 입력 */
	process* p = (process*)malloc(sizeof(process));
	p->sequence = seq++;
	p->process_id = process_id;
	p->priority = priority;
	p->computing_time = computing_time;
	p->next = NULL;

	//예외처리
	if (process_id < 0) {
		printf("process_id값 오류\n");
		return 0;
	}

	if (computing_time < 0) {
		printf("computing_time값 오류\n");
		return 0;
	}

	qLevel = check_qLevel(p->priority);

	if (qLevel == -1) {		//Real_time process
		real_timeRunning(p);
		return 0;
	}
	else if (qLevel == -2) {	//-2입력시 오류
		printf("priority값 오류. -10~-1, 1~31의 값 입력가능\n");
		return 0;
	}

	if (ready_queue[qLevel].head == NULL) { // 큐가 비어 있으면, head와 tail에 연결
		ready_queue[qLevel].head = p;
		ready_queue[qLevel].tail = p;
	}
	else { // 큐가 비어있지 않으면, 큐 마지막 위치에 연결
		ready_queue[qLevel].tail->next = p;
		ready_queue[qLevel].tail = p;
	}
	ready_queue[qLevel].size++; // 큐 크기 증가

	return 0;
}

/*모든 큐가 비어 있는지 검사*/
int is_all_queue_empty() {

	if (ready_queue[0].head == NULL && ready_queue[1].head == NULL && ready_queue[2].head == NULL && ready_queue[3].head == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int scheduling() { //type=1 일때

	process* p;


	int minSeq = INT_MAX;
	int minSeqQue;
	int i;
	//count = 0;
	int k = 1;
	while (!is_all_queue_empty()) { // 모든 큐에 프로세스가 존재하지 않을때까지 반복

		minSeq = INT_MAX;

		//count++;

		/*ready_queue 중에서 먼저 들어온 프로세스가 있는 큐의 index를 구함*/
		for (i = 0; i < 4; i++) {
			if (ready_queue[i].head != NULL) {
				if (ready_queue[i].head->sequence < minSeq) {
					minSeq = ready_queue[i].head->sequence;
					minSeqQue = i;
				}
			}
		}

		p = ready_queue[minSeqQue].head; // p는 먼저 들어온 프로세스가 속해있는 큐의 첫번째 프로세스
		ready_queue[minSeqQue].head = ready_queue[minSeqQue].head->next; // 큐의 시작점을 다음으로 이동



		OStime += p->computing_time; // ostime에 프로세스 동작시간을 더해줌






		printf("%-12dQ%-9d%-10d%-16d%-16d\n", p->process_id, minSeqQue + 1, p->priority, p->computing_time, OStime); // 현재 프로세스 스케쥴 결과 출력
		//printf("count의 값: %d\n", count);
		if (p->process_id == time[k])
		{

			OStime = OStime + TIME_QUANTUM;
			k++;

		}


		for (int i = 0; i < 10; i++) {

			ntt = (double)OStime / (double)p->computing_time;
			ntt_sum = ntt_sum + ntt;
		}
		final_ntt = ntt_sum / 10;

		free(p); // 현재프로세스 메모리 해제
		ready_queue[minSeqQue].size--; // 큐 크기 감소
	}


	return 0;
}

int real_timeRunning(process* p) { //real time queue id
	OStime += p->computing_time;
	printf("%-12d%-10s%-10d%-16d%-16d\n", p->process_id, "real", p->priority, p->computing_time, OStime);
	return 0;
}

int check_qLevel(int priority) {
	if (priority >= -10 && priority <= -1)
		return -1;
	else if (priority >= 1 && priority <= 31)
		return (priority - 1) / 10;
	else
		return -2;
}
