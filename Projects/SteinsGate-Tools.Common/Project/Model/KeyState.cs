/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 2:22:36 PM
 *
 */

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

using SGToolsCommon.Extension;

using Vanara.PInvoke;

namespace SGToolsCommon.Model
{
    public enum SGKey
    {
        LeftCtrl,
        LeftAlt,
        LeftShift,
        Delete,
        Space,
        Enter,
        Left,
        Up,
        Right,
        Down,
        X,
        C,
        V,
        S,
        L,
        Z,
        B,
        Escape,
        F2,
        F6,
        F7,
        F8,
        Max
    }

    public delegate void KeyDownHandler(SGKey _key);

    public class KeyState : IDisposable
    {
        // 멀티쓰레드 기반에서 키캡쳐 구현할려고했는데.
        // Keyboard 클래스자체가 UI 쓰레드에서 동작해야하네..
        // 따라서 아래와 같이 64바이트 단위로 정렬하는게 의미없는 짓이다.
        // 일단 추후 멀티 쓰레드에서도 안전하게 키입력 받을 수 있도록 구현 할수도있으니 남겨두자.
        [StructLayout(LayoutKind.Explicit, Size = Constant.CacheAlignSize, Pack = 8)]
        private struct KeyElement
        {
            [FieldOffset(0)] public Key Key;
            [FieldOffset(8)] public bool Pressed;
            [FieldOffset(16)] public bool FireContinuous;   // 키를 누르고있으면 연속 키입력 이벤트를 보낼지 설정
            [FieldOffset(24)] public long FireDelayCurMs;
            [FieldOffset(32)] public long FireDelayMs;

            //////////////////////////////////////////////////////////////////////////////////
            public KeyElement(Key _key)
            {
                Key = _key;
                Pressed = false;
                FireContinuous = false;
                FireDelayCurMs = 0;
                FireDelayMs = 150;
            }
        }

        private readonly KeyElement[] keys_;
        private readonly Thread keyCaptureThread_;
        private volatile bool isRunning_;
        private long previousTick_;
        private Stopwatch stopWatch_ = new();
        private AutoResetEvent finishHandle_;

        public event KeyDownHandler? KeyDown;
        public event KeyDownHandler? KeyUp;

        public bool IsShiftPressed => IsPressed(SGKey.LeftShift);
        public bool IsAltPressed => IsPressed(SGKey.LeftAlt);
        public bool IsCtrlPressed => IsPressed(SGKey.LeftCtrl);

        public bool IsModifierKeyPressed => keys_[(int)SGKey.LeftShift].Pressed ||
                                            keys_[(int)SGKey.LeftAlt].Pressed ||
                                            keys_[(int)SGKey.LeftCtrl].Pressed;

        private long ElapsedDeltaMs => (stopWatch_.ElapsedTicks - previousTick_) / 10000; // 틱이 100나노초 단위이므로, 밀리초단위로 변환

        //////////////////////////////////////////////////////////////////////////////////
        public KeyState()
        {
            keys_ = new KeyElement[(int)SGKey.Max];
            keys_[(int)SGKey.LeftCtrl] = new KeyElement(Key.LeftCtrl);
            keys_[(int)SGKey.LeftAlt] = new KeyElement(Key.LeftAlt);
            keys_[(int)SGKey.LeftShift] = new KeyElement(Key.LeftShift);
            keys_[(int)SGKey.Delete] = new KeyElement(Key.Delete);
            keys_[(int)SGKey.Space] = new KeyElement(Key.Space);
            keys_[(int)SGKey.Enter] = new KeyElement(Key.Enter);
            keys_[(int)SGKey.Left] = new KeyElement(Key.Left);
            keys_[(int)SGKey.Up] = new KeyElement(Key.Up);
            keys_[(int)SGKey.Right] = new KeyElement(Key.Right);
            keys_[(int)SGKey.Down] = new KeyElement(Key.Down);
            keys_[(int)SGKey.X] = new KeyElement(Key.X);
            keys_[(int)SGKey.C] = new KeyElement(Key.C);
            keys_[(int)SGKey.V] = new KeyElement(Key.V);
            keys_[(int)SGKey.S] = new KeyElement(Key.S);
            keys_[(int)SGKey.L] = new KeyElement(Key.L);
            keys_[(int)SGKey.Z] = new KeyElement(Key.Z);
            keys_[(int)SGKey.B] = new KeyElement(Key.B);
            keys_[(int)SGKey.Escape] = new KeyElement(Key.Escape);
            keys_[(int)SGKey.F2] = new KeyElement(Key.F2);
            keys_[(int)SGKey.F6] = new KeyElement(Key.F6);
            keys_[(int)SGKey.F7] = new KeyElement(Key.F7);
            keys_[(int)SGKey.F8] = new KeyElement(Key.F8);

            // 방향키 연속키입력 허용
            SetEnableFireContinuous(SGKey.Left, true);
            SetEnableFireContinuous(SGKey.Up, true);
            SetEnableFireContinuous(SGKey.Right, true);
            SetEnableFireContinuous(SGKey.Down, true);

            isRunning_ = true;
            finishHandle_ = new AutoResetEvent(false);
            keyCaptureThread_ = new Thread(CaptureThreadRoutine);
            keyCaptureThread_.Start();
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void Dispose()
        {
            isRunning_ = false;

            /*
             * 외부쓰레드에서 종료를 안받으면 데드락이 걸릴 수가 있다.
             *
             * Dispatcher.Invoke 시작
             *         -> WPF 작업
             *         -> WPF 작업 2
             *         -> WPF 작업 3 -> 내가 종료버튼 누른걸 처리
             * Dispatcher.Invoke 종료
             * Thread.Sleep(10)
             *
             * 운이 안좋아서 Dispatcher.Invoke가 실행중일때
             * 이벤트 루틴이 처리중일 때 내가 타이밍 맞게 종료를 눌러버리면
             * Invoke가 수행중일 때 종료 이벤트가 처리될 수있다.
             *
             * 그렇게되면 UI 쓰레드에서 캡쳐 쓰레드가 종료되는걸 기다리고
             * 캡쳐 쓰레드가 종료될려면 UI 쓰레드에서 실행중인 Dispatcher.Invoke 함수의 종료가 완료되어야하는 연쇄가 발생한다.
             * https://stackoverflow.com/questions/24211934/deadlock-when-thread-uses-dispatcher-and-the-main-thread-is-waiting-for-thread-t
             */
            Task.Run(() =>
            {
                finishHandle_.WaitOne();
                keyCaptureThread_.Join();
            });
        }

        //////////////////////////////////////////////////////////////////////////////////
        public void SetEnableFireContinuous(SGKey _key, bool _fire, long _delay = 70)
        {
            int idx = (int)_key;
            keys_[idx].FireContinuous = _fire;
            keys_[idx].FireDelayMs = _delay;
        }

        //////////////////////////////////////////////////////////////////////////////////
        public bool IsPressed(SGKey _key) => keys_[(int)_key].Pressed;

        //////////////////////////////////////////////////////////////////////////////////
        private void CaptureThreadRoutine()
        {
            stopWatch_.Start();
            while (isRunning_)
            {
                Application.Current?.Dispatcher.Invoke(CaptureKeyState);
                previousTick_ = stopWatch_.ElapsedTicks;
                Thread.Sleep(10);
            }
            finishHandle_.Set();
        }

        //////////////////////////////////////////////////////////////////////////////////
        private void CaptureKeyState()
        {
            if (!WindowEx.IsMainWindowForeground())
                return;

            SGKey sgKey;
            Key wpfKey;
            bool isKeyDown;

            for (int i = 0; i < keys_.Length; ++i)
            {
                sgKey = (SGKey)i;
                wpfKey = keys_[i].Key;
                isKeyDown = Keyboard.IsKeyDown(wpfKey);
                bool isKeyDownInvoked = false;

                if (isKeyDown && !keys_[i].Pressed)
                {
                    keys_[i].Pressed = true;
                    KeyDown?.Invoke(sgKey);
                    isKeyDownInvoked = true;
                }

                if (!isKeyDown && keys_[i].Pressed)
                {
                    keys_[i].FireDelayCurMs = 0;
                    keys_[i].Pressed = false;
                    KeyUp?.Invoke(sgKey);
                }

                // 키 입력이벤트를 수행안했고, 키가 눌린 상태이고, 해당 키가 연속 키입력을 허용하는 경우
                // 따다다다다다 키이벤트 보낸다.
                if (!isKeyDownInvoked && isKeyDown && keys_[i].FireContinuous)
                {
                    keys_[i].FireDelayCurMs += ElapsedDeltaMs;

                    if (keys_[i].FireDelayCurMs >= keys_[i].FireDelayMs)
                    {
                        keys_[i].FireDelayCurMs = 0;
                        KeyDown?.Invoke(sgKey);
                    }
                }
            }
        }
    }
}
