using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using WindowsDisplayAPI;

namespace SGToolsCommon.Extension
{
    public static class DisplayEx
    {
        public static List<Rect> GetDisplayRectList()
        {
            List<Rect> rectList = new List<Rect>();
            Display[] displays = Display.GetDisplays().ToArray();

            if (displays == null || displays.Length < 1) 
                return rectList;

            foreach (var display in displays)
            {
                Point position = new global::System.Windows.Point(display.CurrentSetting.Position.X, display.CurrentSetting.Position.Y);
                Size size = new Size(display.CurrentSetting.Resolution.Width, display.CurrentSetting.Resolution.Height);

                rectList.Add(new Rect(position, size));
            }

            return rectList;
        }
    }
}
