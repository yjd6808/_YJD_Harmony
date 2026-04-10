using System;
using System.Diagnostics;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace SGToolsUI
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        public App()
        {
        }

        //////////////////////////////////////////////////////////////////////////////////
        // https://stackoverflow.com/questions/793100/globally-catch-exceptions-in-a-wpf-application
        // 아직 쓸것 같진 않은데.. 일단 달아놓음
        protected override void OnStartup(StartupEventArgs _e)
        {
            base.OnStartup(_e);
            SetupExceptionHandling();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void SetupExceptionHandling()
        {
            AppDomain.CurrentDomain.UnhandledException += (_s, _e) =>
                LogUnhandledException((Exception)_e.ExceptionObject, "AppDomain.CurrentDomain.UnhandledException");

            DispatcherUnhandledException += (_s, _e) =>
            {
                LogUnhandledException(_e.Exception, "Application.Current.DispatcherUnhandledException");
                _e.Handled = true;
            };

            TaskScheduler.UnobservedTaskException += (_s, _e) =>
            {
                LogUnhandledException(_e.Exception, "TaskScheduler.UnobservedTaskException");
                _e.SetObserved();
            };
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void LogUnhandledException(Exception _exception, string _source)
        {
            string message = $"Unhandled exception ({_source})";
            Debug.Assert(false);

            try
            {
                System.Reflection.AssemblyName assemblyName = System.Reflection.Assembly.GetExecutingAssembly().GetName();
                message = string.Format("Unhandled exception in {0} v{1}", assemblyName.Name, assemblyName.Version);
            }
            catch (Exception)
            {
                // _logger.Error(ex, "Exception in LogUnhandledException");
            }
            finally
            {
                // _logger.Error(_exception, message);
            }
        }
    }
}
