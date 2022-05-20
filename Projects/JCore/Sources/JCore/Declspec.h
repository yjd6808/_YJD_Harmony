/*
 *	�ۼ��� : ������
 *
 *	__declspec : ����ũ�μ���Ʈ Ȯ�� ������� Ÿ�� �Ǵ� �Լ��� Storage Class �Ӽ��� �ο��ϴ� ���� ����ϴ� Ű�����̴�.
 *			     �� ������, �ϴ� MSVC ������ ������ �˾Ƶ���.
 *				 dllimport, dllexport �̰� 2����
 *	Storage Class : ����� ����?�� ���� �Ǵ� �Լ��� ����� ���� ����?(Visibility)�� �����Ѵ�.
 *
 *	@����1 : https://stackoverflow.com/questions/2284610/what-is-declspec-and-when-do-i-need-to-use-it
 *	@����2 : https://www.tutorialspoint.com/cplusplus/cpp_storage_classes.htm
 */

#pragma once

 // novtable�̶�?
 // @���� �� �� : https://blog.naver.com/wjdeh313/222733324896
#define JCORE_NOVTABLE	__declspec(novtable)
