/*
    Copyright 따위는 없습니다. 
    홍성민(https://github.com/KKodiac) 
    이름이라도 띄워보겠습니다.

            <<< Multi-Level Feedback Queue >>>
    CPU Scheduler 중 하나 입니다. 
    목적 : 
        1. 짧은 작업을 먼저 실행시켜, 반환시간을 최적화 합니다.
        2. 사용자와 대화하는 대화형 프로세스를 우선으로 처리해, 응답시간을 최적화 합니다.
    기본 규칙 : 
        1. 여러개의 우선순위 큐로 구성됩니다. 각 큐마다 상위에서 하위 우선순위가 배정됩니다.
        2. 각 큐에는 여러 작업이 존재할 수 있고, Round Robin 스케줄링을 사용해 처리합니다.
        3. 새 작업(프로세스)가 도착하면 가장 높은 우선순위 큐에 배정됩니다.
        4. 작업 시간을 소진하면, Time Quantum 이 만료되면, 한 단계 낮은 우선순위 큐에 재배정됩니다.
        5. 각 큐마다 TIME QUANTUM을 다르게 둔다. 
                높은 우선순위는 작은 TIME QUANTUM
            5-1. 처리시간이 짧은 프로세스(대화형)는 응답시간을 더욱 최적화할 수 있다.
                낮은 우선순위는 큰 TIME QUANTUM
            5-2. 처리시간이 긴 CPU중심 프로세스들은 긴 TIME QUANTUM 이 적합하다.
        6. VOODOO_CONSTANT 을 넘어서면 하위 우선순위 큐에 있는 작업들은 가장 높은 우선순위 큐에 넣어진다.
            기아 현상을 방지하기 위함이다.
    스케줄러는 작업이 짧은 작업인지, 긴 작업인지 알 수 없기 때문에 일단 짧은 작업이라고 가정하여 높은 우선순위 부여합니다.
    진짜 짧은 작업이면 빨리 실행되고 바로 종료되고, 
    짧은 작업이 아니라면, 천천히 아래 큐로 이동하게 되고 스스로 긴 배치형 작업이라는 것을 증명하게 됩니다.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 전체 head_array의 큐의 갯수
#define HEAD_QUEUE_SIZE     4
// 기아 현상을 방지하기 위한 상수, 해당 값 이상을 돌았으면, Boosting 을 해주어 모든 프로세스 노드를 
// 가장 높은 우선순위 큐에 재배치한다.
#define VOODOO_CONSTANT     10
// 각 head_array의 큐마다 적용되는 Time Quantum의 값
const int TIME_SLICE[HEAD_QUEUE_SIZE] = {10,20,25,30};
int Game_Tolerance[HEAD_QUEUE_SIZE] = {10,20,25,30};
// 누적되는 Turn Around Time 을 유지관리한다.
int processing_time = 0;
int total_turnaround = 0;
// 전체 프로세스가 끝났는지 확인하는 flag 변수
int IS_CPU_WORK_OVER = 0;

typedef struct process *queue_pointer;
struct process
{
    int process_id;     // PID
    int priority;       // 우선순위 큐 삽입을 위한 값
    int process_priority; // 프로세스의 우선순위, 쓰이지 않음
    int computing_time; // 프로세스 총 실행 시간
    int remaining_time; // 남은 프로세스 실행 시간
    int created_time;   // 프로세스가 생성된 시간
    queue_pointer left_link, right_link;
} process;

typedef struct queue_head
{
    queue_pointer tail;  
    queue_pointer head;
} * head_pointer;

// 모든 우선순위 큐를 관리하는 head_pointer 의 ARRAY
head_pointer head_array[HEAD_QUEUE_SIZE];

/*
    head_array 를 초기화 해준다.
    총 4의 길이의 head_array 를 동적할당 받아 생성한다.
*/
void initialize_head(void)
{
    printf("INITIALIZE\n");
    for(int i = 0; i < HEAD_QUEUE_SIZE; i++){
        head_array[i] = malloc(sizeof(struct queue_head));
        if(head_array[i] == NULL){
            perror("Error during head malloc()\n");
            exit(0);
        }
        head_array[i]->tail = NULL;
        head_array[i]->head = NULL;
    }
}

void free_head(void){
    for(int i = 0; i < HEAD_QUEUE_SIZE - 1; i++){
        free(head_array[i]);
    }
}

/*
    가장 높은 우선순위를 갖은 head_array 의 id를 반환한다.
    반환되는 값은 0 ~ 3 사이의 값이다.
*/
int _get_head_queue_id(){
    if(head_array[0]->head != NULL){
        return 0;
    } else if(head_array[1]->head != NULL){
        return 1;
    }else if(head_array[2]->head != NULL){
        return 2;
    }else{
        return 3;
    }
    
    printf("_get_head_queue_id() : Error => Delete operation should not occur with all empty heads!\n");
    exit(0);
}

/*
    프로세스 노드를 생성 및 반환한다.
*/
queue_pointer initialize_process(int process_id, int computing_time, int process_priority){
    queue_pointer new_process = (queue_pointer)malloc(sizeof(process));
    int tq = _get_head_queue_id();
    if(new_process==NULL){
        perror("Error during process malloc()\n");
        exit(0);
    }

    new_process->process_id = process_id;
    new_process->priority = 0;
    new_process->computing_time = computing_time;
    new_process->process_priority = process_priority;
    new_process->remaining_time = computing_time;
    new_process->created_time = processing_time;
    new_process->right_link = NULL;
    new_process->left_link = NULL;
    return new_process;
}


/*
    Turn Around Time 을 보여준다.
    각 큐의 현재 상태를 보여준다.
*/
void display_head(){
    printf("Turn Around T : %d\n", processing_time);
    printf("-------------------------------------------------------\n");
    printf("Queue ID \t Process ID \t Computing T \t Remaining T\n");
    printf("-------------------------------------------------------\n");
    for(int i = 0; i < HEAD_QUEUE_SIZE; i++){
        queue_pointer current_process = head_array[i]->head;    
        while(current_process != NULL){
            printf("%d \t\t %d \t\t %d \t\t %d \t\t\n", i+1, current_process->process_id, current_process->computing_time, current_process->remaining_time);
            current_process = current_process->right_link;
        }
    }
}

/*
    큐에서 완료된 프로세스의 정보를 보여준다.
        - queue_pointer finished_process 
            : head_array[head_id]에서 제거된 프로세스 노드
*/
void display_finished_process(queue_pointer finished_process){
    printf("Turn Around T : %d\n", processing_time);
    total_turnaround = (processing_time / finished_process->computing_time) + total_turnaround;
    printf("%d \t\t %d \t\t %d \t\t %d \n", finished_process->process_id, finished_process->process_priority, finished_process->computing_time, processing_time);
}

/*
    새로 생성된 프로세스 노드와 head_array의 인덱스(head_id)를 입력받는다.
    해당되는 head_array 의 가장 앞에 프로세스 노드를 삽입한다.
        - int head_id 
            : head_array 의 인덱스, 0 <= head_id < HEAD_QUEUE_SIZE
        - queue_pointer new_process
            : 새로 생성된 프로세스 노드
*/
void _insert_at_head(int head_id, queue_pointer new_process){
    queue_pointer head_process = head_array[head_id]->head;
    if(head_array[head_id]->head==NULL){
        head_array[head_id]->head = new_process;
        return;
    }
    
    head_process->left_link = new_process;
    new_process->right_link = head_process;
    head_array[head_id]->head = new_process;
}

/*
    한 번의 CPU할당이 완료되고 아직 computing_time 이 남아있는 프로세스 노드를 
    다음 크기의 인덱스인 head_array 에 삽입할 때 사용한다.
        - head_id 
            : head_array 의 인덱스, 0 <= head_id < HEAD_QUEUE_SIZE
        - queue_pointer new_process
            : 큐에 삽입될 프로세스 노드
*/
void _insert_at_tail(int head_id, queue_pointer new_process){
    queue_pointer current_process = head_array[head_id]->head;
    if(current_process==NULL){
        head_array[head_id]->head = new_process;
        return;
    }
    while(current_process->right_link != NULL){
        current_process = current_process->right_link;
    }
    current_process->right_link = new_process;
    new_process->left_link = current_process;    
}

/*
    새로 생성될 프로세스 노드를 생성 및 해당되는 head_array에 삽입한다.
        - int process_id
            : 새로 생성 될 프로세스 노드의 pid
        - int computing_time
            : 새로 생성 될 프로세스 노드의 compute time
*/
void insert_new_process(int process_id, int computing_time, int process_priority){    
    queue_pointer new_process = initialize_process(process_id, computing_time, process_priority);
    _insert_at_head(new_process->priority, new_process);
}

/*
    한 번 CPU 작업이 이루어지고 나서도 computing time 이 남아있는 프로세스 노드를
    한 단계 낮은 head_array에 삽입한다.
        - queue_pointer process
            : 한 단계 낮은 큐에 삽입될 프로세스 노드
*/
void insert_process_to_lower_queue(queue_pointer process){
    process->priority++;
    if(process->priority > HEAD_QUEUE_SIZE){
        process->priority = HEAD_QUEUE_SIZE - 1;
    }
    _insert_at_tail(process->priority, process);
}

/*
    최하에 위치한 프로세스 노드를 처리 할 시 사용되는 삽입 동작이다.
    Round Robin을 위해 사용된다.
        - queue_pointer process
            : 삽입될 프로세스 노드
*/
void insert_process_to_round_robin(queue_pointer process){
    if(process->priority > HEAD_QUEUE_SIZE){
        process->priority = HEAD_QUEUE_SIZE - 1;
    }
    _insert_at_tail(process->priority, process);
}

/*
    해당되는 head_array[head_id] 에서 가장 앞 프로세스 노드를 큐에서 제거하고 반환한다.
    CPU 작업을 완료된 프로세스 노드를 의미한다.
    제거된 프로세스 노드를 반환한다.
        - int head_id 
            : head_array 의 인덱스, 0 <= head_id < HEAD_QUEUE_SIZE
*/
queue_pointer delete_process(int head_id){
    queue_pointer current_process = head_array[head_id]->head;
    if(current_process == NULL){
        IS_CPU_WORK_OVER = 1;
        return NULL;
    }
    if(current_process->right_link == NULL){
        head_array[head_id]->head = NULL;
    }else{
        head_array[head_id]->head = current_process->right_link;
        current_process->right_link->left_link = NULL;
        current_process->right_link = NULL;
        current_process->left_link = NULL;

    }
    
    return current_process;
}

/*
    입력 파일에서 1 0 0 이 입력되면 실행된다.

    프로세스의 작업이 완료 되었다는 의미이며, 이 후 computing time 이 아직 남아있는 프로세스 노드는
    해당되는 head_array 큐에서 제거되고 한 단계 낮은 head_array 에 맨 뒤에 삽입된다.
    제거된 프로세스 노드를 반환한다.

    만약 해당 프로세스 노드가 이미 최하 단계의 우선순위 큐(head_array) 에 위치하고 있다면, 
    Round Robin 방식으로 진행을 하기 위해 위치하고 있는 큐의 맨뒤로 삽입된다.

    remaining_time 또한 업데이트 해주며 프로세스 노드의 잔여 시간이 남아있지 않다면, 
    Turn Around Time 을 유지관리하는 processing_time 변수를 업데이트 해준다.

    만약 더 이상 작업된 프로세스 노드가 없다면 함수를 exit 한다.
*/
void process_pop(){
    queue_pointer popped_process;
    int head_id = _get_head_queue_id();
    int remaining_time; 
    popped_process = delete_process(head_id);
    if(popped_process==NULL){
        return;
    } 
    if(head_id == HEAD_QUEUE_SIZE-1){ // 마지막 큐는 Round Robin
        remaining_time = popped_process->remaining_time;
        popped_process->remaining_time = popped_process->remaining_time - TIME_SLICE[head_id];
        if(popped_process->remaining_time > 0){
            processing_time = processing_time + TIME_SLICE[head_id];
            insert_process_to_round_robin(popped_process);
        } else if(popped_process->remaining_time <= 0){
            processing_time = processing_time + remaining_time;
            display_finished_process(popped_process);
            free(popped_process);
        }
    }               // 앞 1,2,3 번째 큐는 FCFS
    else {
        if(Game_Tolerance[head_id]==0){ // Game Tolerance 적용, 정해진 time slice 이상 작업이면, 낮은 큐 재배치
            Game_Tolerance[head_id] = TIME_SLICE[head_id];
            processing_time = processing_time + TIME_SLICE[head_id];
            insert_process_to_lower_queue(popped_process);
        }else{                          // 일반적 처리
            remaining_time = popped_process->remaining_time;
            Game_Tolerance[head_id] = Game_Tolerance[head_id] - remaining_time;
            popped_process->remaining_time = popped_process->remaining_time - TIME_SLICE[head_id];
            if(popped_process->remaining_time > 0){
                processing_time = processing_time + TIME_SLICE[head_id];
                insert_process_to_lower_queue(popped_process);
            } else if(popped_process->remaining_time <= 0){
                processing_time = processing_time + remaining_time;
                display_finished_process(popped_process);
                free(popped_process);
            }
        }
    }
}

/*
    우선순위를 상향조정(Boosting)하는 작업을 실행한다.
    너무 많은 대화형 작업이 존재하면, 작업 시간이 긴 프로세스는 오랜시간 CPU 할당을 받지 못해서
    기아가 될 확률이 높다.
    따라서 VOODOO_CONSTANT 이상의 작업을 진행하게 되면, 모든 작업을 상향조정(Boost)하여 
    기아현상을 방지한다.
    낮은 우선순위 큐에 있는 프로세스 노드를 가장 최고 우선순위 큐에 맨 뒤에 재배치한다.
*/
void boost_process(void){
    // int head_id = _get_head_queue_id();
    // if(head_id == 0){
    //     return;
    // }
    // printf("BOOSTING\n");
    for(int i = 1; i < HEAD_QUEUE_SIZE; i++){
        while(head_array[i]->head != NULL){
            queue_pointer starved_process = delete_process(i);
            starved_process->priority = 0;
            _insert_at_tail(0, starved_process);
        }
    }
    // printf("DONE\n");
}

/*
    VOODOO_CONSTANT 만큼 작업을 실행했으면, 참을 반환한다.
*/
int is_starving(int cpu_cycle){
    return (cpu_cycle > VOODOO_CONSTANT);
}
/*
    터미널에서 인자로 입력되는 input_file.txt 를 파일 포인터로 읽고
    해당되는 type, process_id, computing_time 변수를 정의한다.

    프로그램의 main loop 역할을 한다.
*/
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
    initialize_head();

    // 파일을 읽으며, 큐에 삽입 및 업데이트 될 프로세스 노드를 생성, 제거, 업데이트 해준다.
    int cpu_cycle = 0;
    int process_count = 0;
    printf("______________________________________________\n");
    printf("Process id \t Priority \t Computing T \t Turn Around T \n");
    printf("______________________________________________\n");
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (!strcmp(buffer, "-1"))
        {
            // printf("STOPPED\n");
            break;
        }

        // 파일 내용 토큰화 
        // type             (0) insert, (1) ended process
        // process_id       프로세스의 id
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
                process_count++;
                insert_new_process(process_id, computing_time, priority);
                break;
            case 1:
                // printf("Process Time Exceeded\n");
                process_pop();
                // display_head();

                break;
            default:
                perror("You have messed something up, dude...");
                break;
        }
        if(is_starving(cpu_cycle)){
            cpu_cycle = 0;
            boost_process();
        }
        cpu_cycle++;
    }

    // 파일 읽기 후 head_array 큐에 남아있는 프로세스 노드들의 남은 작업을 완료한다.
    while(!IS_CPU_WORK_OVER){
        // display_head();
        process_pop();
        if(is_starving(cpu_cycle)){
            cpu_cycle = 0;
            boost_process();
        }
        cpu_cycle++;
    }

    printf("Normalized Turnaround Time : %d\n", total_turnaround / process_count);
    return 0;
}