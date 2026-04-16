/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 4:44:35 PM
 *
 */

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Windows.Controls;
using Newtonsoft.Json.Linq;
using SGToolsCommon.Base;

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
                T? t = o as T;
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
                T? t = o as T;
                if (t == null) throw new NullReferenceException($"{typeof(T).Name}으로 변환되지 않는 타입입니다.");
                _action(t);
            }
        }

        //////////////////////////////////////////////////////////////////////////////////
        public static IEnumerable<T> Cast<T>(this IList _list) where T : class
        {
            foreach (object o in _list)
            {
                T? t = o as T;
                if (t == null) throw new InvalidCastException("캐스팅 실패");
                yield return t;
            }
        }

        // @출처: https://stackoverflow.com/questions/9337039/why-is-there-no-reverseenumerator-in-c
        public static IEnumerable<T> ReverseEx<T>(this IEnumerable<T> _coll)
        {
            var quick = _coll as IList<T>;
            if (quick == null)
            {
                foreach (T item in _coll.Reverse())
                    yield return item;
            }
            else
            {
                for (int ix = quick.Count - 1; ix >= 0; --ix)
                {
                    yield return quick[ix];
                }
            }
        }

        public static void AddRange<T>(this ICollection<T> _collection, ICollection<T> _values)
        {
            foreach (var value in _values)
                _collection.Add(value);
        }

        public static int RemoveAllIf<T>(this LinkedList<T> _list, Predicate<T> _predicate)
        {
            if (_list == null)
                throw new ArgumentNullException(nameof(_list));

            if (_predicate == null)
                throw new ArgumentNullException(nameof(_predicate));

            var count = 0;
            var node = _list.First;
            while (node != null)
            {
                var next = node.Next;
                if (_predicate(node.Value))
                {
                    _list.Remove(node);
                    count++;
                }

                node = next;
            }

            return count;
        }

        public static int AddRange<T>(this List<T> _list, List<T> _otherList, int _offset, int _count)
        {
            if (_otherList == null)
            {
                Debug.Assert(false);
                return 0;
            }

            if (_count <= 0)
            {
                Debug.Assert(false);
                return 0;
            }

            if (_offset >= _otherList.Count)
            {
                Debug.Assert(false);
                return 0;
            }

            int copyCount = 0;

            for (int i = _offset; i < _otherList.Count; ++i)
            {
                _list.Add(_otherList[i]);
                copyCount++;

                if (copyCount >= _count)
                    break;
            }

            return copyCount;
        }

        public static List<T> ToList<T>(this IEnumerator _enumerator)
        {
            var list = new List<T>();
            while (_enumerator.MoveNext())
            {
                if (_enumerator.Current is T item)
                {
                    list.Add(item);
                }
                else
                {
                    Debug.Assert(false, "이상한 타입으로 변환할라캄");
                    break;
                }
            }

            return list;
        }

        public static int GetLength(this IEnumerable _enumerable)
        {
            if (_enumerable is Array array)
            {
                return array.Length;
            }

            int count = 0;
            foreach (var _ in _enumerable)
            {
                ++count;
            }

            return count;
        }

        public static T? NextOfOrDefault<T>(this IEnumerable<T> _enumerable, T _item, T? _default = default)
        {
            bool found = false;
            foreach (var _ in _enumerable)
            {
                if (found)
                {
                    return _;
                }

                if (_ != null && _.Equals(_item))
                {
                    found = true;
                }
            }

            return _default;
        }

        // 교집합 원소들을 predicate(교집합 조건)로 검출 후, union 원소들을 대상으로 action 수행
        public static void ForEachUnionMinus<T>(
            this ICollection<T> _srcEnumerable,
            ICollection<T> _dstEnumerable,
            Func<T, T, bool> _predicate,
            Action<T, T>? _unionAction = null,
            Action<T, bool>? _minusAction = null)
        {
            if (_unionAction == null && _minusAction == null)
                throw new Exception("둘다 널이면 안댐;");

            Dictionary<int, T> srcRemovedDict = new Dictionary<int, T>();
            Dictionary<int, T> dstRemovedDict = new Dictionary<int, T>();

            int i = 0;
            int j = 0;

            foreach (T src in _srcEnumerable)
            {
                if (srcRemovedDict.ContainsKey(i))
                    continue;

                j = 0;

                foreach (T dst in _dstEnumerable)
                {
                    if (dstRemovedDict.ContainsKey(j))
                        continue;

                    if (_predicate(src, dst))
                    {
                        if (_unionAction != null)
                            _unionAction(src, dst);

                        srcRemovedDict.Add(i, src);
                        dstRemovedDict.Add(j, dst);
                        break;
                    }

                    ++j;
                }

                ++i;
            }

            if (_minusAction == null)
                return;

            i = 0;
            foreach (T src in _srcEnumerable)
            {
                if (srcRemovedDict.ContainsKey(i)) // 교집합 소스
                    continue;
                _minusAction(src, true);
                ++i;
            }

            i = 0;
            foreach (T dst in _dstEnumerable)
            {
                if (dstRemovedDict.ContainsKey(i)) // 교집합 데스트
                    continue;
                _minusAction(dst, false);
                ++i;
            }
        }

        public static void ForEach<T>(this IEnumerator _enumerator, Action<T> _action)
        {
            while (_enumerator.MoveNext())
            {
                if (_enumerator.Current is T item)
                {
                    _action(item);
                }
                else
                {
                    Debug.Assert(false, "이상한 타입으로 변환할라캄");
                    break;
                }
            }
        }

        public static void ForEach<T>(this IEnumerable<T> _source, Action<T> _action)
        {
            foreach (var item in _source)
            {
                _action(item);
            }
        }

        public static void ForEach<T>(this IEnumerable<T> _source, Action<T, int> _action)
        {
            int idx = 0;
            foreach (var item in _source)
            {
                _action(item, idx++);
            }
        }

        public static int IndexOf<T>(this IEnumerable<T> _source, T _value)
        {
            int index = 0;
            var comparer = EqualityComparer<T>.Default;
            foreach (T item in _source)
            {
                if (comparer.Equals(item, _value))
                {
                    return index;
                }

                index++;
            }

            return -1;
        }

        public static bool HasItem(this IEnumerable _source)
        {
            var enumerator = _source.GetEnumerator();
            return enumerator.MoveNext();
        }

        public static JArray SaveAsJArray<T>(this IEnumerable<T> _list)
        {
            JArray jarray = new JArray();

            if (typeof(T) == typeof(string) || typeof(T).IsPrimitive)
            {
                foreach (var item in _list)
                    jarray.Add(item);
                return jarray;
            }

            if (typeof(T).IsEnum)
            {
                foreach (var item in _list)
                    jarray.Add(item?.ToString());
                return jarray;
            }

            Debug.Assert(typeof(ISerializableToJToken).IsAssignableFrom(typeof(T)));
            foreach (var item in _list)
            {
                var serializable = item as ISerializableToJToken;
                if (serializable == null)
                    throw new Exception("ISerializableToJObject를 상속받지 않은 개체임");
                JToken obj = serializable.Serialize();
                jarray.Add(obj);
            }

            return jarray;
        }

        // SaveAsJArray는 무조건 배열로 반환하고, 요건 1개만 있는 경우 그냥 토큰으로 반환함.
        public static JToken? SaveAsJToken<T>(this IEnumerable<T> _list, JToken? _default = null)
        {
            var jarray = _list.SaveAsJArray();
            if (jarray.Count == 0)
                return _default;
            if (jarray.Count == 1)
                return jarray[0];
            return jarray;
        }

        public static T First<T>(this IEnumerator _enumerator)
        {
            if (_enumerator.MoveNext())
            {
                return (T)_enumerator.Current;
            }

            return default!;
        }

        public static bool IsIndexOutOfRange(this ICollection _collection, int _index)
        {
            return _index >= _collection.Count || _index < 0;
        }

        public static T? Find<T>(this IList<T> _list, Predicate<T> _match)
        {
            if (_list == null)
                throw new ArgumentNullException(nameof(_list));
            if (_match == null)
                throw new ArgumentNullException(nameof(_match));

            foreach (var item in _list)
            {
                if (_match(item))
                    return item;
            }

            return default;
        }

        public static bool RemoveLast<T>(this IList<T> _list)
        {
            if (_list.Count == 0)
                return false;

            _list.RemoveAt(_list.Count - 1);
            return true;
        }

        public static void PopNoReturn<T>(this List<T> _list)
        {
            if (_list.Count == 0)
                return;
            _list.RemoveAt(_list.Count - 1);
        }

        public static T Pop<T>(this List<T> _list)
        {
            if (_list.Count == 0)
                return default!;

            T item = _list[_list.Count - 1];
            _list.RemoveAt(_list.Count - 1);
            return item;
        }

        public static bool TryPop<T>(this List<T> _list, out T _value)
        {
            _value = default!;
            if (_list.Count == 0)
                return false;

            _value = _list[_list.Count - 1];
            _list.RemoveAt(_list.Count - 1);
            return true;
        }

        public static void Push<T>(this List<T> _list, T _item) => _list.Add(_item);
        public static T Top<T>(this List<T> _list)
        {
            if (_list.Count == 0)
                return default!;
            return _list[_list.Count - 1];
        }

        public static IEnumerable<T> Yield<T>(this T item)
        {
            yield return item;
        }

        public static void BubbleForEach<T>(this IList<T> _source, Action<T, T> _action)
        {
            for (int i = 0; i < _source.Count; i++)
            {
                for (int j = i + 1; j < _source.Count; ++j)
                {
                    _action(_source[i], _source[j]);
                }
            }
        }

        public static string ConcatRange(this IList<string> _list, int _index, int _range, Func<int, string>? _headerGen = null)
        {
            // 범위내에 없음
            if (_list.Count <= _index)
            {
                Debug.Assert(false);
                return string.Empty;
            }

            int min = Math.Max(_index - _range, 0);
            int max = Math.Min(_index + _range, _list.Count - 1);

            StringBuilder builder = new StringBuilder(512);
            for (int i = min; i <= max; ++i)
            {
                if (_headerGen != null)
                {
                    builder.AppendLine($"{_headerGen(i)}{_list[i]}");
                }
                else
                {
                    builder.AppendLine(_list[i]);
                }
            }
            return builder.ToString();
        }

        public static void PushRange<T>(this Stack<T> _stack, IEnumerable<T> _children)
        {
            foreach (T v in _children)
                _stack.Push(v);
        }

        public static void Push<T>(this Queue<T> _queue, T _value) => _queue.Enqueue(_value);
        public static void PushRange<T>(this Queue<T> _queue, IEnumerable<T> _children)
        {
            foreach (T v in _children)
                _queue.Push(v);
        }

        public static T Pop<T>(this Queue<T> _queue) => _queue.Dequeue();

        public static IEnumerable<int> ToIndexEnumerable(this int _max)
        {
            for (int i = 0; i < _max; ++i)
                yield return i;
        }

        public static T? TryPeek<T>(this Stack<T> stack)
        {
            if (stack == null) throw new ArgumentNullException(nameof(stack));
            if (stack.Count == 0)
            {
                return default;
            }
            return stack.Peek();
        }

        public static T? TryPeek<T>(this List<T> list)
        {
            if (list == null) throw new ArgumentNullException(nameof(list));
            if (list.Count == 0)
            {
                return default;
            }
            return list[list.Count - 1];
        }

        public static T? FirstPrevOrDefault<T>(
            this IEnumerable<T> _source,
            Func<T, bool> _predicate,
            T? _defaultValue = default)
        {
            if (_source == null) throw new ArgumentNullException(nameof(_source));
            if (_predicate == null) throw new ArgumentNullException(nameof(_predicate));

            T? _prev = _defaultValue;

            foreach (var _item in _source)
            {
                if (_predicate(_item))
                {
                    return _prev;
                }
                _prev = _item;
            }

            return _prev;
        }
    }
}
