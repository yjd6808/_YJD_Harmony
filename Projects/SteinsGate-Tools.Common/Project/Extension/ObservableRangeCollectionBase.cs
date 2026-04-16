// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.
// See the LICENSE file in the project root for more information.

// @원본 소스코드: https://stackoverflow.com/questions/670577/observablecollection-doesnt-support-addrange-method-so-i-get-notified-for-each
// 기존 ObservableCollection은 Range 작업을 지원하지 않으므로..

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Windows.Data;

namespace SGToolsCommon.Extension
{
    public class ObservableRangeCollection<T> : ObservableRangeCollectionBase<T>
    {
        private class DeferredEventsCollection : List<NotifyCollectionChangedEventArgs>, IDisposable
        {
            private readonly ObservableRangeCollection<T> collection_;

            public DeferredEventsCollection(ObservableRangeCollection<T> _collection)
            {
                Debug.Assert(_collection is not null);
                Debug.Assert(_collection!.deferredEvents_ is null);

                collection_ = _collection;
                collection_.deferredEvents_ = this;
            }

            public void Dispose()
            {
                collection_.deferredEvents_ = null;

                var handlers = collection_
                  .GetHandlers()
                  .ToLookup(_h => _h.Target is CollectionView);

                foreach (var handler in handlers[false])
                {
                    foreach (var e in this)
                    {
                        handler(collection_, e);
                    }
                }

                foreach (var cv in handlers[true]
                  .Select(_h => _h.Target)
                  .Cast<CollectionView>()
                  .Distinct())
                {
                    cv.Refresh();
                }
            }
        }

        private DeferredEventsCollection? deferredEvents_;

        /// <inheritdoc/>
        public ObservableRangeCollection(bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null)
            : base(_allowDuplicates, _comparer)
        { }

        /// <inheritdoc/>
        public ObservableRangeCollection(IEnumerable<T> _collection, bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null)
            : base(_collection, _allowDuplicates, _comparer)
        { }

        /// <inheritdoc/>
        public ObservableRangeCollection(List<T> _list, bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null)
            : base(_list, _allowDuplicates, _comparer)
        { }

        /// <inheritdoc/>
        protected override IDisposable DeferEvents()
        {
            return new DeferredEventsCollection(this);
        }

        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs _e)
        {
            if (typeof(ObservableRangeCollection<T>).GetField(nameof(deferredEvents_), BindingFlags.Instance | BindingFlags.NonPublic)?.GetValue(this) is ICollection<NotifyCollectionChangedEventArgs> deferredEvents)
            {
                deferredEvents.Add(_e);
                return;
            }

            foreach (var handler in GetHandlers())
            {
                if (ObservableRangeCollection<T>.IsRange(_e) && handler.Target is CollectionView cv)
                {
                    cv.Refresh();
                }
                else
                {
                    handler(this, _e);
                }
            }
        }

        private static bool IsRange(NotifyCollectionChangedEventArgs _e)
        {
            return _e.NewItems?.Count > 1 || _e.OldItems?.Count > 1;
        }

        private IEnumerable<NotifyCollectionChangedEventHandler> GetHandlers()
        {
            var info = typeof(ObservableCollection<T>).GetField(nameof(CollectionChanged), BindingFlags.Instance | BindingFlags.NonPublic);
            var @event = info?.GetValue(this) as MulticastDelegate;

            return @event?.GetInvocationList()
              .Cast<NotifyCollectionChangedEventHandler>()
              .Distinct()
              ?? Enumerable.Empty<NotifyCollectionChangedEventHandler>();
        }
    }

    /// <summary>
    /// An <see cref="ObservableCollection{T}"/> that supports bulk operations to avoid frequent update notification events.
    /// </summary>
    public class ObservableRangeCollectionBase<T> : ObservableCollection<T>
    {
        [NonSerialized]
        private DeferredEventsCollection? deferredEvents_;

        public ObservableRangeCollectionBase(bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null)
        {
            AllowDuplicates = _allowDuplicates;
            Comparer = _comparer ?? EqualityComparer<T>.Default;
        }

        public ObservableRangeCollectionBase(IEnumerable<T> _collection, bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null) : base(_collection)
        {
            AllowDuplicates = _allowDuplicates;
            Comparer = _comparer ?? EqualityComparer<T>.Default;
        }

        public ObservableRangeCollectionBase(List<T> _list, bool _allowDuplicates = true, EqualityComparer<T>? _comparer = null) : base(_list)
        {
            AllowDuplicates = _allowDuplicates;
            Comparer = _comparer ?? EqualityComparer<T>.Default;
        }

        public bool AllowDuplicates { get; } = true;

        public EqualityComparer<T> Comparer { get; }

        public int AddRange(IEnumerable<T> _collection)
        {
            return InsertRange(Count, _collection);
        }

        public int InsertRange(int _index, IEnumerable<T> _collection)
        {
            if (_collection is null)
                throw new ArgumentNullException(nameof(_collection));

            if (_index < 0)
                throw new ArgumentOutOfRangeException(nameof(_index));

            if (_index > Count)
                throw new ArgumentOutOfRangeException(nameof(_index));

            if (!AllowDuplicates)
            {
                _collection = _collection
                  .Distinct(Comparer)
                  .Where(_item => !Items.Contains(_item, Comparer));
            }

            int limitedCount = _collection.Take(2).Count();

            if (limitedCount == 0)
                return 0;

            if (limitedCount == 1)
            {
                Add(_collection.First());
                return 1;
            }

            CheckReentrancy();

            var items = (List<T>)Items;
            items.InsertRange(_index, _collection);

            OnEssentialPropertiesChanged();

            var changedItems = _collection.ToList();
            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, changedItems, _index));

            return changedItems.Count;
        }

        public int RemoveAll(Predicate<T> _match)
        {
            return RemoveAll(0, Count, _match);
        }

        public int RemoveAll(int _index, int _count, Predicate<T> _match)
        {
            if (_index < 0)
                throw new ArgumentOutOfRangeException(nameof(_index));

            if (_count < 0)
                throw new ArgumentOutOfRangeException(nameof(_count));

            if (Count - _index < _count)
                throw new ArgumentException("Offset and length were out of bounds for the array or count is greater than the number of items from index to the end of the source collection.");

            if (_match is null)
                throw new ArgumentNullException(nameof(_match));

            if (Count == 0)
                return 0;

            List<T>? cluster = null;
            int clusterIndex = -1;
            int removedCount = 0;

            using (BlockReentrancy())
            using (DeferEvents())
            {
                for (int i = 0; i < _count; i++, _index++)
                {
                    var item = Items[_index];

                    if (_match(item))
                    {
                        Items.RemoveAt(_index);
                        removedCount++;

                        if (clusterIndex == _index)
                        {
                            Debug.Assert(cluster is not null);
                            cluster!.Add(item);
                        }
                        else
                        {
                            cluster = new List<T> { item };
                            clusterIndex = _index;
                        }

                        _index--;
                    }
                    else if (clusterIndex > -1)
                    {
                        OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, cluster, clusterIndex));
                        clusterIndex = -1;
                        cluster = null;
                    }
                }

                if (clusterIndex > -1)
                {
                    OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, cluster, clusterIndex));
                }
            }

            if (removedCount > 0)
                OnEssentialPropertiesChanged();

            return removedCount;
        }

        public int RemoveRange(IEnumerable<T> _collection)
        {
            if (_collection is null)
                throw new ArgumentNullException(nameof(_collection));

            if (Count == 0)
                return 0;

            int limitedCount = _collection.Take(2).Count();

            if (limitedCount == 0)
                return 0;

            if (limitedCount == 1)
            {
                bool removed = Remove(_collection.First());
                return removed ? 1 : 0;
            }

            CheckReentrancy();

            int removedCount = 0;

            foreach (var item in _collection)
            {
                bool removed = Items.Remove(item);
                removedCount += removed ? 1 : 0;
            }

            if (removedCount == 0)
                return 0;

            OnEssentialPropertiesChanged();

            if (Count == 0)
            {
                OnCollectionReset();
            }
            else
            {
                OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, _collection.ToList()));
            }

            return removedCount;
        }

        public void RemoveRange(int _index, int _count)
        {
            if (_index < 0)
                throw new ArgumentOutOfRangeException(nameof(_index));

            if (_count < 0)
                throw new ArgumentOutOfRangeException(nameof(_count));

            if (Count - _index < _count)
                throw new ArgumentException("Offset and length were out of bounds for the array or count is greater than the number of items from index to the end of the source collection.");

            if (_count == 0)
                return;

            if (_count == 1)
            {
                RemoveItem(_index);
                return;
            }

            if (_index == 0 && _count == Count)
            {
                Clear();
                return;
            }

            var items = (List<T>)Items;
            var removedItems = items.GetRange(_index, _count);

            CheckReentrancy();

            items.RemoveRange(_index, _count);

            OnEssentialPropertiesChanged();

            OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, removedItems, _index));
        }

        public int Replace(T _item)
        {
            return ReplaceRange(0, Count, new[] { _item });
        }

        public int ReplaceRange(IEnumerable<T> _collection)
        {
            return ReplaceRange(0, Count, _collection);
        }

        public int ReplaceRange(int _index, int _count, IEnumerable<T> _collection)
        {
            void OnRangeReplaced(int _followingItemIndex, ICollection<T> _newCluster, ICollection<T> _oldCluster)
            {
                if (_oldCluster is null || _oldCluster.Count == 0)
                {
                    Debug.Assert(_newCluster is null || _newCluster.Count == 0);
                    return;
                }

                OnCollectionChanged(
                  new NotifyCollectionChangedEventArgs(
                    NotifyCollectionChangedAction.Replace,
                    new List<T>(_newCluster),
                    new List<T>(_oldCluster),
                    _followingItemIndex - _oldCluster.Count));

                _oldCluster.Clear();
                _newCluster.Clear();
            }

            if (_index < 0)
                throw new ArgumentOutOfRangeException(nameof(_index));

            if (_count < 0)
                throw new ArgumentOutOfRangeException(nameof(_count));

            if (Count - _index < _count)
                throw new ArgumentException("Offset and length were out of bounds for the array or count is greater than the number of items from index to the end of the source collection.");

            if (_collection is null)
                throw new ArgumentNullException(nameof(_collection));

            if (!_collection.Any())
            {
                RemoveRange(_index, _count);
                return -_count;
            }

            if (!AllowDuplicates)
            {
                _collection = _collection
                  .Distinct(Comparer)
                  .ToList();
            }

            if (_index + _count == 0)
            {
                int added = InsertRange(0, _collection);
                return added;
            }

            int oldCount = Count;

            if (_collection is not IList<T> list)
            {
                list = new List<T>(_collection);
            }

            using (BlockReentrancy())
            using (DeferEvents())
            {
                int rangeCount = _index + _count;
                int addedCount = list.Count;

                bool changesMade = false;
                List<T>? newCluster = null;
                List<T>? oldCluster = null;

                int i = _index;

                for (; i < rangeCount && i - _index < addedCount; i++)
                {
                    T old = this[i], @new = list[i - _index];

                    if (Comparer.Equals(old, @new))
                    {
                        OnRangeReplaced(i, newCluster!, oldCluster!);
                        continue;
                    }
                    else
                    {
                        Items[i] = @new;

                        if (newCluster is null)
                        {
                            Debug.Assert(oldCluster is null);
                            newCluster = new List<T> { @new };
                            oldCluster = new List<T> { old };
                        }
                        else
                        {
                            newCluster.Add(@new);
                            oldCluster!.Add(old);
                        }

                        changesMade = true;
                    }
                }

                OnRangeReplaced(i, newCluster!, oldCluster!);

                if (_count != addedCount)
                {
                    var items = (List<T>)Items;

                    if (_count > addedCount)
                    {
                        int removedCount = rangeCount - addedCount;
                        var removed = new T[removedCount];
                        items.CopyTo(i, removed, 0, removed.Length);
                        items.RemoveRange(i, removedCount);
                        OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Remove, removed, i));
                    }
                    else
                    {
                        int k = i - _index;
                        var added = new T[addedCount - k];

                        for (int j = k; j < addedCount; j++)
                        {
                            var @new = list[j];
                            added[j - k] = @new;
                        }

                        items.InsertRange(i, added);
                        OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Add, added, i));
                    }

                    OnEssentialPropertiesChanged();
                }
                else if (changesMade)
                {
                    OnIndexerPropertyChanged();
                }
            }

            return Count - oldCount;
        }

        protected override void ClearItems()
        {
            if (Count == 0)
                return;

            base.ClearItems();
        }

        protected virtual IDisposable DeferEvents()
        {
            return new DeferredEventsCollection(this);
        }

        protected override void InsertItem(int _index, T _item)
        {
            if (!AllowDuplicates && Items.Contains(_item))
                return;

            base.InsertItem(_index, _item);
        }

        protected override void OnCollectionChanged(NotifyCollectionChangedEventArgs _e)
        {
            if (deferredEvents_ is not null)
            {
                deferredEvents_.Add(_e);
                return;
            }

            base.OnCollectionChanged(_e);
        }

        protected override void SetItem(int _index, T _item)
        {
            if (AllowDuplicates)
            {
                if (Comparer.Equals(this[_index], _item))
                    return;
            }
            else if (Items.Contains(_item, Comparer))
            {
                return;
            }

            base.SetItem(_index, _item);
        }

        private void OnCollectionReset()
        {
            OnCollectionChanged(EventArgsCache.ResetCollectionChanged);
        }

        private void OnEssentialPropertiesChanged()
        {
            OnPropertyChanged(EventArgsCache.CountPropertyChanged);
            OnIndexerPropertyChanged();
        }

        private void OnIndexerPropertyChanged()
        {
            OnPropertyChanged(EventArgsCache.IndexerPropertyChanged);
        }

        private sealed class DeferredEventsCollection : List<NotifyCollectionChangedEventArgs>, IDisposable
        {
            private readonly ObservableRangeCollectionBase<T> collectionBase_;

            public DeferredEventsCollection(ObservableRangeCollectionBase<T> _collectionBase)
            {
                Debug.Assert(_collectionBase is not null);
                Debug.Assert(_collectionBase.deferredEvents_ is null);

                collectionBase_ = _collectionBase;
                collectionBase_!.deferredEvents_ = this;
            }

            public void Dispose()
            {
                collectionBase_.deferredEvents_ = null;

                foreach (var args in this)
                {
                    collectionBase_.OnCollectionChanged(args);
                }
            }
        }
    }

    /// <remarks>
    /// To be kept outside <see cref="ObservableCollection{T}"/>, since otherwise, a new instance will be created for each generic type used.
    /// </remarks>
    internal static class EventArgsCache
    {
        internal static readonly PropertyChangedEventArgs CountPropertyChanged = new("Count");

        internal static readonly PropertyChangedEventArgs IndexerPropertyChanged = new("Item[]");

        internal static readonly NotifyCollectionChangedEventArgs ResetCollectionChanged = new(NotifyCollectionChangedAction.Reset);
    }
}
