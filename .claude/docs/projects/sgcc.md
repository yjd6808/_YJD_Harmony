# sgcc (SteinsGate Common Components)

## 개요
SteinsGate 프로젝트의 **공용 수학 및 기본 타입 라이브러리** (Static Library).  
Cocos2d-x와 독립적인 순수 C++ 유틸리티. 모든 SteinsGate 모듈에서 공유.

## 소스 구조

```
Sources/sgcc/
├── PCH/Core.h/cpp
├── base/
│   └── ccTypesBase.h/cpp    - 기본 타입 정의
└── math/
    ├── CCMath.h              - 수학 라이브러리 통합 헤더
    ├── Vec2.h/cpp            - 2D 벡터
    ├── Vec3.h/cpp            - 3D 벡터
    ├── Vec4.h/cpp            - 4D 벡터
    ├── Mat4.h/cpp            - 4x4 행렬
    ├── Quaternion.h/cpp      - 쿼터니언
    ├── CCGeometry.h/cpp      - 기하학 (점, 크기, 직사각형)
    ├── CCAffineTransform.h/cpp - 아핀 변환
    ├── CCVertex.h/cpp        - 정점 정의
    ├── MathUtil.h/cpp        - 수학 유틸리티
    └── TransformUtils.h/cpp  - 변환 유틸리티
```

## 의존성
- 외부 의존성 없음 (Pure C++)

## 빌드 산출물
- `sgcc.lib` (Static Library)
