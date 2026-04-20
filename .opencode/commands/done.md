---
description: 작업 완료 후 history 파일 생성
---

오늘 날짜(YYYY-MM-DD)와 "$ARGUMENTS"를 조합하여 `.admin/histories/{날짜}-$ARGUMENTS.md` 파일을 생성하라.

파일에는 다음 내용을 포함할 것:
- 작업 날짜
- 작업 결과 (빌드 성공/실패, 경고/오류 수)
- 작업 배경
- 변경된 파일 목록 (신규 생성 / 수정 구분)
- 주요 변경 내용 요약

파일 생성 후 사용자에게 history 파일 경로를 알려주어라.
