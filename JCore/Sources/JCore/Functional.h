/*
	�ۼ��� : ������
	C#�� �ִ� ����� ��¦ �䳻��
*/

#pragma once

#include <functional>

namespace JCore {

/* ====================================================================
 [����]

 (1) ���ٸ� �̿��� �ʱ�ȭ
 Action<int&> increaser = [](int& ref) { ref++; };


 (2) ���͸� �̿��� �ʱ�ȭ
 struct Increaser {
	operator() (int& ref) {
		ref++;
	}
 }

 Action<int&> increaser = Increase();

 --------------------------------------------------------------------
 [���]

 int a = 0;
 increaser(a); // a = 1

 ==================================================================== */


// ��ȯ���� ���� �븮��
template <typename... Args>
using Action = std::function<void(Args&&...)>;

// ��ȯ���� �ִ� �븮��
template <typename R, typename... Args>
using Func = std::function<R(Args&&...)>;

// ��/������ �Ǻ��ϴ� �븮��
template <typename... Args>
using Predicate = std::function<bool(Args&&...)>;



} // namespace JCore