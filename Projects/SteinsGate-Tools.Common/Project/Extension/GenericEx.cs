// jdyun 24/08/18(일)
namespace SGToolsCommon.Extension
{
    public static class GenericEx
    {
        public static void Swap<T>(ref T _a, ref T _b)
        {
            (_a, _b) = (_b, _a);
        }
    }
}
