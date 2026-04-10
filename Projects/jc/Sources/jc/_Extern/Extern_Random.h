/*
 * 작성자 : 윤정도
 * 목적 : MASM 어셈블리 코드에서 Random API를 호출하기 위한 extern "C" 래퍼
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Random 초기화
 */
void RandomInit();

/*
 * 정수형 난수 생성 (포함 범위: _inclusiveBegin ~ _inclusiveEnd)
 * @param _inclusiveBegin : 시작 값 (포함)
 * @param _inclusiveEnd : 끝 값 (포함)
 * @return 생성된 난수
 */
int RandomGenerateInt(int _inclusiveBegin, int _inclusiveEnd);

/*
 * 실수형 난수 생성 (포함 범위: _inclusiveBegin ~ _inclusiveEnd)
 * @param _inclusiveBegin : 시작 값 (포함)
 * @param _inclusiveEnd : 끝 값 (포함)
 * @return 생성된 난수
 */
double RandomGenerateDouble(double _inclusiveBegin, double _inclusiveEnd);

/*
 * 알파벳 난수 생성
 * @return 생성된 알파벳 (a-z, A-Z)
 */
char RandomGenerateAlphabat();

/*
 * 확률 기반 난수 생성
 * @param _percentProbability : 백분율로 표현된 확률 (예: 70.0 = 70%)
 * @return 확률에 따른 결과 (1 = true, 0 = false)
 */
int RandomChance(double _percentProbability);

/*
 * 버퍼에 난수 알파벳 텍스트 작성
 * @param _length : 생성할 문자 개수
 * @param _pBuff : 출력 버퍼
 * @param _capacity : 버퍼 용량
 */
void RandomWriteAlphabatTextBuffered(int _length, char* _pBuff, int _capacity);

#ifdef __cplusplus
}
#endif
