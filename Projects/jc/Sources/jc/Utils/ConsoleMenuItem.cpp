/*
 * 작성자 : 윤정도
 */

#include "jc/Utils/ConsoleMenuItem.h"

#include <cstdio>

// ============================================================
// 내부 헬퍼 (파일 스코프 static)
// NS_JC_BEGIN 밖에 선언해 익명 네임스페이스 중첩 문제를 회피
// ============================================================
namespace
{
	enum class MenuResult { Back, Home };

	// 키 문자열을 옵션에 맞게 포맷팅
	jc::String CMI_FormatEntryKey(const jc::String& _key, const jc::ConsoleMenuItemOption& _opt)
	{
		jc::String displayKey = _opt.inputForceCase_ ? _key.ToUpperCase() : _key;

		if (_opt.inputFormatPadding_ > 0)
		{
			char buf[64];
			sprintf_s(buf, sizeof(buf), "%*s", _opt.inputFormatPadding_, displayKey.SafeSource());
			return jc::String(buf);
		}
		
		// 패딩 미사용
		return displayKey;
	}

	// 단일 항목 한 줄 출력
	void CMI_PrintEntry(const jc::ConsoleMenuItem::Entry& _entry, const jc::ConsoleMenuItemOption& _opt)
	{
		// 표시 텍스트 결정
		const char* displayText = _entry.text_.SafeSource();
		if (_entry.text_.IsEmpty())
		{
			if (_entry.type_ == jc::ConsoleMenuItem::EntryType::Back)
				displayText = _opt.backMenuName_.SafeSource();
			else if (_entry.type_ == jc::ConsoleMenuItem::EntryType::Home)
				displayText = _opt.homeMenuName_.SafeSource();
			else
				displayText = _opt.defaultMenuName_.SafeSource();
		}

		jc::String formattedKey = CMI_FormatEntryKey(_entry.key_, _opt);

		// 포맷: [LEFT_PAD][LEFT_BRACE][formatted_key][RIGHT_BRACE][RIGHT_PAD][description]
		if (_opt.inputLeftPadding_.Length() > 0)
			jc::Console::Write(_opt.inputPaddingColor_, "%s", _opt.inputLeftPadding_.SafeSource());

		if (_opt.inputLeftBrace_.Length() > 0)
			jc::Console::Write(_opt.inputBraceColor_,   "%s", _opt.inputLeftBrace_.SafeSource());

		// key
		jc::Console::Write(_opt.inputColor_,        "%s", formattedKey.SafeSource());

		if (_opt.inputRightBrace_.Length() > 0)
			jc::Console::Write(_opt.inputBraceColor_,   "%s", _opt.inputRightBrace_.SafeSource());
		if (_opt.inputRightPadding_.Length() > 0)
			jc::Console::Write(_opt.inputPaddingColor_, "%s", _opt.inputRightPadding_.SafeSource());

		// desc
		jc::Console::WriteLine(_opt.menuColor_,     "%s", displayText);
	}

	// 메뉴 한 레벨을 처리하는 재귀 함수
	// 반환값: Back (부모로 이동) 또는 Home (루트로 이동)
	MenuResult CMI_RunMenu(jc::ConsoleMenuItem* _pItem, const jc::ConsoleMenuItemOption& _opt)
	{
		while (true)
		{
			jc::Console::Clear();

			// 헤더 출력
			if (!_pItem->header_.IsEmpty())
				jc::Console::WriteLine(_opt.titleColor_, "%s", _pItem->header_.SafeSource());

			// 항목 출력
			for (const auto& entry : _pItem->entries_)
				CMI_PrintEntry(entry, _opt);

			// 입력 읽기
			jc::String input;
			if (_opt.userInputTitle_)
			{
				input = jc::Console::ReadLine(_opt.inputTitle_.SafeSource());
			}
			else
			{
				input = jc::Console::ReadLine();
			}

			// 입력 매칭
			for (auto& entry : _pItem->entries_)
			{
				jc::String compareKey   = _opt.caseMatch_ ? entry.key_   : entry.key_.ToUpperCase();
				jc::String compareInput = _opt.caseMatch_ ? input         : input.ToUpperCase();

				if (compareKey != compareInput)
					continue;

				if (entry.type_ == jc::ConsoleMenuItem::EntryType::Back)
					return MenuResult::Back;

				if (entry.type_ == jc::ConsoleMenuItem::EntryType::Home)
					return MenuResult::Home;

				if (entry.pChild_.Exist())
				{
					// 서브메뉴 진입
					MenuResult result = CMI_RunMenu(entry.pChild_.GetPtr(), _opt);
					if (result == MenuResult::Home && _pItem->pParent_ != nullptr)
						return MenuResult::Home;  // 루트가 아니면 홈 신호 전파
					// Back 또는 Home(루트에서) -> 현재 메뉴 재출력
				}
				else if (entry.callback_)
				{
					entry.callback_(*_pItem);

					if (_opt.pressAnyKeyAfterCallback_)
					{
						jc::Console::WriteLine(jc::ConsoleColor::Green, "아무 키나 눌러서 계속하세요...");
						jc::Console::ReadKey();
					}
				}
				break;
			}
		}
	}

} // anonymous namespace


NS_JC_BEGIN

// ============================================================
// ConsoleMenuItem 빌더 메서드 구현
// ============================================================

ConsoleMenuItem* ConsoleMenuItem::AddHeader(const jc::String& _text)
{
	header_ = _text;
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleMenuItem* ConsoleMenuItem::Add(const jc::String& _key, const jc::String& _text, jc::Action<ConsoleMenuItem&> _callback)
{
	Entry entry;
	entry.key_      = _key;
	entry.text_     = _text;
	entry.type_     = EntryType::Normal;
	entry.callback_ = jc::Move(_callback);
	entries_.push_back(jc::Move(entry));
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleMenuItem* ConsoleMenuItem::Add(const jc::String& _key, const jc::String& _text, jc::Action<> _callback)
{
	// Action<> 콜백을 Action<ConsoleMenuItem&> 로 래핑
	return Add(_key, _text, [cb = jc::Move(_callback)](ConsoleMenuItem&) { if (cb) cb(); });
}

//////////////////////////////////////////////////////////////////////////////////////////
bool ConsoleMenuItem::AddMenu(const jc::String& _key, const jc::String& _text, ConsoleMenuItem*& _pOutChild)
{
	ConsoleMenuItemPtr pChild = jc::MakeShared<ConsoleMenuItem>();
	pChild->pParent_ = this;

	_pOutChild = pChild.GetPtr();

	Entry entry;
	entry.key_    = _key;
	entry.text_   = _text.IsEmpty() ? _key : _text;
	entry.type_   = EntryType::Normal;
	entry.pChild_ = jc::Move(pChild);
	entries_.push_back(jc::Move(entry));
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleMenuItem* ConsoleMenuItem::AddBack(const jc::String& _key, const jc::String& _text)
{
	Entry entry;
	entry.key_  = _key;
	entry.text_ = _text;
	entry.type_ = EntryType::Back;
	entries_.push_back(jc::Move(entry));
	return this;
}

//////////////////////////////////////////////////////////////////////////////////////////
ConsoleMenuItem* ConsoleMenuItem::AddHome(const jc::String& _key, const jc::String& _text)
{
	Entry entry;
	entry.key_  = _key;
	entry.text_ = _text;
	entry.type_ = EntryType::Home;
	entries_.push_back(jc::Move(entry));
	return this;
}

// ============================================================
// ConsoleMenuItem 메뉴 실행
// ============================================================

void ConsoleMenuItem::Show(ConsoleMenuItemOptionPtr _pOption)
{
	ConsoleMenuItemOption defaultOption;
	const ConsoleMenuItemOption& opt = _pOption.Exist() ? *_pOption.GetPtr() : defaultOption;

	// 루트 메뉴 루프: Back -> 종료, Home -> 루트 재출력
	while (true)
	{
		MenuResult result = CMI_RunMenu(this, opt);
		if (result == MenuResult::Back)
			break;
		// MenuResult::Home -> continue (루트 메뉴 재출력)
	}
}

/*static*/
void ConsoleMenuItem::Show(ConsoleMenuItemPtr _pItem, ConsoleMenuItemOptionPtr _pOption)
{
	if (_pItem.Exist())
		_pItem->Show(jc::Move(_pOption));
}

NS_END
