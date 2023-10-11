# 메모리 풀

### 특징
1. ObjectPool\<T\>을 상속받은 객체는 손쉽게 풀링이 가능합니다.
2. 메모리풀은 메모리청크 획득방식에 따라 나눠서 구현하였습니다.  
    * Indexed: O(1)의 시간복잡도로 메모리 블록을 얻습니다. 
    * BinarySearch:O(logn)의 시간복잡도로 메모리 블록을 얻습니다.
    
<br>

### 참고사항
초기에 구현한게 BinarySearch방식의 메모리풀이고  
좀더 성능을 끌어올릴 수 없을까 고민해서 구현한게 Indexed방식의 메모리풀입니다.


<br>

### ObjectPool\<T\> 사용 예시
![ObjectPool](Images/JCore/ObjectPool.png)

<br>

### IndexedMemoryPool 사용 예시
![ThreadPool](Images/JCore/IndexedMemoryPool.png)

<br>
