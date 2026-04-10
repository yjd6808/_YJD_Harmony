/*
 * 작성자: 윤정도
 * 작성일: 2/26/2023 6:37:14 AM
 *
 */

using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace SGToolsCommon
{
    public enum PropertyReflect
    {
        Update,
        DonUpdate
    }

    public class Bindable : INotifyPropertyChanged
    {
        [Browsable(false)]
        public bool IsNotifyEnabled { get; set; } = true;

        public event PropertyChangedEventHandler? PropertyChanged;

        //////////////////////////////////////////////////////////////////////////////////
        protected virtual void OnPropertyChanged([CallerMemberName] string? _propertyName = null)
        {
            if (!IsNotifyEnabled)
                return;

            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(_propertyName));
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void NotifyProperty(string _propertyName)
        {
            OnPropertyChanged(_propertyName);
        }

        //////////////////////////////////////////////////////////////////////////////////
        protected bool SetField<T>(ref T _field, T _value, [CallerMemberName] string? _propertyName = null)
        {
            if (EqualityComparer<T>.Default.Equals(_field, _value))
                return false;

            _field = _value;

            if (IsNotifyEnabled)
                OnPropertyChanged(_propertyName);

            return true;
        }
    }
}
