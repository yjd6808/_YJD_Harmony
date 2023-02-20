/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:41:51 PM
 * =====================
 *
 */


#pragma once

#pragma once

#include <SteinsGate/Common/SgaElement.h>


class SgaSpriteAbstract;
class SgaLoader;
class SgaImage : public SgaElement
{
	using SgaSpriteAbstractPtr = JCore::SharedPtr<SgaSpriteAbstract>;
	using SgaSpriteList = JCore::Vector<SgaSpriteAbstractPtr>;
	using SgaImagePtr = JCore::SharedPtr<SgaImage>;
public:
	SgaImage(const Header& header, int size);
	~SgaImage() override;
public:
	void Set(int idx, const SgaSpriteAbstractPtr& sprite);
	void Remove(const SgaSpriteAbstractPtr& sprite);
	void RemoveAt(const int idx);
	int Count() { return m_Sprites.Size();  }
	void Clear();
	int WaitForLoadingDataCount() { return m_iWaitForLoadingDataCount; }
	SgaSpriteAbstractPtr GetAt(const int idx) { return m_Sprites[idx]; }
	SgaSpriteAbstract& GetAtRef(const int idx) { return m_Sprites[idx].GetRef(); }

	static SgaImagePtr Create(const Header& header, int size);

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	SgaSpriteAbstract& operator[](const int idx) { return m_Sprites[idx].GetRef(); }

	bool Load(bool indexOnly) override;
	bool Unload() override;
protected:
	SgaSpriteList m_Sprites;	
	int m_iWaitForLoadingDataCount;

	friend class SgaLoader;
};

using SgaImagePtr = JCore::SharedPtr<SgaImage>;
using SgaImageWeakPtr = JCore::WeakPtr<SgaImage>;

