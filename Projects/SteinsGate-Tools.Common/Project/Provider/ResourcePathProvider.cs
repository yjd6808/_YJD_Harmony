/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 9:07:34 AM
 *
 */

using System;
using System.IO;
using System.Windows.Markup;

namespace SGToolsCommon.Provider
{
    public class ResourcePathProvider : MarkupExtension
    {
        // private const string PackUrl = "pack://application:,,,/Resource";
        private const string PackUrl = "/SGToolsCommon;component/Resource/";
        public string FileName { get; set; }

        //////////////////////////////////////////////////////////////////////////////////
        public override object ProvideValue(IServiceProvider _serviceProvider)
        {
            string result = Path.Combine(PackUrl, FileName);
            return result;
        }
    }
}
