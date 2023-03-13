/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:41:09 AM
 *
 */

using System.IO;
using System.Windows.Controls;

namespace SGToolsCommon.Sga
{
    public class SgaElementHeader
    {
        public int Offset { get; set; }
        public int NextOffset { get; set; }
        public int IndexInPackage { get; set; }
        public int Length { get; set; }
        public string Name { get; set; }
        public string NameWithoutExt { get; set; }
        // public ListBoxItem Item { get; set; }
    }
}
