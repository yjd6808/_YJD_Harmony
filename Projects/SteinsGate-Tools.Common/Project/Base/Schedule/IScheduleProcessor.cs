using System;

namespace SGToolsCommon.Base.Schedule
{
    public delegate void ScheduleHandler(Schedule _schedule, TimeSpan _deltaTime);

    public interface IScheduleProcessor
    {
        void OnScheduled(Schedule _schedule, TimeSpan _deltaTime);
    }
}
