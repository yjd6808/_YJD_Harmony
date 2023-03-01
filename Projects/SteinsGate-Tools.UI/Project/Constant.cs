﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace SGToolsUI
{
    public static class Constant
    {
        public const double ResolutionWidth = 640.0f;
        public const double ResolutionHeight = 480.0f;
        public const double CanvasWheelDelta = 10.0;
        public const double CanvasWidth = ResolutionWidth + 50.0;
        public const double CanvasHeight = ResolutionHeight + 50.0;
        public const double ResoltionRatio = 480.0f / 640.0f;
        public const string SettingFileName = "setting.json";

        public const double LeftPaneWidth = 300;
        public const int ZOrderInterval = 100;
    }
}
