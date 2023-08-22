/*
 * 작성자: 윤정도
 * 생성일: 6/14/2023 8:53:15 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Define.h>

JCORE_SENUM_BEGIN(LoginResult)
LoginSuccess,				// 로그인 성공
RegisterSuccess,			// 회원가입 성공\n 다시 입력 후 로그인 해주세요.
IdAlreadyExist,				// 회원가입 실패\n 이미 입력하신 ID로 회원가입된 계정이 존재합니다.
IdPasswordMismatch,			// 아이디 또는 비밀번호가 일치하지 않음.
Banned,						// 정지된 계정입니다.\n정지일: yyyy-MM-dd HH:mm:ss\n해제일: yyyy-MM-dd HH:mm:ss
Logined,					// 현재 접속중인 계정입니다.
QueryFailed,				// 서버에서 쿼리 수행중 오류가 발생하였습니다.
Max
JCORE_SENUM_END(LoginResult)

