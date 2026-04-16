/*
 * 작성자: 윤정도
 * 생성일: 3/9/2023 12:34:42 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Linq;

namespace SGToolsCommon.Customize.Control
{
    public class RTBLogWriter
    {
        private readonly RTBLogViewer viewer_;
        private RTBTextOption baseOption_;
        private readonly List<RTBLogItemElement> elements_;
        private int type_;

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogWriter(RTBLogViewer _viewer)
        {
            viewer_     = _viewer;
            baseOption_ = new RTBTextOption();
            elements_   = new List<RTBLogItemElement>();
            type_       = 0;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 이후 모든 .Text() / .Link() 호출에 기본 옵션으로 적용
        public RTBLogWriter Option(RTBTextOption? _option)
        {
            baseOption_ = _option?.Clone() ?? new RTBTextOption();
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 기본 옵션으로 추가
        public RTBLogWriter Text(string _text)
        {
            elements_.Add(new RTBLogItemElement(_text, baseOption_.Clone()));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 기본 옵션에서 Decoration만 교체
        public RTBLogWriter Text(string _text, RTBTextDecoration _decoration)
        {
            RTBTextOption merged = baseOption_.Clone();
            merged.Decoration = _decoration;
            elements_.Add(new RTBLogItemElement(_text, merged));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 기본 옵션에 override 옵션을 병합 (null/NaN/None 필드는 base 값 유지)
        public RTBLogWriter Text(string _text, RTBTextOption? _option)
        {
            RTBTextOption merged = baseOption_.Clone();
            merged.MergeFrom(_option);
            elements_.Add(new RTBLogItemElement(_text, merged));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // IsLink=true 자동 설정 – InlineUIContainer + TextBlock으로 렌더링 (손 커서 + 밑줄)
        public RTBLogWriter Link(string _text)
        {
            RTBTextOption merged = baseOption_.Clone();
            merged.IsLink = true;
            elements_.Add(new RTBLogItemElement(_text, merged));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // IsLink=true + Click 콜백 연결
        public RTBLogWriter Link(string _text, Action _click)
        {
            RTBTextOption merged = baseOption_.Clone();
            merged.IsLink = true;
            merged.Click  = _click;
            elements_.Add(new RTBLogItemElement(_text, merged));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 전달된 옵션에 IsLink=true를 강제 적용 후 추가
        public RTBLogWriter Link(string _text, RTBTextOption? _option)
        {
            RTBTextOption merged = baseOption_.Clone();
            merged.MergeFrom(_option);
            merged.IsLink = true;  // 반드시 IsLink=true 강제
            elements_.Add(new RTBLogItemElement(_text, merged));
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public RTBLogWriter SetType(int _type)
        {
            type_ = _type;
            return this;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Submit()
        {
            RTBLogItem item = new RTBLogItem(elements_.ToList(), type_);
            viewer_.AddItem(item);

            // 다음 체이닝을 위해 상태 초기화
            baseOption_ = new RTBTextOption();
            elements_.Clear();
            type_ = 0;
        }
    }
}
