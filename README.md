# 3축 헬름홀츠코일을 이용한 자력계 자동 검교정 시스템의 제어 소프트웨어

# 서론
오늘날 자기장을 이용한 연구 및 개발은 의학에서부터 항공우주에 이르기까지 다양한 분야에서 이루어지고 있으며, 이는 자기장의 세기 및 방향의 측정을 필요로 한다. 지구 자기장의 경우 약 25-65 μT 정도의 세기를 가지고 있으며 이는 자속게이트(Fluxgate), 서치 코일(Search coil), 이방성 자기저항(Anisotropic magneto-resistive) 소자와 같은 중자계 센서에 의해 측정되어지고, 3축으로 구성된 자력계를 이용함으로써 자기장의 방향을 확인하는 것 또한 가능하다.

3축 자력계의 경우 제조 및 조립 공정 과정에서의 제품별 오프셋(Offset), 감도(Sensitivity) 그리고 직교성(Orthogonality)과 같은 특성들의 편차로 인해 측정값에 에러를 발생시킬 수 있으며, 동작 환경에 따라 발생하는 강철(Hard iron) 및 연철(Soft iron) 효과로 인한 자기장 왜곡 현상으로 추가적인 오차가 발생할 수 있다. 따라서 정밀한 측정값을 위해서는 반드시 자력계의 검교정이 요구되어지게 된다.

3축 자력계의 기본적인 검교정 방식으로는 스칼라(Scalar) 검교정이 있으며 이는 매우 균일하고 안정적인 상태의 지구 자기장에서 3축 자력계를 모든 구면 방향으로 회전시켜 데이터를 측정하는 방법이다. 하지만 이는 검교정 환경으로 인한 장소적 제한성 및 센서 프레임이 고정되어있지 않다는 단점이 존재한다. 따라서, 본 연구에서는 3축 헬름홀츠 코일을 이용한 자력계 검교정 방식을 사용하고자 한다. 헬름홀츠 코일은 균일한 자기장을 발생시키기위한 장치로, 코일의 반경만큼 떨어진 두 개의 동일한 원형 코일 중심에 일정한 자기장을 형성시킬 수 있다. 총 세 축의 헬름홀츠 코일을 사용함으로써 특정 세기 및 방향의 자기장을 생성하는 것이 가능하며, 코일 중심에 놓인 표준 자력계 및 피시험용 자력계를 통해 측정된 자기장의 세기를 비교함으로써 검교정을 수행하게 된다.

저비용의 검교정 시스템 구축을 위해 저렴한 가격의 단일극성(Unipolar) DC 전원공급기가 사용되었으며 이는 아두이노를 이용한 릴레이 모듈을 통해 이중극성(Bipolar)으로 제어되었다. 또한 커맨드를 이용한 DC 전원공급기의 시리얼 제어 기능을 통해 헬름홀츠 코일 중심에 사용자 정의 세기의 자기장이 형성될 수 있으며 이를 포함한 검교정 시스템 전체는 자체 개발한 **QT 프레임워크 기반의 윈도우 어플리케이션**을 통해 제어될 수 있다.

검교정을 위한 데이터 수집 과정은 사용자 정의 자기장 형성 및 데이터 저장과 같은 반복적인 작업으로 이루어진다. 자동화 시스템이 개발되기 이전의 경우 해당 과정은 수동으로 진행되었기 때문에, 시간적 비효율성 및 교정 결과의 신뢰성 하락과 같은 단점들을 불러일으켰다. 따라서 이러한 단점들을 극복하기 위해서 소프트웨어를 통해 반복적인 작업을 자동으로 수행할 수 있도록 하였으며, 결과적으로 검교정 과정의 효율성 및 신뢰성 향상을 기대하고자 한다.

# 시스템 구성

# 하드웨어 디자인
![Block diagram](https://user-images.githubusercontent.com/54430715/77283100-faa04a80-6d0e-11ea-9ac0-62b5d706326d.png)

