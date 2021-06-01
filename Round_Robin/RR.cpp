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
// 다음에 실행할 프로세스 선정
queue_pointer select_process() {
	i = head->right_link;
	// 이미 끝난 프로세스는 생략
	while (finished[idx]) {
		idx = idx + 1 > max_idx ? 1 : ++idx;
	}
	// 해당 번호의 프로세스에 접근
	while (i->processID != idx) {
		i = i->right_link;
	}
	return i;
}
// 실행 완료된 프로세스를 큐에서 삭제
void delete_queue(int processID) {
	i = head->right_link;
	while (i->processID != processID) {
		i = i->right_link;
	}
	i->left_link->right_link = i->right_link;
	i->right_link->left_link = i->left_link;
}
// 프로세스 실행
void run_process() {
	i = select_process();
	// time_quantum 시간 안에 프로세스를 완료할 수 없을 경우
	if (i->remaining_time - time_quantum > 0) {
		i->remaining_time -= time_quantum;
		current_time += time_quantum;
	}
	// time_quantum 시간 안에 프로세스를 완료했을 경우
	else {
		current_time += (i->remaining_time);
		i->remaining_time = 0;
		i->turn_around_time = current_time - (i->input_time);
		sum += i->turn_around_time / i->computing_time;
		printf("%d\t\t%d\t\t%d\t\t%d\n", i->processID, i->priority, i->computing_time, i->turn_around_time);
		// 완료된 프로세스 체크
		finished[i->processID] = 1;
		// 완료된 프로세스를 큐에서 삭제
		delete_queue(i->processID);
		// 완료한 프로세스 수 증가
		++cnt;
	}
	idx = idx + 1 > max_idx ? 1 : ++idx;
	return;
}
// 큐 끝에 노드를 삽입하는 함수 구현
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
	// 큐 초기화
	initialize_queue();
	// 파일 입출력 함수
	FILE* fp;
	if ((fp = fopen("D\:example.txt", "r")) == NULL) {
		printf("파일 읽기 오류! \n");
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
	// 남은 프로세스들 처리
	while (cnt != max_idx) {
		run_process();
	}
	double tr = sum / max_idx;
	printf("(Round-Robin-Algorithm) normalized turn_around time -> %lf\n", sum / max_idx);
	return 0;
}