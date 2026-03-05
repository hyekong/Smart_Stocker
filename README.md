# Smart Stocker : IoT 기반 스마트 창고 온습도 제어 시스템

본 프로젝트는 C언어 자료구조와 아두이노(Arduino)를 연동하여, 창고의 환경(온도, 습도)을 실시간으로 모니터링하고 고온 발생 시 물리적인 액추에이터(환기 팬, 경고등)를 자동 제어하는 IoT 시스템입니다.

## 주요 기능

- **실시간 온습도 모니터링:** SHT20 센서를 활용한 I2C 통신 데이터 수집.
- **다중 작업 처리 (Multi-threading):** 리눅스 `pthread`를 활용하여, 메인 UI와 별개로 백그라운드에서 끊임없이 하드웨어를 감시 및 제어합니다.
- **스마트 하드웨어 제어:** - **LED 경고등:** 30도 이상 초과 시 즉각 점등.
  - **자동 환기 시스템 (DC Motor):** 30도 이상 시 즉시 가동되며, 센서 오작동 방지를 위해 한 번 켜지면 **최소 10초 이상 가동을 보장**하는 타이머 로직이 적용되어 있습니다.
- **자료구조 기반 데이터 로깅:** - 경고 발생 내역을 동적 할당 **연결 리스트(Linked List)** 노드로 메모리에 저장하여 효율적으로 관리합니다.
  - 프로그램 종료 시 `.txt` 파일로 백업하고, 재시작 시 파일을 읽어와(Load) 연결 리스트를 자동 복구합니다.

##  개발 환경 및 하드웨어 구성

**[Software]**
- OS: Ubuntu Linux / Windows
- Language: C (GCC Compiler), C++ (Arduino)

**[Hardware & Pin Map]**
- Board: **Arduino Mega ADK**
- Sensor: **SHT20** 온습도 센서 (I2C 통신)
  - `SDA` -> 20번 핀 / `SCL` -> 21번 핀
- Actuator 1: **LED 경고등**
  - `SIG` -> Digital 2번 핀
- Actuator 2: **DC Motor Module** (환기 팬)
  - `N(방향1)` -> Digital 5번 핀
  - `P(방향2)` -> Digital 6번 핀
  - `EN(엑셀)` -> Digital 7번 핀

## 실행 방법
1. 아두이노 보드에 코드를 업로드하고 센서 및 모터를 연결합니다.
2. 우분투 리눅스 환경에서 아래 명령어를 통해 C 프로그램을 컴파일합니다. (스레드 사용을 위해 `-pthread` 옵션 필수)
   ```bash
   gcc thread_main.c -o thread_main -pthread
   ```



# Smart Stocker: IoT-Based Smart Warehouse Environmental Control System

This project is an integrated IoT system that monitors warehouse conditions (temperature, humidity) in real-time by linking **C-based data structures** with **Arduino**. It features autonomous control of physical actuators (ventilation fans, warning lights) when high temperatures are detected.

## Key Features

- **Real-time Environmental Monitoring:** Data acquisition using the SHT20 sensor via I2C communication.
- **Multi-threaded Processing:** Leverages Linux `pthread` to ensure continuous background hardware monitoring and control, independent of the main UI thread.
- **Intelligent Hardware Control:**
  - **Emergency LED:** Instant activation when the temperature exceeds 30°C.
  - **Automated Ventilation (DC Motor):** Automatically starts at 30°C. Includes a **Minimum 10-second Runtime Guarantee** timer logic to prevent sensor oscillation and system wear.
- **Data Structure-Based Logging:**
  - Manages alert history efficiently in memory using a dynamically allocated **Linked List**.
  - **Data Persistence:** Backs up records to a `.txt` file upon program termination and automatically restores the linked list by loading historical data at startup.

## Tech Stack & Hardware Configuration

**[Software]**
- **OS:** Ubuntu Linux / Windows
- **Language:** C (GCC Compiler), C++ (Arduino IDE)
- **Library:** `pthread.h`, `termios.h`, `Wire.h`

**[Hardware & Pin Map]**
- **Board:** Arduino Mega ADK
- **Sensor:** **SHT20** Temp/Humidity Sensor (I2C)
  - `SDA` -> Pin 20 / `SCL` -> Pin 21
- **Actuator 1:** **Emergency LED**
  - `SIG` -> Digital Pin 2
- **Actuator 2:** **DC Motor Module** (Ventilation Fan)
  - `N (Direction 1)` -> Digital Pin 5
  - `P (Direction 2)` -> Digital Pin 6
  - `EN (Speed/Enable)` -> Digital Pin 7

[Image of Arduino Mega ADK pinout diagram]

## Getting Started

1. Upload the Arduino sketch to your board and connect the sensors and actuators as per the pin map.
2. Compile the C program in your Ubuntu Linux environment. (The `-pthread` flag is mandatory for multi-threading)
   ```bash
   gcc thread_main.c -o thread_main -pthread
