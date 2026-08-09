# nanosvg 설치 안내

sgf 엔진의 SVG 텍스처 로딩(`Texture::LoadFromSvgFile`)은 [nanosvg](https://github.com/memononen/nanosvg) 라이브러리를 사용합니다.

## 설치 방법 (2분 소요)

1. 아래 두 파일을 다운로드해서 **이 폴더**(`Projects/sgf/Sources/sgf/_Extern/nanosvg/`)에 넣으세요.
   - <https://raw.githubusercontent.com/memononen/nanosvg/master/src/nanosvg.h>
   - <https://raw.githubusercontent.com/memononen/nanosvg/master/src/nanosvgrast.h>
2. 다시 빌드하면 끝입니다.

## 동작 원리

- `Texture.cpp`는 `__has_include("sgf/_Extern/nanosvg/nanosvg.h")`로 파일 존재를 감지합니다.
- 파일이 없으면 `SGF_HAS_NANOSVG`가 꺼지고 `LoadFromSvgFile`은 실패(false)를 반환할 뿐,
  나머지 엔진 기능(PNG 로딩 포함)은 모두 정상 동작합니다.
- nanosvg는 헤더 온리 라이브러리라 별도 링크 설정이 필요 없습니다.

## 라이선스

nanosvg는 zlib 라이선스로 상업적 사용이 자유롭습니다.
