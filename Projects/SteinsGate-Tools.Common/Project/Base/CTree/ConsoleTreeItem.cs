using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;

namespace SGToolsCommon.Base.CTree
{
    /*
     *     BridgeLength
     *       <---->
     *      ├────── <CListItem>  
     *      │         ├──────── <CListItem> 
     *      │         ├──────── <CListItem> 
     *      │         └──────── <CListItem> 
     *      │
     *      └─── <CListItem> 
     *            └──────── <CListItem> 
     *        
     * - Fold: true시 자식 항목 출력안됨
     * - Dummy: 해당 아이템을 비어있는 것처럼 처리함
     * - Tag: 사용자 지정 값
     * - Name: 출력시 보일 이름
     * - BridgeLength: ──── 이거 길이
     * - Count: 자기를 제외한 자식의 수
     * - CountRecursive: 자기를 제외한 서브트리 자식들까지 포함한 수
     */
    public class ConsoleTreeItem : IList<ConsoleTreeItem>, ICloneable
    {
        private List<ConsoleTreeItem> Items { get; }
        public ConsoleTreeItem? Parent { get; set; }
        public bool Fold { get; set; }
        public int Count => Items.Count;
        public int CountRecursive => CountOf(this);
        public bool Dummy { get; set; }
        public string Name { get; set; }
        public object? Tag { get; set; }
        public ConsoleColor ForegroundColor { get; set; } = ConsoleColor.Black;
        public ConsoleColor BackgroundColor { get; set; } = ConsoleColor.Black;
        public bool IsReadOnly => false;
        public int BridgeLength { get; set; } = 0;

        public ConsoleTreeItem this[int index]
        {
            get => Items[index];
            set => Items[index] = value;
        }

        public ConsoleTreeItem()
        {
            Name  = string.Empty;
            Items = new List<ConsoleTreeItem>();
        }

        public ConsoleTreeItem(string name)
        {
            Name  = name;
            Items = new List<ConsoleTreeItem>();
        }

        public ConsoleTreeItem(string name, object? tag)
        {
            Name  = name;
            Tag   = tag;
            Items = new List<ConsoleTreeItem>();
        }

        public ConsoleTreeItem(string name, object? tag, List<ConsoleTreeItem> items)
        {
            Name  = name;
            Tag   = tag;
            Items = items;
        }

        public ConsoleTreeItem Add(ConsoleTreeItem item)
        {
            item.Parent = this;
            Items.Add(item);
            return this;
        }

        public ConsoleTreeItem Add(string name)
        {
            var childItem = new ConsoleTreeItem(name) { Parent = this };
            Items.Add(childItem);
            return this;
        }

        public ConsoleTreeItem Add(string name, object tag)
        {
            var childItem = new ConsoleTreeItem(name, tag) { Parent = this };
            Items.Add(childItem);
            return this;
        }

        public ConsoleTreeItem Add(params ConsoleTreeItem[] items)
        {
            foreach (var x in items) x.Parent = this;
            Items.AddRange(items);
            return this;
        }

        public ConsoleTreeItem AddDummy(int count = 1)
        {
            for (int i = 0; i < count; i++)
                Items.Add(new ConsoleTreeItem() { Dummy = true });
            return this;
        }

        public ConsoleTreeItem AddReturnChild(ConsoleTreeItem item)
        {
            item.Parent = this;
            Items.Add(item);
            return item;
        }

        public ConsoleTreeItem AddReturnChild(string name)
        {
            var childItem = new ConsoleTreeItem(name) { Parent = this };
            Items.Add(childItem);
            return childItem;
        }

        public ConsoleTreeItem AddReturnChild(string name, object tag)
        {
            var childItem = new ConsoleTreeItem(name, tag) { Parent = this };
            Items.Add(childItem);
            return childItem;
        }

        public ConsoleTreeItem AddReturnParent(ConsoleTreeItem item)
        {
            item.Parent = this;
            Items.Add(item);
            return Parent!;
        }

        public ConsoleTreeItem AddReturnParent(string name)
        {
            var childItem = new ConsoleTreeItem(name) { Parent = this };
            Items.Add(childItem);
            return Parent!;
        }

        public ConsoleTreeItem AddReturnParent(string name, object tag)
        {
            var childItem = new ConsoleTreeItem(name, tag) { Parent = this };
            Items.Add(childItem);
            return Parent!;
        }

        public static int CountOf(ConsoleTreeItem item)
        {
            int count = item.Items.Count;
            foreach (var child in item.Items)
                count += CountOf(child);
            return count;
        }

        public void Clear() => Items.Clear();
        public bool Contains(ConsoleTreeItem item) => Items.Contains(item);
        public void CopyTo(ConsoleTreeItem[] array, int arrayIndex) => Items.CopyTo(array, arrayIndex);

        public bool Remove(ConsoleTreeItem item)
        {
            bool removed = Items.Remove(item);
            if (removed) item.Parent = null;
            return removed;
        }

        public int IndexOf(ConsoleTreeItem item) => Items.IndexOf(item);

        public void Insert(int index, ConsoleTreeItem item)
        {
            item.Parent = this;
            Items.Insert(index, item);
        }

        public void RemoveAt(int index)
        {
            Items[index].Parent = null;
            Items.RemoveAt(index);
        }

        public ConsoleTreeItem? Find(Predicate<ConsoleTreeItem> predicate) => Items.Find(predicate);

        public ConsoleTreeItem? Find(Predicate<ConsoleTreeItem> predicate, out int index)
        {
            index = -1;
            for (var i = 0; i < Items.Count; i++)
            {
                if (!predicate(Items[i])) continue;
                index = i;
                return Items[i];
            }
            return null;
        }

        public ConsoleTreeItem? FindLast(Predicate<ConsoleTreeItem> predicate) => Items.FindLast(predicate);

        public ConsoleTreeItem? FindLast(Predicate<ConsoleTreeItem> predicate, out int index)
        {
            index = -1;
            for (var i = Items.Count - 1; i >= 0; i--)
            {
                if (!predicate(Items[i])) continue;
                index = i;
                return Items[i];
            }
            return null;
        }

        public IEnumerator<ConsoleTreeItem> GetEnumerator()          => Items.GetEnumerator();
        IEnumerator IEnumerable.GetEnumerator()                      => GetEnumerator();
        void ICollection<ConsoleTreeItem>.Add(ConsoleTreeItem item)  => throw new NotImplementedException();

        public object Clone()
        {
            object? childCloneableTag = Tag;
            if (Tag is ICloneable childCloneable)
                childCloneableTag = childCloneable.Clone();

            return new ConsoleTreeItem(Name, childCloneableTag, CloneChildren(this));
        }

        private static List<ConsoleTreeItem> CloneChildren(ConsoleTreeItem parent)
        {
            return parent.Select(item => (item.Clone() as ConsoleTreeItem)!).ToList();
        }

        public ConsoleTreeItem SetFold(bool foldEnabled)            { Fold = foldEnabled;               return this; }
        public ConsoleTreeItem SetForegroundColor(ConsoleColor c)   { ForegroundColor = c;              return this; }
        public ConsoleTreeItem SetBackgroundColor(ConsoleColor c)   { BackgroundColor = c;              return this; }
        public ConsoleTreeItem SetDummy(bool dummyEnabled)          { Dummy = dummyEnabled;             return this; }
        public ConsoleTreeItem SetTag(object tag)                   { Tag = tag;                        return this; }
        public ConsoleTreeItem SetBridgeLength(int length)          { BridgeLength = length;            return this; }
        public ConsoleTreeItem? GetParent()                          => Parent;
    }
}
