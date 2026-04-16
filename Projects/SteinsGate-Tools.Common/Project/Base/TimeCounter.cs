using System;
using System.Collections.Generic;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// 키 기반 경과시간 카운터. UpdateAndCheck로 만료 여부를 체크한다.
    /// </summary>
    public class TimeCounter
    {
        public class Entry
        {
            public TimeSpan elapsed_;
            public bool loop_;
            public bool fired_;
        }

        private Dictionary<int, Entry> counterMap_ = new();

        public bool UpdateAndCheck(int _key, TimeSpan _elapsedTime, int _expirationTimeMiliSec, bool _loop = true) =>
            UpdateAndCheck(_key, _elapsedTime, TimeSpan.FromMilliseconds(_expirationTimeMiliSec), _loop);

        public bool UpdateAndCheck(int _key, TimeSpan _elapsedTime, TimeSpan _expirationTime, bool _loop = true)
        {
            if (!counterMap_.TryGetValue(_key, out Entry? entry))
            {
                entry = new Entry { loop_ = _loop, fired_ = false };
                counterMap_.Add(_key, entry);
            }

            if (!entry.loop_ && entry.fired_)
                return false;

            entry.elapsed_ += _elapsedTime;

            if (entry.elapsed_ >= _expirationTime)
            {
                entry.elapsed_ = TimeSpan.Zero;
                entry.fired_   = true;
                return true;
            }

            return false;
        }
    }
}
