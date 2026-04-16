using System;

namespace SGToolsCommon.Base
{
    public delegate void ActionRef<T>(ref T _value);
    public delegate TResult FuncRef<T, out TResult>(ref T _value);

    /// <summary>
    /// C++ std::optional 패턴 구현. 값의 존재 여부를 명시적으로 관리한다.
    /// </summary>
    public class Optional<T>
    {
        private bool assigned_;
        private bool assignable_;
        private T? value_;

        public Optional(bool _assignable = true)
        {
            assigned_ = false;
            value_ = default;
            assignable_ = _assignable;
        }

        public Optional(T _value, bool _assignable = true)
        {
            assigned_ = true;
            value_ = _value;
            assignable_ = _assignable;
        }

        public bool Assigned => assigned_;
        public bool Assignable => assignable_;

        public T? Value
        {
            get
            {
                if (!assigned_)
                    return default;
                return value_;
            }
            set
            {
                if (!assignable_)
                    return;
                assigned_ = true;
                value_ = value;
            }
        }

        public static Optional<T?> Create(T _value) => new(_value);
        public static Optional<T> None() => new();

        public void Do(ActionRef<T> _action)
        {
            if (assigned_)
            {
                _action(ref value_!);
                return;
            }
            throw new Exception("어싸인 안댐");
        }

        public bool TryDo(ActionRef<T> _action)
        {
            if (assigned_)
            {
                _action(ref value_!);
                return true;
            }
            return false;
        }

        public TResult? Do<TResult>(FuncRef<T, TResult> _func)
        {
            if (assigned_)
                return _func(ref value_!);
            throw new Exception("어싸인 안댐");
        }

        public bool TryDo<TResult>(FuncRef<T, TResult> _func, out TResult? _result)
        {
            if (assigned_)
            {
                _result = _func(ref value_!);
                return true;
            }
            _result = default;
            return false;
        }
    }
}
