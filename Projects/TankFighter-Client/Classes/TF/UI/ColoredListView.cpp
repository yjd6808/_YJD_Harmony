#include <TF/UI/ColoredListView.h>

#define SCROLLVIEW_BOX				0	

ColoredListView * ColoredListView::create(const Color4B& color)
{
	ColoredListView *ret = new (std::nothrow) ColoredListView();
	if (ret &&ret->init() && ret->initWithColor(color))
	{
		ret->autorelease();
		return ret;
	}
	else
	{
		CC_SAFE_DELETE(ret);
		return nullptr;
	}
}

bool ColoredListView::init()
{
	if (!Layer::init())
		return false;

	m_pListView = ListView::create();
	m_pListView->setDirection(ScrollView::Direction::VERTICAL);
	m_pListView->setContentSize(this->getContentSize());
	m_pListView->setScrollBarEnabled(true);
	m_pListView->setScrollBarAutoHideEnabled(false);
	m_pListView->setAnchorPoint(Vec2::ZERO);
	m_pListView->addEventListener(CC_CALLBACK_2(ColoredListView::OnScrollViewEvent, this));
	this->addChild(m_pListView);

	return true;
}

void ColoredListView::SetContantSize(const Size& size) {
	this->setContentSize(size);
	m_pListView->setContentSize(size);
}

void ColoredListView::OnScrollViewEvent(Ref* pref, ScrollView::EventType type) {
	CCLOG("%d\n", type);
}

/*
void ColoredListView::InsertErrorLog(std::string str)
{
	DFPlayerChatLog debug;
	strcpy(debug.mNick, "Log");
	strcpy(debug.mContent, str.c_str());
	InsertChatLog(debug);
}
EditBox* sendEditBox = EditBox::create(Size(size.width - 100, 30), Scale9Sprite::create("blank.png"));
	sendEditBox->setFontColor(Color4B::WHITE);
	sendEditBox->setFontSize(18.0f);
	sendEditBox->setPlaceHolder("ä���� �Է��ϼ���");
	sendEditBox->setAnchorPoint(Vec2::ZERO);
	sendEditBox->setPlaceholderFontColor(Color4B::GRAY);
	sendEditBox->setInputMode(EditBox::InputMode::ANY);
	this->addChild(sendEditBox, 0, EDITBOX_INPUT_BOX);


*/
/*
void ColoredListView::InsertChatLog(DFPlayerChatLog log)
{
	ScrollView* scrollView = (ScrollView*)this->getChildByTag(SCROLLVIEE_CHATVIEWER);
	float nickSizeX = 80.0f; //�г��� ���α���
	float textSizeX = this->m_ViewerSize.width - nickSizeX; //�ؽ�Ʈ ���α���
	const float oneWordSizeX = 10.555f;	//�ѱ� ���ڴ� ���α���
	const int nextLineLimit = textSizeX / oneWordSizeX;	//���ٿ� �ѱ� ���ڰ� ���� �������̵Ǵ���
	const int expandLimitLineCount = (int)(this->m_ViewerSize.height / 15.0f);	//�� ��ũ�Ѻ䰡 �����̻��� ��� ù Ȯ��Ǿ��ϴ���
	int expandLineNum = 2;
	int lineCount = getLineCount(); //���� �ټ�

	Text

	Size modContainerSize = scrollView->getInnerContainerSize();
	Text* nickText = Text::create(log.mNick, DF_FONT_PATH, 10);
	nickText->setAnchorPoint(Vec2(0.0f, 1.0f));
	nickText->setTextAreaSize(Size(80.0f, 12.5f * (float)expandLineNum));
	nickText->enableOutline(Color4B::BLACK);
	nickText->setPosition(Vec2(0.0f, modContainerSize.height - 20 - (15.0f * lineCount)));
	scrollView->addChild(nickText);

	auto fsdf = this->m_ViewerSize.width - 80.0f;
	Text* text = Text::create(log.mContent, DF_FONT_PATH, 10);
	text->setAnchorPoint(Vec2(0.0f, 1.0f));
	text->setTextAreaSize(Size(this->m_ViewerSize.width - 100.0f, 15.0f * (float)expandLineNum));
	text->setPosition(Vec2(80.0f, modContainerSize.height - 20 - (15.0f * lineCount)));
	scrollView->addChild(text);
}

*/