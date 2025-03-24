202111302 백진률
Computer Graphics Assignment 1

1. 구현 세부 사항
1.1 구현된 클래스
•	Ray (광선): 광선의 원점과 방향을 나타냅니다.
•	Surface (추상 기본 클래스): 광선과 교차할 수 있는 객체의 기본 인터페이스를 정의합니다.
•	Sphere (구, Surface 상속): 구와 광선의 교차 로직을 구현합니다.
•	Plane (평면, Surface 상속): 평면과 광선의 교차 로직을 구현합니다.
•	Camera (카메라): 원근 투영을 기반으로 각 픽셀에 대한 광선을 생성합니다.
•	Scene (장면): 장면 내의 객체를 관리하고 광선과의 교차 검사를 수행합니다.

1.2 Assignment.cpp
구현한 클래스 및 코드들은 Assignment.cpp 안에 존재하고, 구현되었습니다.

3. 결과

 ![image](https://github.com/user-attachments/assets/301275ed-2aed-47ad-b37f-215854bcb1f3)

3. 컴파일 및 실행 방법
3.1 필요 라이브러리
본 프로젝트는 다음 라이브러리를 필요로 합니다:
•	GLFW: 창 관리
•	GLEW: OpenGL 확장 기능 처리
•	GLM: 벡터 및 행렬 연산
•	OpenGL: 최종 이미지 렌더링

3.2 컴파일 및 실행 방법
1.	Visual Studio에서 프로젝트 솔루션 파일을 엽니다.
2.	필요한 라이브러리(GLFW, GLEW, OpenGL)가 포함되어 있는지 확인합니다.
3.	프로젝트를 빌드하고 실행합니다.
