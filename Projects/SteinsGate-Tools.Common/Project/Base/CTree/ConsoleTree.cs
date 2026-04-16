using System;
using System.Collections.Generic;
using System.Text;

namespace SGToolsCommon.Base.CTree
{
    /// <summary>
    /// cmd의 tree 명령처럼 콘솔 트리 구조를 출력하는 클래스.
    /// </summary>
    public class ConsoleTree : ICloneable
    {
        public ConsoleTreeItem Root { get; }
        public int Count => Root.Count + 1;
        public int CountRecursive => Root.CountRecursive + 1;

        public char VertialBridge            { get; set; } = '│';
        public char VerticalItemBridge       { get; set; } = '├';
        public char VerticalItemBridgeLast   { get; set; } = '└';
        public char HorizontalBrdige         { get; set; } = '─';
        public char Space                    { get; set; } = ' ';
        public int  ItemLeftPad              { get; set; } = 1;

        public ConsoleColor BridgeForegroundColor { get; set; } = Console.ForegroundColor;
        public ConsoleColor ItemForegroundColor   { get; set; } = Console.ForegroundColor;
        public ConsoleColor ItemBackgroundColor   { get; set; } = Console.BackgroundColor;

        public ConsoleTree()                                                                            => Root = new ConsoleTreeItem();
        public ConsoleTree(ConsoleTreeItem root)                                                        => Root = root;
        public ConsoleTree(string rootName)                                                             => Root = new ConsoleTreeItem(rootName);
        public ConsoleTree(string rootName, object tag)                                                 => Root = new ConsoleTreeItem(rootName, tag);
        public ConsoleTree(string rootName, object tag, List<ConsoleTreeItem> children)                => Root = new ConsoleTreeItem(rootName, tag, children);

        public ConsoleTreeItem Add(ConsoleTreeItem item1)                                               { Root.Add(item1); return Root; }
        public ConsoleTreeItem Add(params ConsoleTreeItem[] items)                                      { Root.Add(items); return Root; }
        public ConsoleTreeItem Add(string name)                                                         { Root.Add(name);  return Root; }
        public ConsoleTreeItem Add(string name, object tag)                                             { Root.Add(name, tag); return Root; }
        public ConsoleTreeItem AddDummy(int count = 1)                                                  { Root.AddDummy(count); return Root; }
        public ConsoleTreeItem AddReturnChild(ConsoleTreeItem item)                                     => Root.AddReturnChild(item);
        public ConsoleTreeItem AddReturnChild(string name)                                              => Root.AddReturnChild(name);
        public ConsoleTreeItem AddReturnChild(string name, object tag)                                  => Root.AddReturnChild(name, tag);
        public bool Remove(ConsoleTreeItem item)                                                        => Root.Remove(item);
        public bool Contains(ConsoleTreeItem item)                                                      => Contains(Root, item);
        public ConsoleTreeItem? Find(Predicate<ConsoleTreeItem> predicate)                              => Find(Root, predicate);
        public void ForEach(Action<ConsoleTreeItem> action)                                             => ForEach(Root, action);

        private static bool Contains(ConsoleTreeItem parent, ConsoleTreeItem item)
        {
            if (parent.Contains(item)) return true;
            foreach (var child in parent)
                if (Contains(child, item)) return true;
            return false;
        }

        private static void ForEach(ConsoleTreeItem parent, Action<ConsoleTreeItem> action)
        {
            action(parent);
            foreach (var child in parent)
                ForEach(child, action);
        }

        private static ConsoleTreeItem? Find(ConsoleTreeItem parent, Predicate<ConsoleTreeItem> predicate)
        {
            if (predicate(parent)) return parent;
            foreach (var child in parent)
            {
                var found = Find(child, predicate);
                if (found != null) return found;
            }
            return null;
        }

        public void Print()
        {
            PrintItemName(Root);
            Print(Root, string.Empty);
        }

        private void Print(ConsoleTreeItem item, string bridge)
        {
            if (item.Count <= 0) return;

            ConsoleTreeItem? lastDummy    = item.FindLast(x => x.Dummy,  out int lastDummyIdx);
            ConsoleTreeItem? unused       = item.FindLast(x => !x.Dummy, out int lastNotDummyidx);

            if (lastDummyIdx == item.Count - 1)
                item.RemoveAt(lastDummyIdx);

            for (int i = 0; i < item.Count; i++)
            {
                var child = item[i];

                string currentBridge;
                string nextBridge;

                if (i < lastNotDummyidx)
                {
                    currentBridge = bridge + VerticalItemBridge.ToString() + new string(HorizontalBrdige, child.BridgeLength) + new string(Space, ItemLeftPad);
                    nextBridge    = currentBridge.Replace(VerticalItemBridge, VertialBridge).Replace(HorizontalBrdige, Space);
                }
                else
                {
                    currentBridge = bridge + VerticalItemBridgeLast.ToString() + new string(HorizontalBrdige, child.BridgeLength) + new string(Space, ItemLeftPad);
                    nextBridge    = currentBridge.Replace(VerticalItemBridgeLast, Space).Replace(HorizontalBrdige, Space);
                }

                if (child.Dummy)
                {
                    WriteColored(nextBridge + Environment.NewLine, BridgeForegroundColor);
                    continue;
                }

                WriteColored(currentBridge, BridgeForegroundColor);
                PrintItemName(child);

                if (!child.Fold)
                    Print(child, nextBridge);
            }

            if (lastDummyIdx == item.Count - 1)
                item.Add(lastDummy!);
        }

        private void PrintItemName(ConsoleTreeItem item)
        {
            var fg = item.ForegroundColor == ConsoleColor.Black ? ItemForegroundColor : item.ForegroundColor;
            var bg = item.BackgroundColor == ConsoleColor.Black ? ItemBackgroundColor : item.BackgroundColor;
            var prev_fg = Console.ForegroundColor;
            var prev_bg = Console.BackgroundColor;
            Console.ForegroundColor = fg;
            Console.BackgroundColor = bg;
            Console.WriteLine(item.Name);
            Console.ForegroundColor = prev_fg;
            Console.BackgroundColor = prev_bg;
        }

        private static void WriteColored(string text, ConsoleColor color)
        {
            var prev = Console.ForegroundColor;
            Console.ForegroundColor = color;
            Console.Write(text);
            Console.ForegroundColor = prev;
        }

        public override string ToString()
        {
            var sb = new StringBuilder(2048);
            sb.AppendLine(Root.Name);
            MakeString(Root, string.Empty, sb);
            return sb.ToString();
        }

        private void MakeString(ConsoleTreeItem item, string bridge, StringBuilder _builder)
        {
            if (item.Count <= 0) return;

            ConsoleTreeItem? lastDummy  = item.FindLast(x => x.Dummy,  out int lastDummyIdx);
            ConsoleTreeItem? unused     = item.FindLast(x => !x.Dummy, out int lastNotDummyidx);

            if (lastDummyIdx == item.Count - 1)
                item.RemoveAt(lastDummyIdx);

            for (int i = 0; i < item.Count; i++)
            {
                var child = item[i];

                string currentBridge;
                string nextBridge;

                if (i < lastNotDummyidx)
                {
                    currentBridge = bridge + VerticalItemBridge.ToString() + new string(HorizontalBrdige, child.BridgeLength) + new string(Space, ItemLeftPad);
                    nextBridge    = currentBridge.Replace(VerticalItemBridge, VertialBridge).Replace(HorizontalBrdige, Space);
                }
                else
                {
                    currentBridge = bridge + VerticalItemBridgeLast.ToString() + new string(HorizontalBrdige, child.BridgeLength) + new string(Space, ItemLeftPad);
                    nextBridge    = currentBridge.Replace(VerticalItemBridgeLast, Space).Replace(HorizontalBrdige, Space);
                }

                if (child.Dummy)
                {
                    _builder.AppendLine(nextBridge);
                    continue;
                }

                _builder.Append(currentBridge);
                _builder.AppendLine(child.Name);

                if (!child.Fold)
                    MakeString(child, nextBridge, _builder);
            }

            if (lastDummyIdx == item.Count - 1)
                item.Add(lastDummy!);
        }

        public object Clone() => new ConsoleTree((Root.Clone() as ConsoleTreeItem)!);

        public ConsoleTree SetBridgeForegroundColor(ConsoleColor c) { BridgeForegroundColor = c; return this; }
        public ConsoleTree SetItemForegroundColor(ConsoleColor c)   { ItemForegroundColor   = c; return this; }
        public ConsoleTree SetItemBackgroundColor(ConsoleColor c)   { ItemBackgroundColor   = c; return this; }
    }
}
