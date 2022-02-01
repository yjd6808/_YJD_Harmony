#pragma once

/*
	�ۼ��� : ������
	�ƽ�Ű ���� ����
*/

namespace JCore {
	namespace Ascii {
		inline bool IsAlphabat(const char ch) {
			return ch >= 'a' && ch <= 'z' || ch >= 'A' && ch <= 'Z';
		}

		inline bool IsSpace(const char ch) {
			return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\v' || ch == '\r' || ch == '\f';
		}
	}
} // namespace JCore