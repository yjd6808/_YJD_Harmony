/*
 * 작성자: 윤정도
 * 생성일: 1/31/2023 4:15:22 PM
 * =====================
 *
 */

#include "Core.h"
#include "SGStruct.h"

#include <SteinsGate/Common/SGConfig.h>

USING_NS_JC;

SGVisualInfo::SGVisualInfo() {
	JCore::Arrays::Fill(NpkIndex, InvalidValue_v);
	JCore::Arrays::Fill(ImgIndex, InvalidValue_v);
}

SGVisualInfo::SGVisualInfo(const SGVisualInfo& other) {
	JCore::Memory::CopyUnsafe(NpkIndex, other.NpkIndex, sizeof(int) * VisualType::Max);
	JCore::Memory::CopyUnsafe(ImgIndex, other.ImgIndex, sizeof(int) * VisualType::Max);
}
