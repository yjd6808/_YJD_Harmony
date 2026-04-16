using System.Diagnostics;
using System.Threading;

namespace SGToolsCommon.Base.Progress
{
    internal class ProgressPercentStrategy : IProgressUnitStrategy
    {
        private readonly float percent_;
        private float quantity_;
        private float nextQuantity_;

        public ProgressPercentStrategy(float _percent)
        {
            percent_ = _percent;
        }

        public void OnStarted(ProgressUnit _unit)
        {
            Debug.Assert(percent_ >= 0.1f, "알림 퍼센트가 0.1%보다 작으면 안됩니다.");
            quantity_     = _unit.Max * percent_ * 0.01f;
            nextQuantity_ = quantity_;
        }

        public bool Progress(ProgressUnit _unit, int _count)
        {
            Debug.Assert(_unit.Max > 0,        "작업량(max)이 설정되지 않았습니다.");
            Debug.Assert(_unit.Cur < _unit.Max, "Progress가 정해진 작업량(max)보다 더 많이 실행되었습니다.");

            _unit.Cur += _count;

            if (_unit.Cur >= _unit.Max)
                return true;

            if (_unit.ReportInterval == Timeout.InfiniteTimeSpan)
                return CalcNextQuantity(_unit);

            var elapsed = _unit.Stopwatch.Elapsed;
            if (elapsed >= _unit.LastReportedElapsed + _unit.ReportInterval)
            {
                _unit.LastReportedElapsed = elapsed;
                return true;
            }

            return CalcNextQuantity(_unit);
        }

        private bool CalcNextQuantity(ProgressUnit _unit)
        {
            if (_unit.Cur <= nextQuantity_)
                return false;
            nextQuantity_ += quantity_;
            return true;
        }
    }
}
