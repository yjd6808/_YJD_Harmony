using System;

namespace SGToolsCommon.Base.Schedule
{
    public class Schedule : IScheduleProcessor
    {
        public int Id                     { get; private set; }
        public TimeSpan Interval          { get; private set; }
        public IScheduleProcessor Processor { get; private set; } = null!;
        public event ScheduleHandler? ScheduleHandler;
        public TimeSpan ExpireTime        { get; set; }
        public DateTime CallTime          => Scheduler.CounterCreated + ExpireTime;
        public bool Loop                  { get; private set; }
        public object? Data               = null;

        private Schedule() {}

        public static Schedule Create(int _id, TimeSpan _interval, IScheduleProcessor? _processor = null, bool _loop = true)
        {
            Schedule schedule = new Schedule();
            schedule.Id        = _id;
            schedule.Interval  = _interval;
            schedule.Processor = _processor ?? schedule;
            schedule.Loop      = _loop;
            return schedule;
        }

        public void OnScheduled(Schedule _schedule, TimeSpan _deltaTime)
        {
            ScheduleHandler?.Invoke(_schedule, _deltaTime);
        }
    }
}
