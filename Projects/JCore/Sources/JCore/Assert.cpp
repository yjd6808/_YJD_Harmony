/*
 *	작성자 : 윤정도
 *	작성일 : 2023/02/05(일요일 오후 3시 35분 경)
 */


#include <JCore/Core.h>
#include <JCore/Assert.h>
#include <JCore/Primitives/StringUtil.h>
#include <JCore/FileSystem/Path.h>

namespace JCore {
	namespace Detail {
		int __StringLength(const char* str) {
			return StringUtil::Length(str);
		}
		void __StringFormatBuffer(char* buff, const int buffCapacity, const char* format, ...) {
			va_list args;
			va_start(args, format);
			StringUtil::FormatBuffer(buff, buffCapacity, format, args);
			va_end(args);
		}

		void __StringConcatInnerFront(char* buf, int bufCapacity, const char* concatStr) {
			StringUtil::ConcatInnerFront(buf, bufCapacity, concatStr);
		}
		void __PathFileNameLevel(char* buf, int bufSize, const char* path, int level) {
			Path::FileNameLevel(buf, bufSize, path, StringUtil::Length(path), level);
		}
	}
}

