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
    }
}
