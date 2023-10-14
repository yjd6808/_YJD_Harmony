/*
 * 작성자: 윤정도
 * 생성일: 7/3/2023 5:46:16 PM
 * =====================
 *
 */


#pragma once

#include <JCore/Container/HashMap.h>
#include <JNetwork/Packet/Command.h>

NS_JNET_BEGIN

class CommandNameDictionary
{
public:
	CommandNameDictionary(int capacity = 64) : m_CommandNameMap(capacity) {}
	virtual ~CommandNameDictionary() = default;

	template <typename Cmd>
	void Add() {
		static_assert(JCore::IsBaseOf_v<ICommand, Cmd>, "... template parameter is not derived from ICommand");

		if (m_CommandNameMap.Exist(Cmd::Command())) {
			DebugAssertMsg(false, "해당 커맨드가 이미 있습니다.");
			return;
		}

		m_CommandNameMap.Insert(Cmd::Command(), Cmd::Name());
	}


	const char* Get(Cmd_t cmd) const;
protected:
	JCore::HashMap<Cmd_t, const char*> m_CommandNameMap;
};


NS_JNET_END