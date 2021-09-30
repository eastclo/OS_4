# OS_4
Pthread
* 과제 목표
  - 다중 쓰레드 프로그래밍의 기본 개념을 이해
  - lock 및 조건 변수를 사용하는 쓰레드(pthread) 프로그램 구현

* 기본지식
  - pthread 사용자 수준 쓰레드 라이브러리
  - 다중 쓰레드로 구현된 시스템의 일반적인 형태

* 과제 기본 내용
  - 많은 응용에서 볼 수 있는 간단한 마스터 및 워커 스레드 풀을 구현
  - 읽기-쓰기 Locks을 통한 쓰레드 간 병행성 향상 프로그램 구현
  - pthread를 이용한 사용자 수준의 SSU_세마포어 구현

* 과제 구현 내용
  1. Master-Worker Thread Pool 구현
  2. Reader-Writer Locks 쓰레드 프로그램 구현
  3. pthread를 이용한 사용자 수준 세마포어(SSU_Sem) 구현
