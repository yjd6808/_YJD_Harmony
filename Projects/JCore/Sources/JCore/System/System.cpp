/*
 * 작성자: 윤정도
 * 생성일: 12/13/2022 10:43:07 PM
 * =====================
 */

#include <JCore/Core.h>
#include <JCore/System/SingletonLifeCycle.h>
#include <JCore/System/System.h>

namespace JCore {
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
	void SystemAbstract::CreateClassConstructionOrder(ISingletonLifeCycle* cycledClass) {
		auto pNewNode = new Detail::ManagedClassNode{ cycledClass, nullptr };

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

	void SystemAbstract::CreateClassDestructionOrder(ISingletonLifeCycle* cycledClass) {
		auto pNewNode = new Detail::ManagedClassNode{ cycledClass, nullptr };

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