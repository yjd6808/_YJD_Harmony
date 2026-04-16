using System.Diagnostics;
using System.Threading;

namespace SGToolsCommon.Base.Progress
{
    public class ProgressEachStrategy : IProgressUnitStrategy
    {
        public void OnStarted(ProgressUnit _unit)
        {
            // 별도의 시작 로직 없음
        }

        public bool Progress(ProgressUnit _unit, int _count)
        {
            Debug.Assert(_unit.Max > 0,        "작업량(max)이 설정되지 않았습니다.");
            Debug.Assert(_unit.Cur < _unit.Max, "Progress가 정해진 작업량(max)보다 더 많이 실행되었습니다.");

            _unit.Cur += _count;

            if (_unit.ReportInterval == Timeout.InfiniteTimeSpan)
                return true;

            var elapsed = _unit.Stopwatch.Elapsed;
            if (elapsed >= _unit.LastReportedElapsed + _unit.ReportInterval)
            {
                _unit.LastReportedElapsed = elapsed;
                return true;
            }

            return false;
        }
    }
}
