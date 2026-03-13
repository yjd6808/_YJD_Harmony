/*
 * 작성자: 윤정도
 * 생성일: 8/26/2023 9:44:58 PM
 * =====================
 *
 */

#pragma once

NS_SG_BEGIN

class SG_DLL GameDB : public jdb::MysqlDatabase, public jc::SingletonPointer<GameDB>
{

};

NS_END

#define g_cGameDB JC_DECL_SINGLETON_BODY(sg::GameDB)