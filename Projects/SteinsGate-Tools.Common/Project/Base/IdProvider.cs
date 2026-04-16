using System.Collections.Generic;
using System.Diagnostics;

namespace SGToolsCommon.Base
{
    public interface IIdProvider
    {
        int     Provide();
        void    Release(int _id);
    }

    /// <summary>
    /// 해제된 ID를 재사용 풀에 보관하고 재할당하는 ID 제공자.
    /// </summary>
    public class ReusableIdProvider : IIdProvider
    {
        private LinkedList<int> freeList_ = new();
        private List<int> usingList_ = new();
        private int maxId_ = 0;
        private object lock_;

        public ReusableIdProvider(object? _parentLock = null)
        {
            lock_ = _parentLock ?? new object();
        }

        public int Provide()
        {
            using var _ = new DisposableLock(lock_);
            return ProvideRaw();
        }

        public int ProvideRaw()
        {
            int allocId;
            if (freeList_.Count > 0)
            {
                var node = freeList_.First!;
                allocId = node.Value;
                freeList_.RemoveFirst();
            }
            else
            {
                allocId = ++maxId_;
            }

            usingList_.Add(allocId);
            return allocId;
        }

        public void ReleaseRaw(int _id)
        {
            if (!IsUsing(_id))
            {
                Debug.Assert(false);
                return;
            }

            freeList_.AddFirst(_id);
            usingList_.Remove(_id);
        }

        public void Release(int _id)
        {
            using var _ = new DisposableLock(lock_);
            ReleaseRaw(_id);
        }

        public bool IsUsing(int _id)
        {
            using var _ = new DisposableLock(lock_);
            return usingList_.Contains(_id);
        }
    }
}
