namespace SGToolsCommon.Base.Progress
{
    public interface IProgressListener
    {
        /// <summary>
        /// Notifier에 의해 slot 프로그래스 유닛으로부터 완료보고를 받음.
        /// Notifier[slot].Cur 멤버에 접근하여 Report된 인덱스 정보를 얻으면 된다.
        /// </summary>
        void OnReported(int _slot);
    }
}
