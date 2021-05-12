# CPUScheduling
운영체제 팀플 과제: CPU 스케줄링

CPU 스케줄링 프로그램의 설계 및 구현(in C Language)
[과제 Due Date 지나며 정리해서 올리겠습니다.]

## Scheduling Algorithms
Given a list of scheduling algorithms, FCFS(First Come First Served), Round-Robin, and Multilevel Feedback Queue were chosen

FCFS, Round-Robin, MLFQ 를 각자 선택하여 구현하였습니다.

- FCFS(First Come First Served)
    - `FIFO` 와 같은 의미
    - Ready-Queue 에 Ready State Process 를 enqueue 시킨다. 
    - Ready-Queue 에서 가장 오래 기다린(큐 맨앞의) Process 가 다음 실행 프로세스로 선정
    - Non-preemptive 이기 때문에, computing time 이 긴 프로세스에 유리
    - 처리기 중심 프로세스 우대

- Round-Robin
    - FCFS 에서 처리 시간이 짧은 프로세스가 보는 피해 현상 완화
        - `Time Quantum` 을 기준으로 일정 시간 넘어가는 프로세스를 중단시킨(Preemption)
    - Time Quantum(각 프로세스의 시간 할당량) 에 따른 알고리즘 성능
        - 시간 할당량이 너무 작다 -> Increase in Context Switching Overhead 
        - 시간 할당량이 너무 크다 -> FCFS 처럼 실행됨
        - 권장길이
            > 프로세스가 사용자와 최소한 한 번 이상 대화 
            
            > 프로세스 내 함수 정도 실행 가능한 길이
    - 처리기 중심 프로세스 우대
        - `Virtual Round Robin` 으로 해소

- MLFQ(Multilevel Feedback Queue)
    - `우선순위 큐`(Priority Queue) 활용, 우선순위 별 대기 큐 생성
    - 높은 대기 큐에 있는 프로세스 실행
    - 가장 높은 대기 큐(Processes with same low priority)는 Round-Robin 스케줄링 활용
        - Time Quantum 을 각 대기 큐마다 다르게 함
    - 중단점을 만나면 한 단계 낮은 대기 큐에 할
    - 새 프로세스, 짧은 프로세스 일수록 우대
        - `기아현상`(Process Starvation) 우

## Input Process Data
Process 에 대한 입력 데이터
```
# Type ProcessID Priority Computing_Time
  0    1         25       80
  0    2         15       40
  0    3         8        30
  1    0         0        0
  0    4         12       10
  0    5         22       30
  0    6         28       50
  1    0         0        0
  0    7         5        20
  0    8         3        40
  0    9         13       60
  1    0         0        0
  0    10        24       45
  -1   
```
 - Type (0, 1)
    > 0 : Create Process
    
    > 1 : Time Quantum Exceeded

 - ProcessID
    - ID of individual Process

 - Priority
    - Priority of an individual Process

 - Computing Time
    - Total amount of Compute Time required for an individual Process

## Output Process Data
스케줄링 이후 출력 데이터
```
# ProcessID Priority Computing_Time Turn_Around_Time
```

 - ProcessID
    - ID of individual Process

 - Priority
    - Priority of an individual Process

 - Computing Time
    - Total amount of Compute Time required for an individual Process

 - Turn Around Time
    - Total amount of CPU computing time required for an entire scheduled Processes to go through

# Team
- [홍성민: MLFQ](https://github.com/KKodiac)

- [권순호: Round-Robin](https://github.com/tnsgh9603)

- [박서진: FCFS]()
