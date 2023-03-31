/*
 * 작성자: 윤정도
 * 생성일: 3/24/2023 10:26:49 PM
 * =====================
 *
 */

#include "Tutturu.h"
#include "GameCoreHeader.h"
#include "SGNetClientGroup.h"

USING_NS_JC;
USING_NS_CC;
USING_NS_JNET;

SGNetClientGroup::SGNetClientGroup()
	: m_spAuthServer(nullptr)
{}
SGNetClientGroup::~SGNetClientGroup() {
}

void SGNetClientGroup::Initialize() {

	CreateIocp(2);
	CreateBufferPool({});
	RunIocp();

	TcpClientPtr spAuthClient = MakeShared<TcpClient>(m_spIOCP, m_spBufferPool, &m_LoginListener);

	AddHost(m_spAuthServer);
}

