# Auto control - 3 axis helmholtz coil 
- Development tool: C++ / Qt5, Arduino

## 어떠한 작업을 수행하는 시스템인가요?
- 3축 헬름홀츠코일을 이용한 자력계 자동 검교정 시스템

## 커뮤니케이션
- RS232 시리얼 통신 (PSU <-> PC)

## 시스템 구성 블록 다이어그램

<p align="center"><img src="https://user-images.githubusercontent.com/54430715/77283100-faa04a80-6d0e-11ea-9ac0-62b5d706326d.png" width="50%" height="50%"></p>

## 주요내용
- 3축 헬름홀츠코일 중심에 위치한 표준 자력계의 측정값을 피드백함으로써 외부 자기 환경의 보상 및 코일에 의해 형성된 자기장 세기의 오차를 보정

    -> **사용자 정의 세기의 자기장 형성이 가능하다!**

- 사용자 정의 자기장 형성 알고리즘

<p align="center"><img src="https://user-images.githubusercontent.com/54430715/77283987-fc6b0d80-6d10-11ea-9fe2-109cd2693733.png" width="50%" height="50%"></p>

~~~
1. 헬름홀츠 코일 중심 형성할 사용자 정의 자기장 세기(B_User) 입력
2. 표준자력계 측정값(B_Ref.) 기준 자기장 세기 변화량(dB = B_User - B_Ref.) 계산
3. 비오사바르 법칙을 통한 전압 변화량(dV) 계산
4. DC 전원공급기에 인가될 전압(V + dV)이 양수일 경우, 극성을 Positive로 변경
   DC 전원공급기에 인가될 전압(V + dV)이 음수일 경우, 극성을 Negative로 변경
   (V: 현재 DC 전원공급기에 인가되어 있는 전압)
5. DC 전원공급기에 전압(|V+dV|) 인가
6. 형성된 자기장의 세기(B_Ref.)가 사용자 정의 자기장 세기(B_User)와 차이가 날 경우, 전압 변화량(dV)이 분해능보다 낮아질 때까지 2. ~ 6.을 반복
~~~

위의 알고리즘을 통해 형성할 수 있는 자기장의 최소 세기는 DC 전원공급기의 분해능에 따라 결정되며, 현재 검교정 시스템에서는 헬름홀츠 코일 각 축별 400 nT 정도로 구현이 가능하다.

## UI

