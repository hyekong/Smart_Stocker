#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <pthread.h>

// ==========================================
// [1] 연결 리스트(자료구조) 노드 정의
// ==========================================
typedef struct WarningNode {
    char time_info[50];       // 발생 시간
    float temperature;        // 발생 당시 온도
    struct WarningNode* next; // 다음 노드를 가리키는 포인터
} WarningNode;

WarningNode* head = NULL; // 리스트의 머리 부분

// 리스트 끝에 새 경고 데이터를 추가하는 함수
void appendNode(char* time_str, float t) {
    WarningNode* newNode = (WarningNode*)malloc(sizeof(WarningNode));
    strcpy(newNode->time_info, time_str);
    newNode->temperature = t;
    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;
        return;
    }
    WarningNode* temp = head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}
// 프로그램 시작 시 파일에서 데이터를 읽어와 리스트를 복구하는 함수
void loadListFromFile() {
    FILE *file = fopen("warning_data.txt", "r");
    if (file == NULL) {
        // 파일이 없으면 처음 실행하는 것이므로 그냥 넘어감
        return; 
    }

    char time_str[50];
    float t;
    
    // 쉼표(,)를 기준으로 시간 문자열과 온도 실수를 분리해서 읽어들임
    while (fscanf(file, "%[^,],%f\n", time_str, &t) == 2) {
        appendNode(time_str, t); // 읽어온 데이터를 통해 노드를 다시 생성!
    }
    
    fclose(file);
    printf("📂 이전 경고 기록을 성공적으로 불러왔습니다!\n");
}

// ==========================================
// [2] 전역 변수 및 타이머 설정
// ==========================================
int fd; 
float current_temp = 0.0; 
float current_humi = 0.0; 

time_t motor_start_time = 0;
int is_motor_running = 0; 


// ==========================================
// [3] 백그라운드 모니터링 스레드
// ==========================================
void *monitor_thread(void *arg) {
    char buf[256];
    float temp, humi;

    while (1) {
        int n = read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            
            if (sscanf(buf, "T:%f,H:%f", &temp, &humi) == 2) {
                current_temp = temp;
                current_humi = humi;
                time_t now = time(NULL); 
                
                // [A] LED 즉각 제어
                if (temp >= 30.0) {
                    write(fd, "L", 1); 
                } else {
                    write(fd, "l", 1); 
                }

                // [B] 모터 10초 타이머 제어 및 자료구조 저장
                if (temp >= 30.0) {
                    if (!is_motor_running) {
                        write(fd, "M", 1); // 모터 ON
                        is_motor_running = 1;
                        motor_start_time = now; 
                        
                        // 현재 시간을 문자열로 변환
                        struct tm *t = localtime(&now);
                        char timeStr[50];
                        sprintf(timeStr, "%04d-%02d-%02d %02d:%02d:%02d", 
                                t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                                t->tm_hour, t->tm_min, t->tm_sec);

                        // 경고가 발생할 때마다 연결 리스트에 노드 추가.
                        appendNode(timeStr, temp);

			// 하드디스크 텍스트 파일에도 동시에 백업용으로 기록
			FILE *log_file = fopen("warning_data.txt", "a"); // 파일 이름을 깔끔하게 변경
			if (log_file != NULL) {
			    //컴퓨터가 읽기 쉽게 '시간,온도' 두 개만 쉼표로 구분해서 저장.
			    fprintf(log_file, "%s,%.1f\n", timeStr, temp);
			    fclose(log_file);
			}
                    }
                } 
                else { 
                    if (is_motor_running) {
                        double elapsed = difftime(now, motor_start_time);
                        if (elapsed >= 10.0) {
                            write(fd, "m", 1); // 10초 경과 시 모터 OFF
                            is_motor_running = 0;
                        } 
                    }
                }
            }
        }
        usleep(100000); 
    }
    return NULL;
}

// ==========================================
// [4] 메인 화면 및 메뉴 제어
// ==========================================
int main() {
    loadListFromFile();

    fd = open("/dev/ttyACM0", O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("포트 열기 실패");
        return 1;
    }

    struct termios tty;
    tcgetattr(fd, &tty);
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tcsetattr(fd, TCSANOW, &tty);

    pthread_t tid;
    pthread_create(&tid, NULL, monitor_thread, NULL);

    int choice;

    while (1) {
        printf("\n================================\n");
        printf(" 스마트 재고관리 시스템\n");
        printf("================================\n");
        printf(" 1. 현재 온습도 확인하기\n");
        printf(" 2. 경고 기록 확인\n");
        printf(" 3. 프로그램 종료\n");
        printf("================================\n");
        printf("메뉴를 선택하세요: ");
        
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("숫자를 입력해주세요.\n");
            continue;
        }

        if (choice == 1) {
            printf("\n🌡️ 최신 측정 온도: %.1f도 | 💧 습도: %.1f%%\n", current_temp, current_humi);
        
        } else if (choice == 2) {
            // '연결 리스트'를 순회하며 출력
            printf("\n🚨 [경고 기록] 🚨\n");
            printf("---------------------------------------------\n");
            WarningNode* tempNode = head;
            int count = 1;
            
            if (tempNode == NULL) {
                printf("📂 아직 메모리에 생성된 경고 노드가 없습니다.\n");
            } else {
                while (tempNode != NULL) {
                    printf("%d -> 시간: %s | 발생 온도: %.1f도\n", count++, tempNode->time_info, tempNode->temperature);
                    tempNode = tempNode->next;
                }
            }
            printf("---------------------------------------------\n");
            
        } else if (choice == 3) {
            printf("\n프로그램을 종료합니다.\n");
            pthread_cancel(tid); 
            break; 
        } else {
            printf("\n❌ 잘못된 번호입니다.\n");
        }
    }

    close(fd);
    return 0;
}

