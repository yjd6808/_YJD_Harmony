/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 10:43:07 PM
 * =====================
 * [! WARNING !]이 파일은 어디서도 쓰이지 않는 헤더파일이다.
 * 이번에 구현한 PrioritySingleton 기능 구현을 위해 작성된 파일들인데 제약사항이 너무 커서
 * 폐기처분한다.
 *
 * 1. main 함수 시작/끝에서 클래스 생성/소멸을 위해 - initialize, finalize를 필수적으로 수행해야함.
 *   이때문에 전역코드 레벨에서 원하는 기능을 구현하는데 자유도가 떨어진다.
 * 2. 전역변수를 포인터로 두지말자. 객체가 존재하지 않는 경우를 신경 써야한다니..
 *    올라가야할 산 정상은 까마득한데 어깨에 무거운 짐을 짊어진 느낌..
 *
 * 다음부턴 이렇게 만들지 않기 위해 박제한다.
 */

#include <JCore/Core.h>
#include <JCore/Debug/MemoryLeakDetector.h>
#include <JCore/System/PrioritySingleton.h>
#include <JCore/System/System.h>

#if 0
namespace JCore {

	MemoryLeakDetector leakDetector;

	SystemAbstract::SystemAbstract(int code) {
		DebugAssertMessage(code >= 0 && code <= Detail::MaxSystemCode_v, "시스템 코드번호는 0 ~ 64사이로만 지정가능합니다.");
		DebugAssertMessage(SystemMap_v[code] == nullptr, "이미 해당 코드번호에 해당하는 시스템이 존재합니다.");
		m_iCode = code;
		SystemMap_v[code] = this;
	}

	SystemAbstract::~SystemAbstract() {
		SystemAbstract::OnTerminate();
	}

	void SystemAbstract::RegisterPrioritySingleton(IPrioritySingleton* priorityClass) {
		if (priorityClass->GetConstructionPriority() >= 0)
			CreateClassConstructionOrder(priorityClass);

		CreateClassDestructionOrder(priorityClass);
	}

	void SystemAbstract::OnStartUp() {
		ConstructGlobalObject();
	}

	void SystemAbstract::OnTerminate() {
		DestroyGlobalObject();
	}

	void SystemAbstract::ConstructGlobalObject() {

		// 우선순위대로 생성
		auto pCurNode = m_pConstructionOrderHead;
		while (pCurNode != nullptr) {
			auto pTemp = pCurNode;
			pCurNode->Class->Construct();
			pCurNode = pCurNode->Next;
			DeleteSafe(pTemp);
		}

		m_pConstructionOrderHead = nullptr;
		m_pConstructionOrderTail = nullptr;
	}

	void SystemAbstract::DestroyGlobalObject() {

		// 우선순서대로 소멸
		auto pCurNode = m_pDestructionOrderHead;
		while (pCurNode != nullptr) {
			auto pTemp = pCurNode;
			pCurNode->Class->Destroy();
			pCurNode = pCurNode->Next;
			DeleteSafe(pTemp);
		}

		m_pDestructionOrderHead = nullptr;
		m_pDestructionOrderTail = nullptr;
	}

	// 명시된 클래스 생성 순서대로 삽입
	void SystemAbstract::CreateClassConstructionOrder(IPrioritySingleton* priorityClass) {
		auto pNewNode = new Detail::ManagedClassNode{ priorityClass, nullptr };

		if (m_pConstructionOrderHead == nullptr) {
			m_pConstructionOrderHead = pNewNode;
			m_pConstructionOrderTail = pNewNode;
			return;
		}

		auto pCurNode = m_pConstructionOrderHead;
		auto pPrevNode = pCurNode; pPrevNode = nullptr;

		while (pCurNode != nullptr) {

			if (pNewNode->Class->GetConstructionPriority() < pCurNode->Class->GetConstructionPriority()) {
				if (pPrevNode == nullptr) {
					m_pConstructionOrderHead = pNewNode;
					m_pConstructionOrderHead->Next = pCurNode;
					return;
				}

				pPrevNode->Next = pNewNode;
				pNewNode->Next = pCurNode;
				return;
			}

			pPrevNode = pCurNode;
			pCurNode = pCurNode->Next;
		}

		m_pConstructionOrderTail->Next = pNewNode;
		m_pConstructionOrderTail = pNewNode;
	}

	void SystemAbstract::CreateClassDestructionOrder(IPrioritySingleton* priorityClass) {
		auto pNewNode = new Detail::ManagedClassNode{ priorityClass, nullptr };

		if (m_pDestructionOrderHead == nullptr) {
			m_pDestructionOrderHead = pNewNode;
			m_pDestructionOrderTail = pNewNode;
			return;
		}

		auto pCurNode = m_pDestructionOrderHead;
		auto pPrevNode = pCurNode; pPrevNode = nullptr;

		while (pCurNode != nullptr) {
			if (pNewNode->Class->GetDestructionPriority() < pCurNode->Class->GetDestructionPriority()) {
				if (pPrevNode == nullptr) {
					m_pDestructionOrderHead = pNewNode;
					m_pDestructionOrderHead->Next = pCurNode;
					return;
				}

				pPrevNode->Next = pNewNode;
				pNewNode->Next = pCurNode;
				return;
			}

			pPrevNode = pCurNode;
			pCurNode = pCurNode->Next;
		}

		m_pDestructionOrderTail->Next = pNewNode;
		m_pDestructionOrderTail = pNewNode;
	}
}
#endif