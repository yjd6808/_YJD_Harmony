/*
 * 작성자: 윤정도
 * =====================
 */

#include "GameCoreHeader.h"
#include "sgcl/Game/UI/Xml/UIResolve.h"

#include "sgcl/Game/UI/Core/UIElement.h"
#include "sgcl/Game/UI/Controls/TextBlock.h"
#include "sgcl/Game/UI/Controls/TextBox.h"
#include "sgcl/Game/UI/Controls/ToggleButton.h"
#include "sgcl/Game/UI/Controls/ProgressBar.h"
#include "sgcl/Game/UI/Controls/Slider.h"
#include "sgcl/Game/UI/Controls/ScrollBar.h"
#include "sgcl/Game/UI/Controls/RangeBase.h"

#include <cctype>
#include <cmath>

USING_NS_CC;
USING_NS_JC;

namespace sgui
{

namespace
{

//////////////////////////////////////////////////////////////////////////////////////////
// 해석 중간 값: 요소 참조 또는 값 참조
//////////////////////////////////////////////////////////////////////////////////////////
struct PVal
{
	bool isElement = false;
	UIElement* element = nullptr;
	UIValue value;

	static PVal MakeElement(UIElement* _pElement)
	{
		PVal v;
		v.isElement = true;
		v.element = _pElement;
		return v;
	}

	static PVal MakeValue(const UIValue& _value)
	{
		PVal v;
		v.value = _value;
		return v;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
static int FindMatchingBrace(const jc::String& _expr, int _open)
{
	int depth = 0;

	for (int i = _open; i < _expr.Length(); ++i)
	{
		if (_expr[i] == '{')
		{
			++depth;
		}
		else if (_expr[i] == '}')
		{
			--depth;
			if (depth == 0)
			{
				return i;
			}
		}
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////////////////////
static UIElement* FindChildByName(UIElement* _pParent, const jc::String& _name)
{
	if (_pParent == nullptr)
	{
		return nullptr;
	}

	const int count = _pParent->GetChildElementCount();
	for (int idx = 0; idx < count; ++idx)
	{
		UIElement* pChild = _pParent->GetChildElementAt(idx);
		if (pChild && pChild->getName() == _name.Source())
		{
			return pChild;
		}
	}

	return nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
static UIElement* GetSibling(UIElement* _pElement, int _delta)
{
	if (_pElement == nullptr)
	{
		return nullptr;
	}

	UIElement* pParent = _pElement->GetParentElement();
	if (pParent == nullptr)
	{
		return nullptr;
	}

	const int count = pParent->GetChildElementCount();
	int index = -1;

	for (int idx = 0; idx < count; ++idx)
	{
		if (pParent->GetChildElementAt(idx) == _pElement)
		{
			index = idx;
			break;
		}
	}

	if (index < 0)
	{
		return nullptr;
	}

	const int target = index + _delta;
	return (target >= 0 && target < count) ? pParent->GetChildElementAt(target) : nullptr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 표현식 파서 (재귀 하강)
//////////////////////////////////////////////////////////////////////////////////////////
class ExprParser
{
public:
	ExprParser(const jc::String& _expr, const UIResolveContext& _ctx)
	: expr_(_expr)
	, ctx_(&_ctx)
	{
	}

	UIValue Parse()
	{
		const UIValue result = ParseOr();
		SkipWs();

		// 전체 입력이 소비되지 않았으면 실패 처리
		if (pos_ != expr_.Length())
		{
			Fail();
			return UIValue::MakeNull();
		}

		return failed_ ? UIValue::MakeNull() : result;
	}

	bool Failed() const { return failed_; }

private:
	const jc::String& expr_;
	const UIResolveContext* ctx_;
	int pos_ = 0;
	bool failed_ = false;

	void Fail() { failed_ = true; }

	void SkipWs()
	{
		while (pos_ < expr_.Length() && (expr_[pos_] == ' ' || expr_[pos_] == '\t'))
		{
			++pos_;
		}
	}

	bool Peek(char _c) const { return pos_ < expr_.Length() && expr_[pos_] == _c; }

	bool Consume(char _c)
	{
		if (Peek(_c))
		{
			++pos_;
			return true;
		}
		return false;
	}

	bool TryConsumeWord(const char* _word)
	{
		if (pos_ >= expr_.Length())
		{
			return false;
		}

		const int len = (int)jc::StringUtil::Length(_word);
		if (expr_.Find(pos_, _word) == pos_)
		{
			pos_ += len;
			return true;
		}
		return false;
	}

	jc::String ReadIdent()
	{
		SkipWs();
		const int start = pos_;

		while (pos_ < expr_.Length())
		{
			const char c = expr_[pos_];
			if (std::isalpha((unsigned char)c) || c == '_'
				|| (pos_ > start && std::isdigit((unsigned char)c)))
			{
				++pos_;
			}
			else
			{
				break;
			}
		}

		return expr_.SubStr(start, pos_ - start);
	}

	// ==================== 문법 규칙 ====================

	UIValue ParseOr()
	{
		UIValue lhs = ParseAnd();
		SkipWs();

		while (TryConsumeWord("||"))
		{
			UIValue rhs = ParseAnd();
			lhs = UIValue::MakeBool(lhs.ToBool() || rhs.ToBool());
			SkipWs();
		}

		return lhs;
	}

	UIValue ParseAnd()
	{
		UIValue lhs = ParseCmp();
		SkipWs();

		while (TryConsumeWord("&&"))
		{
			UIValue rhs = ParseCmp();
			lhs = UIValue::MakeBool(lhs.ToBool() && rhs.ToBool());
			SkipWs();
		}

		return lhs;
	}

	UIValue ParseCmp()
	{
		UIValue lhs = ParseAdd();
		SkipWs();

		while (true)
		{
			bool matched = true;

			if (TryConsumeWord("=="))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(CompareEqual(lhs, rhs));
			}
			else if (TryConsumeWord("!="))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(!CompareEqual(lhs, rhs));
			}
			else if (TryConsumeWord("<="))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(CompareLess(lhs, rhs) || CompareEqual(lhs, rhs));
			}
			else if (TryConsumeWord(">="))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(!CompareLess(lhs, rhs));
			}
			else if (Consume('<'))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(CompareLess(lhs, rhs));
			}
			else if (Consume('>'))
			{
				UIValue rhs = ParseAdd();
				lhs = UIValue::MakeBool(!CompareLess(lhs, rhs) && !CompareEqual(lhs, rhs));
			}
			else
			{
				matched = false;
			}

			SkipWs();
			if (!matched)
			{
				break;
			}
		}

		return lhs;
	}

	static bool CompareEqual(const UIValue& _lhs, const UIValue& _rhs)
	{
		if (_lhs.IsNumber() && _rhs.IsNumber())
		{
			return _lhs.ToFloat() == _rhs.ToFloat();
		}
		return _lhs.ToString() == _rhs.ToString();
	}

	static bool CompareLess(const UIValue& _lhs, const UIValue& _rhs)
	{
		if (_lhs.IsNumber() && _rhs.IsNumber())
		{
			return _lhs.ToFloat() < _rhs.ToFloat();
		}
		return _lhs.ToString() < _rhs.ToString();
	}

	UIValue ParseAdd()
	{
		UIValue lhs = ParseMul();
		SkipWs();

		while (true)
		{
			if (Consume('+'))
			{
				UIValue rhs = ParseMul();
				lhs = AddValues(lhs, rhs);
			}
			else if (Consume('-'))
			{
				UIValue rhs = ParseMul();
				lhs = SubValues(lhs, rhs);
			}
			else
			{
				break;
			}

			SkipWs();
		}

		return lhs;
	}

	static UIValue AddValues(const UIValue& _lhs, const UIValue& _rhs)
	{
		// 문자열/컨테이너/Null은 문자열 연결
		if (!_lhs.IsNumber() || !_rhs.IsNumber())
		{
			return UIValue::MakeString(_lhs.ToString() + _rhs.ToString());
		}

		if (_lhs.IsFloat() || _rhs.IsFloat())
		{
			return UIValue::MakeFloat(_lhs.ToFloat() + _rhs.ToFloat());
		}

		return UIValue::MakeInt(_lhs.ToInt() + _rhs.ToInt());
	}

	static UIValue SubValues(const UIValue& _lhs, const UIValue& _rhs)
	{
		if (_lhs.IsFloat() || _rhs.IsFloat())
		{
			return UIValue::MakeFloat(_lhs.ToFloat() - _rhs.ToFloat());
		}
		return UIValue::MakeInt(_lhs.ToInt() - _rhs.ToInt());
	}

	UIValue ParseMul()
	{
		UIValue lhs = ParseUnary();
		SkipWs();

		while (true)
		{
			if (Consume('*'))
			{
				UIValue rhs = ParseUnary();
				lhs = MulValues(lhs, rhs);
			}
			else if (Consume('/'))
			{
				UIValue rhs = ParseUnary();
				lhs = DivValues(lhs, rhs);
			}
			else if (Consume('%'))
			{
				UIValue rhs = ParseUnary();
				lhs = ModValues(lhs, rhs);
			}
			else
			{
				break;
			}

			SkipWs();
		}

		return lhs;
	}

	static UIValue MulValues(const UIValue& _lhs, const UIValue& _rhs)
	{
		if (_lhs.IsFloat() || _rhs.IsFloat())
		{
			return UIValue::MakeFloat(_lhs.ToFloat() * _rhs.ToFloat());
		}
		return UIValue::MakeInt(_lhs.ToInt() * _rhs.ToInt());
	}

	static UIValue DivValues(const UIValue& _lhs, const UIValue& _rhs)
	{
		const double rhs = _rhs.ToFloat();
		if (rhs == 0.0)
		{
			return UIValue::MakeFloat(0.0);
		}

		const double result = _lhs.ToFloat() / rhs;
		return _lhs.IsFloat() || _rhs.IsFloat() ? UIValue::MakeFloat(result) : UIValue::MakeInt((int64_t)result);
	}

	static UIValue ModValues(const UIValue& _lhs, const UIValue& _rhs)
	{
		const int64_t rhs = _rhs.ToInt();
		if (rhs == 0)
		{
			return UIValue::MakeInt(0);
		}
		return UIValue::MakeInt(_lhs.ToInt() % rhs);
	}

	UIValue ParseUnary()
	{
		SkipWs();

		if (Consume('!'))
		{
			UIValue value = ParseUnary();
			return UIValue::MakeBool(!value.ToBool());
		}

		if (Consume('-'))
		{
			UIValue value = ParseUnary();
			return value.IsFloat() ? UIValue::MakeFloat(-value.ToFloat()) : UIValue::MakeInt(-value.ToInt());
		}

		if (Consume('+'))
		{
			return ParseUnary();
		}

		return ParsePrimary();
	}

	UIValue ParsePrimary()
	{
		SkipWs();

		if (pos_ >= expr_.Length())
		{
			Fail();
			return UIValue::MakeNull();
		}

		const char c = expr_[pos_];

		if (c == '(')
		{
			++pos_;
			UIValue value = ParseOr();
			SkipWs();

			if (!Consume(')'))
			{
				Fail();
			}

			return value;
		}

		if (c == '{')
		{
			const int close = FindMatchingBrace(expr_, pos_);
			if (close < 0)
			{
				Fail();
				return UIValue::MakeNull();
			}

			const jc::String inner = expr_.SubStr(pos_ + 1, close - pos_ - 1);
			pos_ = close + 1;

			// 중괄호 Ref: 내부를 재귀 평가한다. (중첩 접근 {@Items[{@Index}]} 처리)
			return ResolveExpression(inner, *ctx_);
		}

		if (c == '`')
		{
			const int end = expr_.Find(pos_ + 1, "`");
			if (end < 0)
			{
				Fail();
				return UIValue::MakeNull();
			}

			const jc::String text = expr_.SubStr(pos_ + 1, end - pos_ - 1);
			pos_ = end + 1;
			return UIValue::MakeString(text);
		}

		if (std::isdigit((unsigned char)c) || (c == '.' && pos_ + 1 < expr_.Length() && std::isdigit((unsigned char)expr_[pos_ + 1])))
		{
			return ParseNumber();
		}

		// 경로 참조 (@ / $ / 식별자 / 함수 / 키워드)
		PVal base = ParsePathBase();
		if (failed_)
		{
			return UIValue::MakeNull();
		}

		PVal result = ParsePathChain(base);
		if (failed_)
		{
			return UIValue::MakeNull();
		}

		// 체인이 요소에서 끝나면 이름 문자열을 결과로 쓴다.
		if (result.isElement)
		{
			return result.element ? UIValue::MakeString(result.element->getName()) : UIValue::MakeNull();
		}

		return result.value;
	}

	UIValue ParseNumber()
	{
		const int start = pos_;

		while (pos_ < expr_.Length() && (std::isdigit((unsigned char)expr_[pos_]) || expr_[pos_] == '.'))
		{
			++pos_;
		}

		// 지수 표기
		if (pos_ < expr_.Length() && (expr_[pos_] == 'e' || expr_[pos_] == 'E'))
		{
			++pos_;
			if (pos_ < expr_.Length() && (expr_[pos_] == '+' || expr_[pos_] == '-'))
			{
				++pos_;
			}
			while (pos_ < expr_.Length() && std::isdigit((unsigned char)expr_[pos_]))
			{
				++pos_;
			}
		}

		const jc::String number = expr_.SubStr(start, pos_ - start);
		return UIValue::MakeFloat(std::strtod(number.Source(), nullptr));
	}

	// ==================== 경로 참조 ====================

	PVal ParsePathBase()
	{
		SkipWs();

		if (pos_ >= expr_.Length())
		{
			Fail();
			return PVal::MakeValue(UIValue::MakeNull());
		}

		const char c = expr_[pos_];

		if (c == '@')
		{
			++pos_;
			const jc::String key = ReadIdent();

			if (key.IsEmpty())
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			if (ctx_->pData == nullptr)
			{
				return PVal::MakeValue(UIValue::MakeNull());
			}

			const UIValue* pValue = ctx_->pData->Find(key);
			return PVal::MakeValue(pValue ? *pValue : UIValue::MakeNull());
		}

		if (c == '$')
		{
			++pos_;
			return ParseSpecialBase();
		}

		if (c == '#')
		{
			++pos_;
			const jc::String name = ReadIdent();

			if (name.IsEmpty())
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			// #이름: name 기반 요소 조회 (미발견 시 실패 → 문자열 보간 폴백 유도)
			UIElement* pElement = ctx_->pRoot ? ctx_->pRoot->FindElementByName(name.Source()) : nullptr;
			if (pElement == nullptr)
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			return PVal::MakeElement(pElement);
		}

		const jc::String ident = ReadIdent();

		if (ident == "true")
		{
			return PVal::MakeValue(UIValue::MakeBool(true));
		}

		if (ident == "false")
		{
			return PVal::MakeValue(UIValue::MakeBool(false));
		}

		if ((ident == "max" || ident == "min") && Peek('('))
		{
			return PVal::MakeValue(ParseFunction(ident));
		}

		// 접두사 없는 bare 식별자는 요소 조회를 하지 않는다. (# 접두사 필수)
		Fail();
		return PVal::MakeValue(UIValue::MakeNull());
	}

	PVal ParseSpecialBase()
	{
		const jc::String special = ReadIdent();

		if (special == "parent")
		{
			return PVal::MakeElement(ctx_->pCurrent ? ctx_->pCurrent->GetParentElement() : nullptr);
		}

		if (special == "root")
		{
			return PVal::MakeElement(ctx_->pRoot);
		}

		if (special == "self")
		{
			return PVal::MakeElement(ctx_->pCurrent);
		}

		if (special == "prev")
		{
			return PVal::MakeElement(GetSibling(ctx_->pCurrent, -1));
		}

		if (special == "next")
		{
			return PVal::MakeElement(GetSibling(ctx_->pCurrent, +1));
		}

		if (special == "ancestor")
		{
			SkipWs();

			if (!Consume('('))
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			UIValue arg = ParseOr();
			SkipWs();

			if (!Consume(')'))
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			UIElement* pElement = ctx_->pCurrent;
			const int count = (int)arg.ToInt();

			for (int idx = 0; idx < count && pElement != nullptr; ++idx)
			{
				pElement = pElement->GetParentElement();
			}

			return PVal::MakeElement(pElement);
		}

		Fail();
		return PVal::MakeValue(UIValue::MakeNull());
	}

	// 체인 내부의 $ 특수 참조 ($parent.$parent 등). _pFrom 기준으로 이동한다.
	PVal ParseSpecialRelative(UIElement* _pFrom)
	{
		const jc::String special = ReadIdent();

		if (special == "parent")
		{
			return PVal::MakeElement(_pFrom ? _pFrom->GetParentElement() : nullptr);
		}

		if (special == "root")
		{
			return PVal::MakeElement(ctx_->pRoot);
		}

		if (special == "self")
		{
			return PVal::MakeElement(_pFrom);
		}

		if (special == "prev")
		{
			return PVal::MakeElement(GetSibling(_pFrom, -1));
		}

		if (special == "next")
		{
			return PVal::MakeElement(GetSibling(_pFrom, +1));
		}

		if (special == "ancestor")
		{
			SkipWs();

			if (!Consume('('))
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			UIValue arg = ParseOr();
			SkipWs();

			if (!Consume(')'))
			{
				Fail();
				return PVal::MakeValue(UIValue::MakeNull());
			}

			UIElement* pElement = _pFrom;
			const int count = (int)arg.ToInt();

			for (int idx = 0; idx < count && pElement != nullptr; ++idx)
			{
				pElement = pElement->GetParentElement();
			}

			return PVal::MakeElement(pElement);
		}

		Fail();
		return PVal::MakeValue(UIValue::MakeNull());
	}

	PVal ParsePathChain(PVal _base)
	{
		PVal current = _base;

		while (!failed_)
		{
			SkipWs();

			if (pos_ >= expr_.Length())
			{
				break;
			}

			if (expr_[pos_] == '.')
			{
				current = ApplyDot(current);
			}
			else if (expr_[pos_] == '[')
			{
				current = ApplyIndex(current);
			}
			else
			{
				break;
			}
		}

		return current;
	}

	PVal ApplyDot(PVal _current)
	{
		++pos_;	// '.'
		SkipWs();

		if (pos_ >= expr_.Length())
		{
			Fail();
			return _current;
		}

		// $ 특수 참조 체인 ($parent.$parent 등)
		if (expr_[pos_] == '$')
		{
			if (!_current.isElement)
			{
				Fail();
				return _current;
			}

			++pos_;
			return ParseSpecialRelative(_current.element);
		}

		const jc::String ident = ReadIdent();

		if (ident.IsEmpty())
		{
			Fail();
			return _current;
		}

		if (_current.isElement)
		{
			// 1) 자식 요소 이름 조회
			if (UIElement* pChild = FindChildByName(_current.element, ident))
			{
				return PVal::MakeElement(pChild);
			}

			// 2) 요소 프로퍼티
			UIValue property;
			if (ResolveElementProperty(_current.element, ident, property))
			{
				return PVal::MakeValue(property);
			}

			Fail();
			return _current;
		}

		// 값(Map)의 점 접근
		if (_current.value.IsMap())
		{
			const UIValue* pValue = _current.value.GetMapValue(ident);
			return PVal::MakeValue(pValue ? *pValue : UIValue::MakeNull());
		}

		Fail();
		return _current;
	}

	PVal ApplyIndex(PVal _current)
	{
		++pos_;	// '['
		SkipWs();

		UIValue index = ParseOr();
		SkipWs();

		if (!Consume(']'))
		{
			Fail();
			return _current;
		}

		if (_current.isElement)
		{
			const int indexValue = (int)index.ToInt();
			const int count = _current.element ? _current.element->GetChildElementCount() : 0;
			UIElement* pChild = (indexValue >= 0 && indexValue < count)
				? _current.element->GetChildElementAt(indexValue)
				: nullptr;
			return PVal::MakeElement(pChild);
		}

		if (_current.value.IsArray() || _current.value.IsMap())
		{
			const UIValue* pValue = _current.value.GetItem(index);
			return PVal::MakeValue(pValue ? *pValue : UIValue::MakeNull());
		}

		Fail();
		return _current;
	}

	UIValue ParseFunction(const jc::String& _name)
	{
		++pos_;	// '('
		SkipWs();

		std::vector<double> args;

		if (!Consume(')'))
		{
			while (true)
			{
				UIValue arg = ParseOr();
				args.push_back(arg.ToFloat());
				SkipWs();

				if (Consume(')'))
				{
					break;
				}

				if (!Consume(','))
				{
					Fail();
					break;
				}

				SkipWs();
			}
		}

		if (args.empty())
		{
			return UIValue::MakeFloat(0.0);
		}

		double result = args[0];
		const bool isMax = _name == "max";

		for (size_t i = 1; i < args.size(); ++i)
		{
			result = isMax ? std::max(result, args[i]) : std::min(result, args[i]);
		}

		return UIValue::MakeFloat(result);
	}
};

} // namespace

//////////////////////////////////////////////////////////////////////////////////////////
// 요소 프로퍼티 읽기
//////////////////////////////////////////////////////////////////////////////////////////
bool ResolveElementProperty(UIElement* _pElement, const jc::String& _property, UIValue& _out)
{
	if (_pElement == nullptr || _property.IsEmpty())
	{
		return false;
	}

	const jc::String prop = _property;

	if (prop == "Name")
	{
		_out = UIValue::MakeString(_pElement->getName());
		return true;
	}

	if (prop == "Width")
	{
		const float width = _pElement->GetWidth();
		_out = UIValue::MakeFloat(IsAutoSize(width) ? _pElement->GetRenderSize().width : width);
		return true;
	}

	if (prop == "Height")
	{
		const float height = _pElement->GetHeight();
		_out = UIValue::MakeFloat(IsAutoSize(height) ? _pElement->GetRenderSize().height : height);
		return true;
	}

	if (prop == "ActualWidth")
	{
		_out = UIValue::MakeFloat(_pElement->GetRenderSize().width);
		return true;
	}

	if (prop == "ActualHeight")
	{
		_out = UIValue::MakeFloat(_pElement->GetRenderSize().height);
		return true;
	}

	if (prop == "Margin")
	{
		const Thickness& margin = _pElement->GetMargin();
		UIValue map = UIValue::MakeMap();
		map.SetMapValue("Left", UIValue::MakeFloat(margin.left_));
		map.SetMapValue("Top", UIValue::MakeFloat(margin.top_));
		map.SetMapValue("Right", UIValue::MakeFloat(margin.right_));
		map.SetMapValue("Bottom", UIValue::MakeFloat(margin.bottom_));
		_out = map;
		return true;
	}

	if (prop == "IsEnabled")
	{
		_out = UIValue::MakeBool(_pElement->IsEnabled());
		return true;
	}

	if (prop == "Visibility")
	{
		switch (_pElement->GetVisibility())
		{
		case Visibility::Hidden: _out = UIValue::MakeString("Hidden"); break;
		case Visibility::Collapsed: _out = UIValue::MakeString("Collapsed"); break;
		default: _out = UIValue::MakeString("Visible"); break;
		}
		return true;
	}

	if (prop == "Text")
	{
		if (TextBlock* pText = dynamic_cast<TextBlock*>(_pElement))
		{
			_out = UIValue::MakeString(pText->GetText());
			return true;
		}

		if (TextBox* pBox = dynamic_cast<TextBox*>(_pElement))
		{
			const char* pText = pBox->GetText();
			_out = UIValue::MakeString(pText ? pText : "");
			return true;
		}
	}

	if (prop == "IsChecked" || prop == "Checked")
	{
		if (ToggleButton* pToggle = dynamic_cast<ToggleButton*>(_pElement))
		{
			_out = UIValue::MakeBool(pToggle->IsChecked());
			return true;
		}
	}

	if (prop == "Percent")
	{
		if (ProgressBar* pBar = dynamic_cast<ProgressBar*>(_pElement))
		{
			_out = UIValue::MakeFloat(pBar->GetPercent());
			return true;
		}
	}

	if (prop == "Value")
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement))
		{
			_out = UIValue::MakeFloat(pRange->GetValue());
			return true;
		}
	}

	if (prop == "Minimum")
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement))
		{
			_out = UIValue::MakeFloat(pRange->GetMinimum());
			return true;
		}
	}

	if (prop == "Maximum")
	{
		if (RangeBase* pRange = dynamic_cast<RangeBase*>(_pElement))
		{
			_out = UIValue::MakeFloat(pRange->GetMaximum());
			return true;
		}
	}

	if (prop == "RowCount")
	{
		if (ScrollBar* pBar = dynamic_cast<ScrollBar*>(_pElement))
		{
			_out = UIValue::MakeInt(pBar->GetRowCount());
			return true;
		}
	}

	if (prop == "RowPos")
	{
		if (ScrollBar* pBar = dynamic_cast<ScrollBar*>(_pElement))
		{
			_out = UIValue::MakeInt(pBar->GetRowPos());
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
// 공개 API
//////////////////////////////////////////////////////////////////////////////////////////
UIValue ResolveExpression(const jc::String& _expr, const UIResolveContext& _ctx)
{
	ExprParser parser(_expr, _ctx);
	const UIValue result = parser.Parse();
	return parser.Failed() ? UIValue::MakeNull() : result;
}

jc::String ResolveInterpolate(const jc::String& _raw, const UIResolveContext& _ctx)
{
	jc::String out;
	int i = 0;

	while (i < _raw.Length())
	{
		if (_raw[i] == '{')
		{
			const int close = FindMatchingBrace(_raw, i);
			if (close < 0)
			{
				out += _raw.SubStr(i, _raw.Length() - i);
				break;
			}

			const jc::String inner = _raw.SubStr(i + 1, close - i - 1);
			const UIValue value = ResolveExpression(inner, _ctx);
			out += value.ToString();
			i = close + 1;
		}
		else
		{
			out += _raw[i];
			++i;
		}
	}

	return out;
}

UIValue ResolveAttribute(const jc::String& _raw, const UIResolveContext& _ctx)
{
	if (_raw.IsEmpty())
	{
		return UIValue::MakeString("");
	}

	// 중괄호/백틱이 없으면 그대로 문자열 리터럴
	const bool hasBrace = _raw.Find("{") != -1;
	const bool hasTick = _raw.Find("`") != -1;

	if (!hasBrace && !hasTick)
	{
		return UIValue::MakeString(_raw);
	}

	const int length = _raw.Length();

	// 전체가 단일 ` ` 로 감싸진 경우: 문자열 리터럴
	if (length >= 2 && _raw[0] == '`' && _raw[length - 1] == '`')
	{
		return UIValue::MakeString(_raw.SubStr(1, length - 2));
	}

	// 전체가 단일 { } 로 감싸진 경우에만 내부 전체를 표현식으로 평가한다.
	// (중괄호 밖의 연산 토큰은 수식으로 해석하지 않는다 — {a} + 30 미지원)
	if (length >= 2 && _raw[0] == '{' && _raw[length - 1] == '}'
		&& FindMatchingBrace(_raw, 0) == length - 1)
	{
		const jc::String inner = _raw.SubStr(1, length - 2);
		const UIValue result = ResolveExpression(inner, _ctx);

		if (!result.IsNull())
		{
			return result;
		}
	}

	// 중괄호 치환 문자열 보간
	return UIValue::MakeString(ResolveInterpolate(_raw, _ctx));
}

} // namespace sgui


