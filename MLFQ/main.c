#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HEAD_SIZE       4
#define TIME_QUANTUM    20

typedef struct priority_queue *queue_pointer;
struct priority_queue
{
    int priority;
    int process_id;
    int computing_time;
    int remaining_time;
    queue_pointer left_link, right_link;
};

typedef struct queue_head
{
    queue_pointer left_link;
    queue_pointer right_link;
} * head_pointer;

// 모든 우선순위 큐를 관리하는 head_pointer 의 ARRAY
head_pointer head_array[HEAD_SIZE];

// 큐의 사이즈를 저장하는 전역 array
int queue_sizes[HEAD_SIZE] = {0,};
int turn_around = 0;

// 필요한 Operation 함수들
void initialize_queue(void);
int insert_node(int, int, int);
queue_pointer delete_node(int);
void process_pop(void);
void print_queue(queue_pointer);
// 실질적 삽입과 삭제를 시행하는 함수
void _insert(queue_pointer qp, int priority_id);
queue_pointer _delete(int priority_id, int priority);

void initialize_queue(void)
{
    printf("INITIALIZE\n");
    for(int i = 0; i < HEAD_SIZE; i++){
        head_array[i] = malloc(sizeof(struct queue_head));
        if(head_array[i] == NULL){
            perror("Error during queue malloc()\n");
            exit(0);
        }
        head_array[i]->left_link = NULL;
        head_array[i]->right_link = NULL;
    }
}

// qp 동적할당된 큐 노드를 우선순위에 따라서 큐 스케줄에 추가
// CPU 가 처리할 새로운 프로세스를 스케줄에 추가하는 것을 의미
void _insert(queue_pointer qp, int priority_id){
    queue_pointer current_node;
    int current_iteration = 0;
    if(head_array[priority_id]->right_link == NULL){
        head_array[priority_id]->right_link = qp;
        head_array[priority_id]->left_link = qp;
        queue_sizes[priority_id]++;
    }
    else{
        current_node = head_array[priority_id]->right_link;

        while(current_node->priority <= qp->priority && current_iteration != queue_sizes[priority_id]){
            current_node = current_node->right_link; // 1 -> 8 -> 9
            current_iteration++; // 0 -> 1 -> 2
        }

        
        if(current_iteration == queue_sizes[priority_id]){
            // 맨 뒤에 들어가는 우선순위다
            // printf("BACK\n");
            current_node->right_link = qp;
            qp->left_link = current_node;


        } else if(current_iteration == 0){
            // 첫 번째 들어가는 우선순위다
            // printf("FRONT\n");
            head_array[priority_id]->right_link = qp;
            current_node->left_link = qp;
            qp->right_link = current_node;
        }
        else{
            // 어느 노드 중간에 들어가는 우선순위다
            // printf("MID\n");
            current_node->left_link->right_link = qp;
            qp->left_link = current_node->left_link;
            current_node->left_link = qp;
            qp->right_link = current_node;
        }
        queue_sizes[priority_id]++;
    }
}

int insert_node(int priority,int process_id, int computing_time)
{
    queue_pointer qp = malloc(sizeof(struct priority_queue));
    qp->process_id = process_id;
    qp->priority = priority;
    qp->computing_time = computing_time;
    qp->remaining_time = computing_time;
    qp->left_link = qp;
    qp->right_link = qp;
    if(priority <= 10){
        _insert(qp,0);
        
    } else if(priority <= 20){
        _insert(qp,1);
    } else if(priority <= 30){        
        _insert(qp,2);
    } else{
        _insert(qp,3);
    }
    return 0;
}

// 큐 우선순위 삭제 실행 및 삭제 된 노드 value 출력
// CPU가 처리한 프로세스를 의미
queue_pointer _delete(int priority_id, int priority){
    queue_pointer current_node;
    
    current_node = head_array[priority_id]->right_link;
    if(current_node->priority <= priority){
        
    }
    head_array[priority_id]->right_link = current_node->right_link;
    return current_node;
}

queue_pointer delete_node(int priority)
{
    queue_pointer qp;
    if(priority <= 10){
        qp = _delete(0, priority);
    } else if(priority <= 20){
        qp = _delete(1, priority);
    } else if(priority <= 30){
        qp = _delete(2, priority);
    } else {
        qp = _delete(3, priority);
    }

    return qp;
}

// head queue 가 비어 있는지 확인하고 할당된 노드가 있는 head의 id 반환
int _check_empty_head(){
    int priority_id = 0;
    while(head_array[priority_id]->right_link==NULL){
        priority_id++;
    }
    printf("HEAD ARRAY ID : %d\n", priority_id);
    return priority_id;
}

// 파일에서 입력된 type 값이 1일때 실행 중인 프로세스를 처리하는 함수
void process_pop(){
    int priority_id = _check_empty_head();
    queue_pointer process = head_array[priority_id]->right_link;
    queue_pointer finished_process;
    // printf("Process ID : %d Priority %d Computing Time %d\n", process->process_id, process->priority, process->computing_time);
    if(process->remaining_time >= TIME_QUANTUM){
        priority_id++;
        process->remaining_time = process->computing_time - TIME_QUANTUM;
        if(process->priority + 10 > 30){
            process->priority = 31;
        }else{
            process->priority = process->priority + 10;
        }
        finished_process = delete_node(process->priority);
        _insert(process, priority_id);
        print_queue(finished_process);
    } else {
        finished_process = delete_node(process->priority);
        print_queue(finished_process);
    } 
}

void print_queue(queue_pointer qp){
    // 우선순위 큐에 남아 있는 value 출력
    queue_pointer current_node;
    printf("\n\n");
    printf("_________________________________________________________________\n");
    printf("Queue ID \t Priority \t Computing Time \t Turn Around Time\n");
    printf("_________________________________________________________________\n");
    
    printf("%d \t\t %d \t\t %d \t\t %d\n", qp->process_id, qp->priority, qp->computing_time, qp->remaining_time);
}


void print_all(){
    // 우선순위 큐에 남아 있는 value 출력
    queue_pointer current_node;
    printf("\n\n");
    printf("___________________________________________________\n");
    printf("What is left inside Rest of Priority Queues\n");
    printf("Queue ID \t ProcessID \t Prority \t Computing Time \t Remaining Time\n");
    printf("___________________________________________________\n");
    for(int i = 0; i < HEAD_SIZE; i++){
        current_node = head_array[i]->right_link;
        if(current_node == NULL){
            continue;
        }
        while (1)
         {
            
            printf("%d \t\t %d \t\t %d \t\t\t %d \t\t %d\n", i+1, current_node->process_id, current_node->priority, current_node->computing_time, current_node->remaining_time);
            if (current_node->right_link->priority == current_node->priority)
            {
               printf("\n");
               break;
            }
            else
            {
                current_node = current_node->right_link;
            }
         }
    }

}


int main(int argc, char* argv[])
{
    FILE *fp;
    char buffer[20];
    char file_content;

    int type, priority, process_id, computing_time;

    const char* INPUT_FILE = argv[1];

    fp = fopen(INPUT_FILE, "r");
    if (fp == NULL)
    {
        perror("File read error.\n");
        exit(EXIT_FAILURE);
    }

    // 초기화 head_pointer array
    initialize_queue();

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (!strcmp(buffer, "-1"))
        {
            break;
        }

        // 파일 내용 토큰화 
        // type             (0) insert, (1) ended process
        // process_id       프로세스의 id
        // priority         우선 순위 value
        // computing_time   computing_time
        char *token = strtok(buffer, " ");
        type = atoi(token);
        token = strtok(NULL, " ");
        process_id = atoi(token);
        token = strtok(NULL, " ");
        priority = atoi(token);
        token = strtok(NULL, " ");
        computing_time = atoi(token);
        
        switch (type)
        {
            case 0:
                // printf("Inserting to Queue\n");
                print_all();
                insert_node(priority, process_id, computing_time);
                break;
            case 1:
                // printf("Process Time Exceeded : Process ID[%d]\n", process_id);
                print_all();
                process_pop();
                break;
            default:
                perror("You have messed something up, dude...");
                break;
        }
        // printf("%d, %d, %d\n", type, priority, computing_time);
    }


    for(int i = 0; i < 3; i++){
        free(head_array[i]);
    }
    return 0;
}