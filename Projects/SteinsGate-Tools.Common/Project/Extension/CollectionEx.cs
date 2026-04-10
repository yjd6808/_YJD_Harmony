/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 4:44:35 PM
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Controls;

namespace SGToolsCommon.Extension
{
    public static class CollectionEx
    {
        //////////////////////////////////////////////////////////////////////////////////
        public static IEnumerable<T> Reversed<T>(this ObservableCollection<T> _collection) where T : class
        {
            for (int i = _collection.Count - 1; i >= 0; --i)
                yield return _collection[i];
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void IndexingForEach<T>(this IList<T> _collection, Action<int, T> _action) where T : class
        {
            for (int i = 0; i < _collection.Count; ++i)
                _action(i, _collection[i]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 반대로 인덱스와 함께 순회
        public static void ReversedIndexingForEach<T>(this IList<T> _collection, Action<int, T> _action) where T : class
        {
            for (int i = _collection.Count - 1; i >= 0; --i)
                _action(i, _collection[i]);
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 반대로 조건이 true일때까지만 순회
        // return true가 break이고
        // return false가 continue라고 생각하면된다.
        public static void ReversedConditionForEach<T>(this IList<T> _collection, Predicate<T> _predicate) where T : class
        {
            for (int i = _collection.Count - 1; i >= 0; --i)
                if (!_predicate(_collection[i]))
                    return;
        }

        //////////////////////////////////////////////////////////////////////////////////
        // 반대로 인덱스와 함께 조건이 true일때까지만 순회
        public static void ReversedIndexingConditionForEach<T>(this IList<T> _collection, Func<int, T, bool> _predicate) where T : class
        {
            for (int i = _collection.Count - 1; i >= 0; --i)
                if (!_predicate(i, _collection[i]))
                    return;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static ObservableCollection<T> InsertRangeNew<T>(
            this ObservableCollection<T> _collection, int _index, IList<T> _elements)
            where T : class
        {
            ObservableCollection<T> newCollection = new(new List<T>(_collection.Count + _elements.Count + 16));

            if (_index > _collection.Count)
                _index = _collection.Count;

            for (int i = 0; i < _index; ++i)
                newCollection.Add(_collection[i]);

            for (int i = 0; i < _elements.Count; ++i)
                newCollection.Add(_elements[i]);

            for (int i = _index; i < _collection.Count; ++i)
                newCollection.Add(_collection[i]);

            return newCollection;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static ObservableCollection<T> AddAfterRangeNew<T>(
            this ObservableCollection<T> _collection, int _index, IList<T> _elements)
            where T : class
            => InsertRangeNew(_collection, _index + 1, _elements);

        //////////////////////////////////////////////////////////////////////////////////
        public static void AddRange<T>(
            this ObservableCollection<T> _collection, IList<T> _elements)
            where T : class
        {
            foreach (T element in _elements)
                _collection.Add(element);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ForEach<T>(this IEnumerable<T> _collection, Action<int, T> _action) where T : class
        {
            int i = 0;
            foreach (T item in _collection)
                _action(i++, item);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static bool Exist(this ItemCollection _collection, object _item)
        {
            foreach (object o in _collection)
                if (o == _item)
                    return true;

            return false;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IEnumerable<object> Where(this ItemCollection _collection, Predicate<object> _predicate)
        {
            foreach (object o in _collection)
                if (_predicate(o))
                    yield return o;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IEnumerable<T> Where<T>(this ItemCollection _collection, Predicate<T> _predicate) where T : class
        {
            foreach (object o in _collection)
            {
                T t = o as T;
                if (t == null) throw new NullReferenceException($"{typeof(T).Name}으로 변환되지 않는 타입입니다.");
                if (_predicate(t))
                    yield return t;
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ForEach(this ItemCollection _collection, Action<object> _action)
        {
            foreach (object o in _collection)
                _action(o);
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static void ForEach<T>(this ItemCollection _collection, Action<T> _action) where T : class
        {
            foreach (object o in _collection)
            {
                T t = o as T;
                if (t == null) throw new NullReferenceException($"{typeof(T).Name}으로 변환되지 않는 타입입니다.");
                _action(t);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IEnumerable<T> Cast<T>(this IList _list) where T : class
        {
            foreach (object o in _list)
            {
                T t = o as T;
                if (t == null) throw new InvalidCastException("캐스팅 실패");
                yield return t;
            }
        }
    }
}
