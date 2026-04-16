namespace SGToolsCommon.Base.Progress
{
    public interface IProgressUnitStrategy
    {
        void OnStarted(ProgressUnit _unit);
        bool Progress(ProgressUnit _unit, int _count);
    }
}
