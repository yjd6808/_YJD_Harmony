﻿/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:25:36 AM
 *
 */

using System;
using System.Collections.Generic;
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
using SGToolsCommon;
using SGToolsCommon.Sga;

namespace SGToolsUI.Model
{
    public struct SGUISpriteInfo 
    {
        public SgaPackage Sga { get; set; }
        public SgaImage Img { get; set; }
        public int Index { get; set; }

        public string SgaName => Sga.FileNameWithoutExt;
        public string ImgName => Img.Header.NameWithoutExt;
    }
}
