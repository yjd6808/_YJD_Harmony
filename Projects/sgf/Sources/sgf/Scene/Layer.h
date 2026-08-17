/*
 * 작성자: 윤정도
 * 생성일: 8/16/2026 (게임 오브젝트 시스템)
 * =====================
 * Layer — 화면 밴드 구분자 (명칭 전용 파생)
 *
 * [역할]
 * GameObject와 기능은 동일하다. 다만 "이 노드는 화면 밴드를 나누는 기준"이라는
 * 의도를 타입으로 드러낸다.
 * 예: AddChild(GameLayer, 1) / AddChild(UILayer, 2) → UILayer 서브트리가 무조건 위.
 *
 * 기능 추가 없음 — 추후 Layer 전용 확장(스크롤, 클리핑)을 붙일 자리를 미리 확보.
 */

#pragma once

#include "sgf/Scene/GameObject.h"

NS_SGF_BEGIN

class Layer : public GameObject
{
public:
	using GameObject::GameObject;
};

NS_SGF_END