#include <stdio.h>
#include <stdlib.h>
#define max(a,b)  (((a) > (b)) ? (a) : (b))
int time_quantum = 20, input_time = 0, current_time = 0, idx = 1, max_idx = -1, cnt = 0;
double sum = 0;
bool finished[11];
typedef struct priority_queue* queue_pointer;
struct priority_queue {
	int processID;
	int priority;
	int computing_time;
	int remaining_time;
	int input_time;
	int turn_around_time;
	queue_pointer left_link, right_link;
}priority_queue;
queue_pointer head, tail, i;
// ������ ������ ���μ��� ����
queue_pointer select_process() {
	i = head->right_link;
	// �̹� ���� ���μ����� ����
	while (finished[idx]) {
		idx = idx + 1 > max_idx ? 1 : ++idx;
	}
	// �ش� ��ȣ�� ���μ����� ����
	while (i->processID != idx) {
		i = i->right_link;
	}
	return i;
}
// ���� �Ϸ�� ���μ����� ť���� ����
void delete_queue(int processID) {
	i = head->right_link;
	while (i->processID != processID) {
		i = i->right_link;
	}
	i->left_link->right_link = i->right_link;
	i->right_link->left_link = i->left_link;
}
// ���μ��� ����
void run_process() {
	i = select_process();
	// time_quantum �ð� �ȿ� ���μ����� �Ϸ��� �� ���� ���
	if (i->remaining_time - time_quantum > 0) {
		i->remaining_time -= time_quantum;
		current_time += time_quantum;
	}
	// time_quantum �ð� �ȿ� ���μ����� �Ϸ����� ���
	else {
		current_time += (i->remaining_time);
		i->remaining_time = 0;
		i->turn_around_time = current_time - (i->input_time);
		sum += i->turn_around_time / i->computing_time;
		printf("%d\t\t%d\t\t%d\t\t%d\n", i->processID, i->priority, i->computing_time, i->turn_around_time);
		// �Ϸ�� ���μ��� üũ
		finished[i->processID] = 1;
		// �Ϸ�� ���μ����� ť���� ����
		delete_queue(i->processID);
		// �Ϸ��� ���μ��� �� ����
		++cnt;
	}
	idx = idx + 1 > max_idx ? 1 : ++idx;
	return;
}
// ť ���� ��带 �����ϴ� �Լ� ����
void insert_queue(int processID, int priority, int computing_time) {
	max_idx = max(max_idx, processID);
	queue_pointer i, element;
	i = head->right_link;
	while (i != tail) {
		i = i->right_link;
	}
	element = (queue_pointer)malloc(sizeof(priority_queue));
	element->processID = processID;
	element->priority = priority;
	element->computing_time = computing_time;
	element->input_time = input_time;
	element->remaining_time = computing_time;
	i->left_link->right_link = element;
	element->left_link = i->left_link;
	i->left_link = element;
	element->right_link = i;
}
void initialize_queue() {
	head = (queue_pointer)malloc(sizeof(priority_queue));
	tail = (queue_pointer)malloc(sizeof(priority_queue));
	head->right_link = tail;
	head->left_link = tail;
	tail->right_link = head;
	tail->left_link = head;
}
int main() {
	// ť �ʱ�ȭ
	initialize_queue();
	// ���� ����� �Լ�
	FILE* fp;
	if ((fp = fopen("D\:example.txt", "r")) == NULL) {
		printf("���� �б� ����! \n");
		return 0;
	}
	int a, b, c, d;
	printf("Process_id\tpriority\tcomputing_time\tturn_around_time\n");
	while (fscanf(fp, "%d", &a) && a != -1) {
		fscanf(fp, "%d", &b);
		fscanf(fp, "%d", &c);
		fscanf(fp, "%d", &d);
		if (!a) {
			insert_queue(b, c, d);
		}
		else {
			while (current_time < input_time + time_quantum) {
				run_process();
			}
			input_time += time_quantum;
		}
	}
	// ���� ���μ����� ó��
	while (cnt != max_idx) {
		run_process();
	}
	double tr = sum / max_idx;
	printf("(Round-Robin-Algorithm) normalized turn_around time -> %lf\n", sum / max_idx);
	return 0;
}