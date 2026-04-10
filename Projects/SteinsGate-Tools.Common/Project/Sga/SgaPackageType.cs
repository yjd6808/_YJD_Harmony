/*
 * 작성자: 윤정도
 * 생성일: 2/27/2023 7:18:27 AM
 *
 */

using System.Collections.Generic;

namespace SGToolsCommon.Sga
{
    public class SgaPackageType
    {
        private static int Seq = 0;

        public static readonly int Interface = Seq++;
        public static readonly int Monster = Seq++;
        public static readonly int Gunner = Seq++;
        public static readonly int Creature = Seq++;
        public static readonly int Item = Seq++;
        public static readonly int Map = Seq++;
        public static readonly int Max = Seq++;

        public static Dictionary<int, string> Name = new()
        {
            { Interface, "interface" },
            { Monster  , "monster"   },
            { Gunner   , "gunner"    },
            { Creature , "creature"  },
            { Item     , "item"      },
            { Map      , "map"       }
        };
    }
}
