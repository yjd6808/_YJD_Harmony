using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading;
using SGToolsCommon.Extension;

namespace SGToolsCommon.Base.Progress
{
    public class ProgressNotifier
    {
        private readonly IProgressListener listener_;
        private readonly List<ProgressUnit> units_;
        private int isInterruptRequested_;

        public int Count => units_.Count;
        public bool IsThreadSafe { get; set; }

        public bool IsInterruptRequested
        {
            get => Volatile.Read(ref isInterruptRequested_) == 1;
            set => Interlocked.Exchange(ref isInterruptRequested_, value ? 1 : 0);
        }

        public ProgressUnit First  => GetUnit(0);
        public ProgressUnit Second => GetUnit(1);
        public ProgressUnit Third  => GetUnit(2);

        public ProgressUnit this[int _slot] => GetUnit(_slot);

        public ProgressNotifier(IProgressListener _listener)
        {
            listener_ = _listener;
            units_    = new List<ProgressUnit>();
        }

        public void AddUnit(ProgressUnit _unit)
        {
            units_.Add(_unit);
        }

        public void Start(params int[] _maxs)
        {
            if (IsInterruptRequested)
                throw new InterruptException();

            if (_maxs.Length != units_.Count)
                throw new ArgumentException("Parameter count does not match unit count.");

            bool isThreadSafe = IsThreadSafe;
            for (int i = 0; i < _maxs.Length; ++i)
            {
                var unit = units_[i];
                LockEx.DoIf(isThreadSafe, unit, () => unit.Start(_maxs[i]));
            }
        }

        public ProgressState Progress()                         => Progress(0, 1);
        public ProgressState Progress(int _slot)                => Progress(_slot, 1);
        public ProgressState Progress(int _slot, int _count)
        {
            bool isThreadSafe = IsThreadSafe;
            if (IsInterruptRequested)
                throw new InterruptException();

            if (_slot < 0 || _slot >= units_.Count)
                throw new ArgumentOutOfRangeException(nameof(_slot));

            var unit = units_[_slot];
            if (unit == null)
                return ProgressState.None;

            return LockEx.DoIf(isThreadSafe, unit, () =>
            {
                bool reported = unit.Progress(_count);
                var state = ProgressState.None;

                if (unit.IsFinished)
                    state |= ProgressState.Finished;

                if (reported && unit.Max != int.MaxValue)
                {
                    state |= ProgressState.Reported;
                    Report(_slot);
                }
                return state;
            });
        }

        public void Report(int _slot)
        {
            if (_slot < 0 || _slot >= units_.Count)
                throw new ArgumentOutOfRangeException(nameof(_slot));

            var unit = units_[_slot];
            LockEx.DoIf(IsThreadSafe, unit, () =>
                Debug.Assert(unit.Cur <= unit.Max));
            listener_.OnReported(_slot);
        }

        public class InterruptException : Exception {}

        public bool IsFinished(int _slot)       => LockEx.DoIf(IsThreadSafe, GetUnit(_slot), () => GetUnit(_slot).IsFinished);
        public double GetPercent(int _slot)     => LockEx.DoIf(IsThreadSafe, GetUnit(_slot), () => GetUnit(_slot).Percent);
        public string GetProgressText(int _slot) => LockEx.DoIf(IsThreadSafe, GetUnit(_slot), () => GetUnit(_slot).ProgressText);

        private ProgressUnit GetUnit(int _slot)
        {
            if (_slot >= 0 && _slot < units_.Count)
                return units_[_slot];
            Debug.Assert(false, "유닛이 없음");
            return ProgressUnit.Default;
        }
    }
}
