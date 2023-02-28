/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 9:07:34 AM
 *
 */

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Markup;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;

namespace SGToolsCommon.Provider
{
    public class ResourcePathProvider : MarkupExtension
    {
        // private const string PackUrl = "pack://application:,,,/Resource";
        private const string PackUrl = "/SGToolsCommon;component/Resource/";
        public string FileName { get; set; }
        public override object ProvideValue(IServiceProvider serviceProvider)
        {
            string result = Path.Combine(PackUrl, FileName);

            return result;
        }
    }
}
