using System.Windows;
using SGToolsCommon.Customize.Control;

namespace SGToolsResearch
{
    public partial class RTBLogViewerResearchWindow : Window
    {
        public RTBLogViewer LogViewer => logViewer_;

        public RTBLogViewerResearchWindow()
        {
            InitializeComponent();
        }
    }
}