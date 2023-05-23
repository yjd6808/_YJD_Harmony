/*
 * 작성자: 윤정도
 * 생성일: 5/23/2023 9:57:24 PM
 * =====================
 *
 */


#pragma once

#include <SteinsGate/Common/QueryDefine.h>

NS_QRY_BEGIN

struct SelectAccountInfo : SelectStatement<SelectAccountInfo> {
	static constexpr const char* Script = "select c_id, c_pass from t_account";
};

struct SelectAccountInfoResult : SelectResult<SelectAccountInfoResult> {
	JCore::String Id;
	JCore::String Pass;
};


REGISTER_QRY_STRUCT
(
	SelectAccountInfoResult
	, Id, "c_id"
	, Pass, "c_pass"
);


NS_QRY_END




