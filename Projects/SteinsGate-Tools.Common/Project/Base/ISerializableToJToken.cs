using Newtonsoft.Json.Linq;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// JToken 기반 직렬화/역직렬화 인터페이스.
    /// 구현 시 파라미터 없는 생성자를 반드시 제공할 것.
    /// </summary>
    public interface ISerializableToJToken
    {
        JToken  Serialize();
        void    Deserialize(JToken _obj);
    }
}
