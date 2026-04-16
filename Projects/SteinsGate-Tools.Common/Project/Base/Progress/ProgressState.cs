using System;

namespace SGToolsCommon.Base.Progress
{
    [Flags]
    public enum ProgressState
    {
        None     = 0,
        Reported = 1,
        Finished = 2,
    }
}
