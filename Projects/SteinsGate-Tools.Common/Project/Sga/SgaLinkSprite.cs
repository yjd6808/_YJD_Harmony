﻿/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 2:53:15 AM
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

namespace SGToolsCommon.Sga
{
    public class SgaLinkSprite : SgaSpriteAbstract
    {
        private int _targetFrameIndex;

        public SgaLinkSprite(SgaImage parent, int targetFrameIndex, int frameIndex) : base(SgaSpriteType.LinkSprite, SgaColorFormat.Link, parent, frameIndex)
        {
            _targetFrameIndex = targetFrameIndex;  
        }

        public override bool IsDummy => false;
        public override bool Loaded => true;
        public int TargetFrameIndex => _targetFrameIndex;

        public override void Load()
        {
        }

        public override void Unload()
        {
        }
    }
}