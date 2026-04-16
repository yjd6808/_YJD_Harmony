using System;
using System.Diagnostics;
using System.Threading;

namespace SGToolsCommon.Base.Progress
{
    public class ProgressUnit
    {
        public static readonly ProgressUnit Default = new(new ProgressEachStrategy());

        protected int slot_;
        protected int cur_;
        protected int max_;
        protected Stopwatch stopwatch_ = new();
        protected TimeSpan reportInterval_;
        protected TimeSpan lastReportedElapsed_;

        public IProgressUnitStrategy Strategy { get; }

        internal int Cur
        {
            get => cur_;
            set => cur_ = value;
        }

        internal int Max
        {
            get => max_;
            set => max_ = value;
        }

        public TimeSpan ReportInterval => reportInterval_;

        internal TimeSpan LastReportedElapsed
        {
            get => lastReportedElapsed_;
            set => lastReportedElapsed_ = value;
        }

        internal Stopwatch Stopwatch    => stopwatch_;
        internal bool IsFinished        => cur_ >= max_;
        internal double Percent         => max_ > 0 ? (double)cur_ / max_ * 100.0 : 0.0;
        internal string ProgressText    => $"{cur_} / {max_} ({Percent:0.00}%)";

        public ProgressUnit(IProgressUnitStrategy _strategy)
        {
            Strategy        = _strategy;
            reportInterval_ = Timeout.InfiniteTimeSpan;
        }

        public ProgressUnit(IProgressUnitStrategy _strategy, TimeSpan _reportInterval)
        {
            Strategy        = _strategy;
            reportInterval_ = _reportInterval;
        }

        internal void Start(int _max)
        {
            max_ = _max;
            stopwatch_.Start();
            Strategy.OnStarted(this);
        }

        internal bool Progress(int _count)
        {
            return Strategy.Progress(this, _count);
        }
    }
}
