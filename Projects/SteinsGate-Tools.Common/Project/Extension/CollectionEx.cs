/*
 * 작성자: 윤정도
 * 생성일: 3/6/2023 4:44:35 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace SGToolsCommon.Extension
{
    public static class CollectionEx
    {
        public static IEnumerable<T> Reversed<T>(this ObservableCollection<T> collection) where T : class
        {
            for (int i = collection.Count - 1; i >= 0; --i)
                yield return collection[i];
        }


        // 반대로 인덱스와 함께 순회
        public static void ReversedIndexingForEach<T>(this IList<T> collection, Action<int, T> action) where T : class
        {
            for (int i = collection.Count - 1; i >= 0; --i)
                action(i, collection[i]);
        }

        // 반대로 조건이 true일때까지만 순회
        // return true가 break이고
        // return false가 continue라고 생각하면된다.
        public static void ReversedConditionForEach<T>(this IList<T> collection, Predicate<T> predicate) where T : class
        {
            for (int i = collection.Count - 1; i >= 0; --i)
                if (!predicate(collection[i]))
                    return;
        }


        // 반대로 인덱스와 함께 조건이 true일때까지만 순회
     
        public static void ReversedIndexingConditionForEach<T>(this IList<T> collection, Func<int, T, bool> predicate) where T : class
        {
            for (int i = collection.Count - 1; i >= 0; --i)
                if (!predicate(i, collection[i]))
                    return;
        }
        

        public static ObservableCollection<T> InsertRangeNew<T>(
            this ObservableCollection<T> collection, int index, IList<T> elements) 
            where T : class
        {
            ObservableCollection<T> newCollection = new(new List<T>(collection.Count + elements.Count + 16));

            if (index > collection.Count)
                index = collection.Count;

            for (int i = 0; i < index; ++i)
                newCollection.Add(collection[i]);

            for (int i = 0; i < elements.Count; ++i)
                newCollection.Add(elements[i]);

            for (int i = index; i < collection.Count; ++i)
                newCollection.Add(collection[i]);

            return newCollection;
        }

        public static ObservableCollection<T> AddAfterRangeNew<T>(
            this ObservableCollection<T> collection, int index, IList<T> elements) 
            where T : class
            => InsertRangeNew(collection, index + 1, elements);

        public static void AddRange<T>(
            this ObservableCollection<T> collection, IList<T> elements)
            where T : class
        {
            foreach (var element in elements)
                collection.Add(element);
        }

        public static void ForEach<T>(this IEnumerable<T> collection, Action<int, T> action) where T : class
        {
            int i = 0;
            foreach (T item in collection)
                action(i++, item);
        }

        public static bool Exist(this ItemCollection collection, object item)
        {
            foreach (var o in collection)
                if (o == item)
                    return true;

            return false;
        }

        public static IEnumerable<object> Where(this ItemCollection collection, Predicate<object> predicate)
        {
            foreach (var o in collection)
                if (predicate(o))
                    yield return o;
        }

        public static IEnumerable<T> Where<T>(this ItemCollection collection, Predicate<T> predicate) where T : class
        {
            foreach (var o in collection)
            {
                T t = o as T;
                if (t == null) throw new NullReferenceException($"{typeof(T).Name}으로 변환되지 않는 타입입니다.");
                if (predicate(t))
                    yield return t;
            }
        }

        public static void ForEach(this ItemCollection collection, Action<object> action)
        {
            foreach (var o in collection)
                action(o);
        }

        public static void ForEach<T>(this ItemCollection collection, Action<T> action) where T : class
        {
            foreach (var o in collection)
            {
                T t = o as T;
                if (t == null) throw new NullReferenceException($"{typeof(T).Name}으로 변환되지 않는 타입입니다.");
                action(t);
            }
        }
    }
}
