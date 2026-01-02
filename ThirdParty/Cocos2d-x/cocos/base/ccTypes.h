/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#ifndef __BASE_CCTYPES_H__
#define __BASE_CCTYPES_H__

#include <string>

#include "sgcc/math/CCGeometry.h"
#include "sgcc/math/CCMath.h"
#include "base/CCRef.h"
#include "sgcc/base/ccTypesBase.h"		// jdyun 여기로 분리함 (슈타인즈 게이트 기반 프로젝트에서도 써야하기 때문)
#include "platform/CCGL.h"

/**
 * @addtogroup base
 * @{
 */

NS_CC_BEGIN

/** @struct BlendFunc
 * Blend Function used for textures.
 */
struct CC_DLL BlendFunc
{
    /** source blend function */
    GLenum src;
    /** destination blend function */
    GLenum dst;

    /** Blending disabled. Uses {GL_ONE, GL_ZERO} */
    static const BlendFunc DISABLE;
    /** Blending enabled for textures with Alpha premultiplied. Uses {GL_ONE, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_PREMULTIPLIED;
    /** Blending enabled for textures with Alpha NON premultiplied. Uses {GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA} */
    static const BlendFunc ALPHA_NON_PREMULTIPLIED;
    /** Enables Additive blending. Uses {GL_SRC_ALPHA, GL_ONE} */
    static const BlendFunc ADDITIVE;

    bool operator==(const BlendFunc &a) const
    {
        return src == a.src && dst == a.dst;
    }

    bool operator!=(const BlendFunc &a) const
    {
        return src != a.src || dst != a.dst;
    }

    bool operator<(const BlendFunc &a) const
    {
        return src < a.src || (src == a.src && dst < a.dst);
    }
};

/** @struct Acceleration
 * The device accelerometer reports values for each axis in units of g-force.
 */
class CC_DLL Acceleration
    : public Ref
{
public:
    double x;
    double y;
    double z;

    double timestamp;

    Acceleration(): x(0), y(0), z(0), timestamp(0) {}
};

extern const std::string CC_DLL STD_STRING_EMPTY;
extern const ssize_t CC_DLL CC_INVALID_INDEX;

NS_CC_END
// end group
/// @}
#endif //__BASE_CCTYPES_H__
