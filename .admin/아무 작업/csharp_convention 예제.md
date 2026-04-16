---
decs: 그냥 컨벤션 예제 코드
---

## 컨벤션 예제 (Calculator)

```csharp
// using 순서: System → System.Windows → NuGet → 프로젝트
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using SGToolsCommon;

/*
 * 작성자: 윤정도
 * 생성일: 4/4/2026 12:00:00 PM
 *
 */

// 열거형: PascalCase 값
public enum OperationType
{
    Add,
    Subtract,
    Multiply,
    Divide,
}

// 인터페이스: I prefix + PascalCase
public interface ICalculator
{
    double Result { get; }
    bool TryCalculate(double _a, double _b, OperationType _type, out double _result);
}

// 추상 클래스: Abstract suffix
public abstract class CalculatorAbstract : ICalculator
{
    // const 멤버: PascalCase
    public const int MaxHistorySize = 100;

    // static readonly 멤버: PascalCase
    public static readonly CalculatorAbstract Zero = new Calculator();

    // private 필드: suffix _
    private List<string> history_ = new();
    private double result_;

    // protected 필드: suffix _
    protected int precision_;
    protected string? label_;

    // Expression-bodied 프로퍼티: PascalCase
    public double Result => result_;
    public int HistoryCount => history_.Count;

    // 명시적 getter 프로퍼티
    public string LastOperation
    {
        get
        {
            if (history_.Count == 0)
                return string.Empty;
            return history_[history_.Count - 1];
        }
    }

    // event: PascalCase
    public event Action<double>? CalculationCompleted;

    // setter에서 이벤트 발생
    protected double ResultValue
    {
        get => result_;
        set
        {
            result_ = value;
            CalculationCompleted?.Invoke(result_);
        }
    }

    // abstract 메서드
    public abstract bool TryCalculate(double _a, double _b, OperationType _type, out double _result);

    // virtual 메서드: virtual 명시
    //////////////////////////////////////////////////////////////////////////////////////
    public virtual bool Validate(double _value)
    {
        return !double.IsNaN(_value) && !double.IsInfinity(_value);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    protected void AddHistory(string _entry)
    {
        const int MAX_TRIM_SIZE = 10;  // const 지역 변수: UPPER_CASE

        if (history_.Count >= MaxHistorySize)
            history_.RemoveRange(0, MAX_TRIM_SIZE);

        history_.Add(_entry);
    }

    //////////////////////////////////////////////////////////////////////////////////////
    public string GetSummary()
    {
        return $"[{label_ ?? "Calculator"}] Result={Result:F{precision_}}, History={HistoryCount}";
    }

    // 제네릭 타입 파라미터: 단일 대문자
    //////////////////////////////////////////////////////////////////////////////////////
    public static T Clamp<T>(T _value, T _min, T _max) where T : IComparable<T>
    {
        if (_value.CompareTo(_min) < 0)
            return _min;
        if (_value.CompareTo(_max) > 0)
            return _max;
        return _value;
    }
}

// 구체 클래스
public class Calculator : CalculatorAbstract
{
    // virtual+override: 둘 다 명시
    //////////////////////////////////////////////////////////////////////////////////////
    public virtual bool Validate(double _value) override
    {
        if (!base.Validate(_value))
            return false;
        return _value >= -1e15 && _value <= 1e15;
    }

    // abstract override: override만 명시
    //////////////////////////////////////////////////////////////////////////////////////
    public override bool TryCalculate(double _a, double _b, OperationType _type, out double _result)
    {
        _result = 0;

        if (!Validate(_a) || !Validate(_b))
            return false;

        // 패턴 매칭
        if (_type is OperationType.Divide && _b == 0)
            return false;

        double quotient;  // 일반 지역 변수: camelCase

        switch (_type)
        {
            case OperationType.Add:      _result = _a + _b; break;
            case OperationType.Subtract: _result = _a - _b; break;
            case OperationType.Multiply: _result = _a * _b; break;
            case OperationType.Divide:
                quotient = _a / _b;
                _result = quotient;
                break;
            default:
                return false;
        }

        ResultValue = _result;
        AddHistory($"{_a} {_type} {_b} = {_result}");
        return true;
    }

    // async/await: Async suffix, Task 반환
    //////////////////////////////////////////////////////////////////////////////////////
    public async Task<bool> TryCalculateAsync(double _a, double _b, OperationType _type)
    {
        return await Task.Run(() =>
        {
            return TryCalculate(_a, _b, _type, out double _result);
        });
    }
}

// 확장 메서드 클래스: Ex suffix
public static class CalculatorEx
{
    //////////////////////////////////////////////////////////////////////////////////////
    public static bool TryAdd(this ICalculator _calculator, double _a, double _b, out double _result)
    {
        _result = 0;

        // Nullable 활용
        if (_calculator is not Calculator calc)
            return false;

        return calc.TryCalculate(_a, _b, OperationType.Add, out _result);
    }
}
```
