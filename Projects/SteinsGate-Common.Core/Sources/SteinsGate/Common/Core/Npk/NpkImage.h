/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:41:51 PM
 * =====================
 *
 */


#pragma once

#pragma once

#include <SteinsGate/Common/Core/Npk/NpkElement.h>


class NpkSpriteAbstract;
class NpkLoader;
class NpkImage : public NpkElement
{
	using NpkSpriteAbstractPtr = JCore::SharedPtr<NpkSpriteAbstract>;
	using NpkSpriteList = JCore::Vector<NpkSpriteAbstractPtr>;
	using NpkImagePtr = JCore::SharedPtr<NpkImage>;
public:
	NpkImage(const Header& header) : NpkElement(NpkElement::eImage, header) {}
	~NpkImage() override;
public:
	void Add(NpkSpriteAbstractPtr sprite);
	void Remove(NpkSpriteAbstractPtr sprite);
	void RemoveAt(const int idx);
	int Count() { return m_Sprites.Size();  }
	void Clear();
	int WaitForLoadingDataCount() { return m_iWaitForLoadingDataCount; }
	NpkSpriteAbstractPtr GetAt(const int idx) { return m_Sprites[idx]; }
	NpkSpriteAbstract& GetAtRef(const int idx) { return m_Sprites[idx].GetRef(); }

	static NpkImagePtr Create(const Header& header);

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	NpkSpriteAbstract& operator[](const int idx) { return m_Sprites[idx].GetRef(); }

	
	bool Load(bool indexOnly) override;
	void Unload() override;
protected:
	NpkSpriteList m_Sprites;	
	int m_iWaitForLoadingDataCount;

	friend class NpkLoader;
};

using NpkImagePtr = JCore::SharedPtr<NpkImage>;
using NpkImageWeakPtr = JCore::WeakPtr<NpkImage>;

