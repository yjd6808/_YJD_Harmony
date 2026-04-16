using System;
using System.Collections.Generic;
using System.Diagnostics;
using SGToolsCommon.Base;

namespace SGToolsCommon.Base.Schedule
{
    public class Scheduler
    {
        public static Scheduler Instance    = new(true);
        public static DateTime CounterCreated = DateTime.Now;

        public Stopwatch counter_                                       = Stopwatch.StartNew();
        private readonly SortedDictionary<long, List<Schedule>> schedules_ = new();
        private List<long> expiredKeys_                                = new();
        private List<Schedule> expiredSchedules_                       = new();
        private object syncRoot_                                       = new();
        private bool threadSafe_                                       = false;

        public Scheduler(bool _threadSafe)
        {
            threadSafe_ = _threadSafe;
        }

        public void Update()
        {
            using var _ = new DisposableLock(threadSafe_ ? syncRoot_ : null!);
            expiredKeys_.Clear();
            expiredSchedules_.Clear();

            TimeSpan now = counter_.Elapsed;

            foreach (var kvp in schedules_)
            {
                if (kvp.Key <= now.Ticks)
                {
                    expiredKeys_.Add(kvp.Key);
                    expiredSchedules_.AddRange(kvp.Value);
                }
                else
                {
                    break;
                }
            }

            foreach (var key in expiredKeys_)
                schedules_.Remove(key);

            foreach (var schedule in expiredSchedules_)
            {
                TimeSpan now2     = counter_.Elapsed;
                TimeSpan deltaTime = now2 - (schedule.ExpireTime - schedule.Interval);
                schedule.Processor.OnScheduled(schedule, deltaTime);

                if (schedule.Loop)
                    Register(schedule);
            }
        }

        public long Register(Schedule _schedule)
        {
            using var _ = new DisposableLock(threadSafe_ ? syncRoot_ : null!);
            TimeSpan now         = counter_.Elapsed;
            TimeSpan expiredTime = now + _schedule.Interval;

            _schedule.ExpireTime = expiredTime;
            long scheduleKey     = expiredTime.Ticks;
            if (!schedules_.ContainsKey(scheduleKey))
                schedules_[scheduleKey] = new List<Schedule>();
            schedules_[scheduleKey].Add(_schedule);
            return scheduleKey;
        }

        public long Register(int _id, TimeSpan _interval, IScheduleProcessor? _processor = null, bool _loop = true, object? _data = null)
        {
            Schedule schedule = Schedule.Create(_id, _interval, _processor, _loop);
            schedule.Data = _data;
            return Register(schedule);
        }

        public bool Remove(long _scheduleKey, int _id)
        {
            using var _ = new DisposableLock(threadSafe_ ? syncRoot_ : null!);
            if (schedules_.TryGetValue(_scheduleKey, out List<Schedule>? schedules))
                return schedules.RemoveAll(_sche => _sche.Id == _id) > 0;
            return false;
        }
    }
}
