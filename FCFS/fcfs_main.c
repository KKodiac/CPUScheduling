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

	initialize_queue(); // ť �ʱ�ȭ

	//File open
	stream = fopen("input_file.txt", "r"); // ���� ����
	if (stream == NULL)
		printf("���� ���� ����\n");

	printf("%-12s%-10s%-10s%-16s%-16s\n", "Process_id", "Queue_id", "priority", "computing_time", "turn_around time");

	while (1) {
		fscanf(stream, "%d %d %d %d", &type, &process_id, &priority, &computing_time); //���μ��� ������ �б�
		switch (type) { // Ÿ�Կ� ����
		case 0: // Ÿ���� 0�ΰ��			
			insert_queue(process_id, priority, computing_time); // ť�� �߰�
			count = count + 1;
			break;
		case 1:	// Ÿ���� 1�ΰ��
			count = count + 1;
			time[j] = count - j;
			j++;

			break;
		case -1: // Ÿ���� -1�ΰ��
			scheduling(); // ť�� �ִ� ���μ��� �����층

			break;
		default:
			printf("type�� ����\n");
			break;
		}

		if (type == -1) // Ÿ���� -1�ΰ�� �Է� ����
			break;
	}

	final_ntt = ntt_sum / 100;

	printf("\n(First Come First Served Algorithm) normalized turn_around_time ->  %.2f \n", final_ntt);
	file_state = fclose(stream); // ���� �ݱ�

	if (file_state == EOF)
		printf("���� �ݱ� ����\n");

	return 0;
}

/* ť �ʱ�ȭ �Լ�*/
void initialize_queue() {
	int i;
	for (i = 0; i < 4; i++) {
		ready_queue[i].head = NULL;
		ready_queue[i].tail = NULL;
		ready_queue[i].size = 0;
	}
}

int insert_queue(int process_id, int priority, int computing_time) { //type = 0 �϶� queue ����

	int qLevel = 0;

	/* ���μ��� ���� �� ������ �Է� */
	process* p = (process*)malloc(sizeof(process));
	p->sequence = seq++;
	p->process_id = process_id;
	p->priority = priority;
	p->computing_time = computing_time;
	p->next = NULL;

	//����ó��
	if (process_id < 0) {
		printf("process_id�� ����\n");
		return 0;
	}

	if (computing_time < 0) {
		printf("computing_time�� ����\n");
		return 0;
	}

	qLevel = check_qLevel(p->priority);

	if (qLevel == -1) {		//Real_time process
		real_timeRunning(p);
		return 0;
	}
	else if (qLevel == -2) {	//-2�Է½� ����
		printf("priority�� ����. -10~-1, 1~31�� �� �Է°���\n");
		return 0;
	}

	if (ready_queue[qLevel].head == NULL) { // ť�� ��� ������, head�� tail�� ����
		ready_queue[qLevel].head = p;
		ready_queue[qLevel].tail = p;
	}
	else { // ť�� ������� ������, ť ������ ��ġ�� ����
		ready_queue[qLevel].tail->next = p;
		ready_queue[qLevel].tail = p;
	}
	ready_queue[qLevel].size++; // ť ũ�� ����

	return 0;
}

/*��� ť�� ��� �ִ��� �˻�*/
int is_all_queue_empty() {

	if (ready_queue[0].head == NULL && ready_queue[1].head == NULL && ready_queue[2].head == NULL && ready_queue[3].head == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

int scheduling() { //type=1 �϶�

	process* p;


	int minSeq = INT_MAX;
	int minSeqQue;
	int i;
	//count = 0;
	int k = 1;
	while (!is_all_queue_empty()) { // ��� ť�� ���μ����� �������� ���������� �ݺ�

		minSeq = INT_MAX;

		//count++;

		/*ready_queue �߿��� ���� ���� ���μ����� �ִ� ť�� index�� ����*/
		for (i = 0; i < 4; i++) {
			if (ready_queue[i].head != NULL) {
				if (ready_queue[i].head->sequence < minSeq) {
					minSeq = ready_queue[i].head->sequence;
					minSeqQue = i;
				}
			}
		}

		p = ready_queue[minSeqQue].head; // p�� ���� ���� ���μ����� �����ִ� ť�� ù��° ���μ���
		ready_queue[minSeqQue].head = ready_queue[minSeqQue].head->next; // ť�� �������� �������� �̵�



		OStime += p->computing_time; // ostime�� ���μ��� ���۽ð��� ������






		printf("%-12dQ%-9d%-10d%-16d%-16d\n", p->process_id, minSeqQue + 1, p->priority, p->computing_time, OStime); // ���� ���μ��� ������ ��� ���
		//printf("count�� ��: %d\n", count);
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

		free(p); // �������μ��� �޸� ����
		ready_queue[minSeqQue].size--; // ť ũ�� ����
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
