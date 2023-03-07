﻿/*
 * 작성자: 윤정도
 * 생성일: 3/1/2023 11:25:36 AM
 *
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Security.Policy;
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
using Xceed.Wpf.Toolkit.PropertyGrid.Attributes;

namespace SGToolsUI.Model
{
    [ExpandableObject]
    public struct SGUISpriteInfo 
    {
        public SGUISpriteInfo()
        {
            Sga = null;
            Img = null;
            Sprite = null;
        }

        public SGUISpriteInfo(SgaSprite sprite)
        {
            Sga = sprite.Parent.Parent;
            Img = sprite.Parent;
            Sprite = sprite;
        }

        public SgaPackage Sga { get; }
        public SgaImage Img { get; }
        public SgaSprite Sprite { get; }

        [Browsable(false)]
        public Rect Rect
        {
            get
            {
                if (Sprite == null)
                    throw new Exception("스프라이트가 세팅되어있지 않습니다.");

                return Sprite.Rect;
            }
        }

        [Browsable(false)] 
        public BitmapSource Source => Sprite != null ? Sprite.Source : null;

        [Browsable(false)]
        public bool IsNull => Sprite == null;
        [Browsable(false)]
        public string SgaName => Sga.FileNameWithoutExt;
        [Browsable(false)]
        public string ImgName => Img.Header.NameWithoutExt;

        public override string ToString()
        {
            if (IsNull)
                return "비어있음";

            return "할당됨";
        }
    }
}
