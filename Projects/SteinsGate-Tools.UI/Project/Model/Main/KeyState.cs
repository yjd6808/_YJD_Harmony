/*
 * 작성자: 윤정도
 * 생성일: 3/3/2023 2:22:36 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using SGToolsCommon;
using SGToolsCommon.Extension;
using Vanara.PInvoke;

namespace SGToolsUI.Model.Main
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

    public delegate void KeyDownHandler(SGKey key);

    public class KeyState : IDisposable
    {
        // 멀티쓰레드 기반에서 키캡쳐 구현할려고했는데.
        // Keyboard 클래스자체가 UI 쓰레드에서 동작해야하네..
        // 따라서 아래와 같이 64바이트 단위로 정렬하는게 의미없는 짓이다.
        // 일단 추후 멀티 쓰레드에서도 안전하게 키입력 받을 수 있도록 구현 할수도있으니 남겨두자.
        [StructLayout(LayoutKind.Explicit, Size = Constant.CacheAlignSize, Pack = 8)]
        private struct KeyElement
        {
            public KeyElement(Key key)
            {
                Key = key;
                Pressed = false;
                FireContinuous = false;
                FireDelayCurMs = 0;
                FireDelayMs = 150;
            }

            [FieldOffset(0)] public Key Key;
            [FieldOffset(8)] public bool Pressed;
            [FieldOffset(16)] public bool FireContinuous;   // 키를 누르고있으면 연속 키입력 이벤트를 보낼지 설정
            [FieldOffset(24)] public long FireDelayCurMs;
            [FieldOffset(32)] public long FireDelayMs;
        }

        public KeyState()
        {
            _keys = new KeyElement[(int)SGKey.Max];
            _keys[(int)SGKey.LeftCtrl] = new KeyElement(Key.LeftCtrl);
            _keys[(int)SGKey.LeftAlt] = new KeyElement(Key.LeftAlt);
            _keys[(int)SGKey.LeftShift] = new KeyElement(Key.LeftShift);
            _keys[(int)SGKey.Delete] = new KeyElement(Key.Delete);
            _keys[(int)SGKey.Space] = new KeyElement(Key.Space);
            _keys[(int)SGKey.Enter] = new KeyElement(Key.Enter);
            _keys[(int)SGKey.Left] = new KeyElement(Key.Left);
            _keys[(int)SGKey.Up] = new KeyElement(Key.Up);
            _keys[(int)SGKey.Right] = new KeyElement(Key.Right);
            _keys[(int)SGKey.Down] = new KeyElement(Key.Down);
            _keys[(int)SGKey.X] = new KeyElement(Key.X);
            _keys[(int)SGKey.C] = new KeyElement(Key.C);
            _keys[(int)SGKey.V] = new KeyElement(Key.V);
            _keys[(int)SGKey.S] = new KeyElement(Key.S);
            _keys[(int)SGKey.L] = new KeyElement(Key.L);
            _keys[(int)SGKey.Z] = new KeyElement(Key.Z);
            _keys[(int)SGKey.B] = new KeyElement(Key.B);
            _keys[(int)SGKey.Escape] = new KeyElement(Key.Escape);
            _keys[(int)SGKey.F2] = new KeyElement(Key.F2);
            _keys[(int)SGKey.F6] = new KeyElement(Key.F6);
            _keys[(int)SGKey.F7] = new KeyElement(Key.F7);
            _keys[(int)SGKey.F8] = new KeyElement(Key.F8);
            


            // 방향키 연속키입력 허용
            SetEnableFireContinuous(SGKey.Left, true);
            SetEnableFireContinuous(SGKey.Up, true);
            SetEnableFireContinuous(SGKey.Right, true);
            SetEnableFireContinuous(SGKey.Down, true);



            _isRunning = true;
            _finishHandle = new AutoResetEvent(false);
            _keyCaptureThread = new Thread(CaptureThreadRoutine);
            _keyCaptureThread.Start();
        }

        private void CaptureThreadRoutine()
        {
            _stopWatch.Start();
            while (_isRunning)
            {
                Application.Current?.Dispatcher.Invoke(CaptureKeyState);
                _previousTick = _stopWatch.ElapsedTicks;
                Thread.Sleep(10);
            }
            _finishHandle.Set();
        }

        private void CaptureKeyState()
        {
            if (!WindowEx.IsMainWindowForeground())
                return;

            SGKey sgKey;
            Key wpfKey;
            bool isKeyDown;

            for (int i = 0; i < _keys.Length; ++i)
            {
                sgKey = (SGKey)i;
                wpfKey = _keys[i].Key;
                isKeyDown = Keyboard.IsKeyDown(wpfKey);
                bool isKeyDownInvoked = false;

                if (isKeyDown && !_keys[i].Pressed)
                {
                    _keys[i].Pressed = true;
                    KeyDown?.Invoke(sgKey);
                    isKeyDownInvoked = true;
                }

                if (!isKeyDown && _keys[i].Pressed)
                {
                    _keys[i].FireDelayCurMs = 0;
                    _keys[i].Pressed = false;
                    KeyUp?.Invoke(sgKey);
                }

                // 키 입력이벤트를 수행안했고, 키가 눌린 상태이고, 해당 키가 연속 키입력을 허용하는 경우
                // 따다다다다다 키이벤트 보낸다.
                if (!isKeyDownInvoked && isKeyDown && _keys[i].FireContinuous)
                {
                    _keys[i].FireDelayCurMs += ElapsedDeltaMs;

                    if (_keys[i].FireDelayCurMs >= _keys[i].FireDelayMs)
                    {
                        _keys[i].FireDelayCurMs = 0;
                        KeyDown?.Invoke(sgKey);
                    }
                }
            }
        }

        private long ElapsedDeltaMs => (_stopWatch.ElapsedTicks - _previousTick) / 10000; // 틱이 100나노초 단위이므로, 밀리초단위로 변환

        public void Dispose()
        {
            _isRunning = false;


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
                _finishHandle.WaitOne();
                _keyCaptureThread.Join();
            });
        }


        public void SetEnableFireContinuous(SGKey key, bool fire, long delay = 70)
        {
            int idx = (int)key;

            _keys[idx].FireContinuous = fire;
            _keys[idx].FireDelayMs = delay;
        }
        public bool IsPressed(SGKey key) => _keys[(int)key].Pressed;
        public bool IsShiftPressed => IsPressed(SGKey.LeftShift);
        public bool IsAltPressed => IsPressed(SGKey.LeftAlt);
        public bool IsCtrlPressed => IsPressed(SGKey.LeftCtrl);

        public bool IsModifierKeyPressed => _keys[(int)SGKey.LeftShift].Pressed ||
                                            _keys[(int)SGKey.LeftAlt].Pressed ||
                                            _keys[(int)SGKey.LeftCtrl].Pressed;


        private readonly KeyElement[] _keys;
        private readonly Thread _keyCaptureThread;
        private volatile bool _isRunning;
        private long _previousTick;
        private Stopwatch _stopWatch = new();
        public event KeyDownHandler KeyDown;
        public event KeyDownHandler KeyUp;
        private AutoResetEvent _finishHandle;
    }
}

