/*
 * 작성자: 윤정도
 * 생성일: 1/8/2023 5:41:51 PM
 * =====================
 *
 */


#pragma once

#pragma once

#include <sg/_Sga/SgaElement.h>


class SgaSpriteAbstract;
class SgaLoader;

class SgaImage : public SgaElement
{
	using SgaSpriteAbstractPtr = jc::SharedPtr<SgaSpriteAbstract>;
	using SgaSpriteList = jc::Vector<SgaSpriteAbstractPtr>;
	using SgaImagePtr = jc::SharedPtr<SgaImage>;

public:
	SgaImage(const Header& _header, int _size);
	~SgaImage() override;

public:
	void Set(int _idx, const SgaSpriteAbstractPtr& _pSprite) const;
	void Remove(const SgaSpriteAbstractPtr& _pSprite);
	void RemoveAt(const int _idx);
	int Count() const { return sprite_.Size(); }
	void Clear();
	int WaitForLoadingDataCount() const { return waitForLoadingDataCount_; }
	SgaSpriteAbstractPtr GetAt(const int _idx) const;
	SgaSpriteAbstractPtr GetAtUnsafe(const int _idx) const;
	SgaSpriteAbstract& GetAtRef(const int _idx) const;

	static SgaImagePtr Create(const Header& _header, int _size);

	// 연산자를 사용할 때는 레퍼런스로 가져오도록 하자.
	SgaSpriteAbstract& operator[](const int _idx) const;
	bool IsValidIndex(int _index) const;

	bool Load(bool _indexOnly) override;
	bool Unload() override;
	jc::String ToString() const;

protected:
	SgaSpriteList sprite_;
	int waitForLoadingDataCount_;

	friend class SgaLoader;
};

using SgaImagePtr = jc::SharedPtr<SgaImage>;
using SgaImageWeakPtr = jc::WeakPtr<SgaImage>;
