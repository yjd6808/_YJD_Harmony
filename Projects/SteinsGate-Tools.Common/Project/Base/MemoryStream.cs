// NativeProcess/Net/PE 의존성 제거 버전
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace SGToolsCommon.Base
{
    /// <summary>
    /// 커스텀 바이너리 메모리 스트림.
    /// 관리형 byte[] 버퍼를 사용하여 읽기/쓰기 오프셋을 별도로 관리한다.
    /// </summary>
    public class MemoryStream
    {
        private byte[]? managedBuffer_ = null;
        private int len_ = 0;
        private bool isOwner_ = true;
        private int readOffset_ = 0;
        private int writeOffset_ = 0;

        public MemoryStream()
        {
        }

        public MemoryStream(byte[] _buffer, bool _owner = true)
        {
            managedBuffer_ = _buffer;
            isOwner_ = _owner;
            len_ = _buffer.Length;
        }

        public MemoryStream(int _size, bool _owner = true)
        {
            managedBuffer_ = new byte[_size];
            isOwner_ = _owner;
            len_ = _size;
        }

        ~MemoryStream()
        {
            Free();
        }

        public MemoryStream Clone()
        {
            MemoryStream stream = new MemoryStream();
            stream.len_ = len_;
            stream.readOffset_ = readOffset_;
            stream.writeOffset_ = writeOffset_;
            stream.isOwner_ = isOwner_;
            if (managedBuffer_ != null)
            {
                stream.managedBuffer_ = new byte[managedBuffer_.Length];
                Array.Copy(managedBuffer_, 0, stream.managedBuffer_, 0, len_);
            }
            return stream;
        }

        public bool IsNull() => managedBuffer_ == null;
        public bool IsOwner() => isOwner_;

        public byte[]? DataManaged() => managedBuffer_;

        public Span<byte> DataManagedFromReadOffset()
        {
            if (managedBuffer_ == null)
                return Span<byte>.Empty;
            return new Span<byte>(managedBuffer_, readOffset_, len_ - readOffset_);
        }

        public byte[] DataManagedFromReadOffsetReadable()
        {
            int len = GetReadableSize();
            if (len <= 0 || managedBuffer_ == null)
                return Array.Empty<byte>();
            byte[] bytes = new byte[len];
            Array.Copy(managedBuffer_, readOffset_, bytes, 0, len);
            return bytes;
        }

        public Span<byte> DataManagedFromWriteOffset()
        {
            if (managedBuffer_ == null)
                return Span<byte>.Empty;
            return new Span<byte>(managedBuffer_, writeOffset_, len_ - writeOffset_);
        }

        public int Length() => len_;
        public uint LengthU32() => (uint)len_;

        public void SetWriteOffset(uint _offset) { writeOffset_ = (int)_offset; }
        public void SetWriteOffset(int _offset)  { writeOffset_ = _offset; }
        public void SetReadOffset(uint _offset)  { readOffset_ = (int)_offset; }
        public void SetReadOffset(int _offset)   { readOffset_ = _offset; }

        public int GetWriteOffset()    => writeOffset_;
        public int GetReadOffset()     => readOffset_;
        public int GetReadableSize()   => writeOffset_ - readOffset_;
        public int GetWriteableSize()
        {
            if (IsNull()) return 0;
            return len_ - writeOffset_;
        }

        public void Free()
        {
            if (!isOwner_) return;
            // 관리형 버퍼는 GC가 처리하므로 참조만 null로 설정
            managedBuffer_ = null;
        }

        public void ExpandAllocateIfNeeded(int _minimumSize = 0)
        {
            if (!isOwner_) return;

            const int DEFAULT_BUF_SIZE = 1 << 12;

            if (IsNull())
            {
                managedBuffer_ = new byte[DEFAULT_BUF_SIZE];
                len_ = DEFAULT_BUF_SIZE;
            }
            else if (GetWriteableSize() < _minimumSize)
            {
                int newLen = len_ * 2;
                byte[] newBuffer = new byte[newLen];
                Array.Copy(managedBuffer_!, 0, newBuffer, 0, len_);
                managedBuffer_ = newBuffer;
                len_ = newLen;
            }
        }

        // ── Write 메서드들 ──────────────────────────────────────────────

        public void WriteBool(bool _value)
        {
            ExpandAllocateIfNeeded(sizeof(byte));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteS8(sbyte _value)
        {
            ExpandAllocateIfNeeded(sizeof(sbyte));
            WriteInternal(new[] { (byte)_value });
        }

        public void WriteS16(short _value)
        {
            ExpandAllocateIfNeeded(sizeof(short));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteS32(int _value)
        {
            ExpandAllocateIfNeeded(sizeof(int));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteS64(long _value)
        {
            ExpandAllocateIfNeeded(sizeof(long));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteU8(byte _value)
        {
            ExpandAllocateIfNeeded(sizeof(byte));
            WriteInternal(new byte[] { _value });
        }

        public void WriteU16(ushort _value)
        {
            ExpandAllocateIfNeeded(sizeof(ushort));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteU32(uint _value)
        {
            ExpandAllocateIfNeeded(sizeof(uint));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteU64(ulong _value)
        {
            ExpandAllocateIfNeeded(sizeof(ulong));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteByte(byte _value)
        {
            ExpandAllocateIfNeeded(sizeof(byte));
            WriteInternal(new byte[] { _value });
        }

        public void WriteFloat(float _value)
        {
            ExpandAllocateIfNeeded(sizeof(float));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WriteDouble(double _value)
        {
            ExpandAllocateIfNeeded(sizeof(double));
            WriteInternal(BitConverter.GetBytes(_value));
        }

        public void WritePtr(IntPtr _value)
        {
            ExpandAllocateIfNeeded(IntPtr.Size);
            if (IntPtr.Size == 8)
                WriteInternal(BitConverter.GetBytes(_value.ToInt64()));
            else
                WriteInternal(BitConverter.GetBytes(_value.ToInt32()));
        }

        public void WriteString(string _value)
        {
            byte[] byteArray = System.Text.Encoding.Unicode.GetBytes(_value);
            ExpandAllocateIfNeeded(_value.Length * sizeof(char));
            WriteS32(_value.Length);
            WriteInternal(byteArray);
        }

        public void Write(byte[] _data)
        {
            ExpandAllocateIfNeeded(_data.Length);
            WriteInternal(_data);
        }

        public void WriteStream(MemoryStream _stream)
        {
            byte[] readableBytes = _stream.DataManagedFromReadOffsetReadable();
            if (readableBytes.Length > 0)
            {
                ExpandAllocateIfNeeded(readableBytes.Length);
                WriteInternal(readableBytes);
            }
        }

        private bool WriteInternal(byte[] _data)
        {
            if (IsNull()) return false;
            if (writeOffset_ + _data.Length > len_) return false;

            Array.Copy(_data, 0, managedBuffer_!, writeOffset_, _data.Length);
            writeOffset_ += _data.Length;
            return true;
        }

        // ── Read 메서드들 ────────────────────────────────────────────────

        public bool ReadBool(bool _pop = false)
        {
            byte[] data = new byte[sizeof(bool)];
            if (!Read(data, 0, data.Length)) return false;
            if (_pop) PopReads();
            return BitConverter.ToBoolean(data, 0);
        }

        public sbyte ReadS8(bool _pop = false)
        {
            byte[] data = new byte[sizeof(sbyte)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return (sbyte)data[0];
        }

        public short ReadS16(bool _pop = false)
        {
            byte[] data = new byte[sizeof(short)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToInt16(data, 0);
        }

        public int ReadS32(bool _pop = false)
        {
            byte[] data = new byte[sizeof(int)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToInt32(data, 0);
        }

        public long ReadS64(bool _pop = false)
        {
            byte[] data = new byte[sizeof(long)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToInt64(data, 0);
        }

        public byte ReadU8(bool _pop = false)
        {
            byte[] data = new byte[sizeof(byte)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return data[0];
        }

        public ushort ReadU16(bool _pop = false)
        {
            byte[] data = new byte[sizeof(ushort)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToUInt16(data, 0);
        }

        public uint ReadU32(bool _pop = false)
        {
            byte[] data = new byte[sizeof(uint)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToUInt32(data, 0);
        }

        public ulong ReadU64(bool _pop = false)
        {
            byte[] data = new byte[sizeof(ulong)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToUInt64(data, 0);
        }

        public float ReadFloat(bool _pop = false)
        {
            byte[] data = new byte[sizeof(float)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToSingle(data, 0);
        }

        public double ReadDouble(bool _pop = false)
        {
            byte[] data = new byte[sizeof(double)];
            if (!Read(data, 0, data.Length)) return 0;
            if (_pop) PopReads();
            return BitConverter.ToDouble(data, 0);
        }

        public IntPtr ReadPtr(bool _pop = false)
        {
            if (IntPtr.Size == 4) return new IntPtr(ReadS32(_pop));
            return new IntPtr(ReadS64(_pop));
        }

        public string ReadString(int _capacity = int.MaxValue, bool _pop = false)
        {
            if (IsNull()) return string.Empty;

            int length = ReadS32();
            if (length <= 0 || length >= len_)
            {
                Debug.Assert(false);
                return string.Empty;
            }

            if (_capacity != int.MaxValue && length > _capacity)
            {
                length = _capacity;
                Debug.Assert(false, "스트림의 문자열 길이가 _capacity를 초과합니다. 잘라서 가져옵니다.");
            }

            byte[] data = new byte[length * sizeof(char)];
            if (!Read(data, 0, data.Length)) return string.Empty;
            if (_pop) PopReads();

            return System.Text.Encoding.Unicode.GetString(data, 0, length * sizeof(char));
        }

        public MemoryStream? ReadStream(int _length)
        {
            if (GetReadableSize() < _length) return null;

            byte[] newBuf = new byte[_length];
            Read(newBuf, 0, _length);
            MemoryStream stream = new MemoryStream();
            stream.managedBuffer_ = newBuf;
            stream.len_ = _length;
            stream.SetWriteOffset(_length);
            return stream;
        }

        public unsafe bool Read(byte[] _value, int _offset, int _length)
        {
            fixed (byte* p = _value)
            {
                IntPtr ptr = (IntPtr)p;
                return Read(ptr, _offset, _length);
            }
        }

        public bool Read(IntPtr _nativeBuf, int _offset, int _length)
        {
            if (Peek(_nativeBuf, _offset, _length))
            {
                readOffset_ += _length;
                return true;
            }
            return false;
        }

        public void PopReads()
        {
            Debug.Assert(writeOffset_ >= readOffset_);
            Debug.Assert(managedBuffer_ != null);
            Array.Copy(managedBuffer_!, readOffset_, managedBuffer_!, 0, writeOffset_ - readOffset_);
            writeOffset_ -= readOffset_;
            readOffset_ = 0;
        }

        public int PeekS32()
        {
            if (IsNull()) return 0;
            byte[] data = new byte[sizeof(int)];
            if (!Peek(data, 0, data.Length)) return 0;
            return BitConverter.ToInt32(data, 0);
        }

        private bool Peek(IntPtr _nativeBuf, int _offset, int _length)
        {
            if (IsNull()) { Debug.Assert(false); return false; }
            Debug.Assert(_length >= 0);

            if (readOffset_ + _length > writeOffset_) { Debug.Assert(false); return false; }

            Marshal.Copy(managedBuffer_!, readOffset_, _nativeBuf + _offset, _length);
            return true;
        }

        private unsafe bool Peek(byte[] _value, int _offset, int _length)
        {
            fixed (byte* p = _value)
            {
                IntPtr ptr = (IntPtr)p;
                return Peek(ptr, _offset, _length);
            }
        }

        // ── WriteOffset 계열 (특정 오프셋에 직접 덮어쓰기) ─────────────

        public void WriteOffsetBool(bool _data, int _offset)   => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetByte(byte _data, int _offset)   => OverwriteInternal(BitConverter.GetBytes((short)_data), _offset);
        public void WriteOffsetS16(short _data, int _offset)   => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetU16(ushort _data, int _offset)  => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetChar(char _data, int _offset)   => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetS32(int _data, int _offset)     => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetU32(uint _data, int _offset)    => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetS64(long _data, int _offset)    => OverwriteInternal(BitConverter.GetBytes(_data), _offset);
        public void WriteOffsetU64(ulong _data, int _offset)   => OverwriteInternal(BitConverter.GetBytes(_data), _offset);

        public void WriteOffsetPtr(IntPtr _data, int _offset)
        {
            byte[] data = IntPtr.Size == 8
                ? BitConverter.GetBytes(_data.ToInt64())
                : BitConverter.GetBytes(_data.ToInt32());
            OverwriteInternal(data, _offset);
        }

        private void OverwriteInternal(byte[] _data, int _offset)
        {
            if (IsNull())
            {
                Debug.Assert(false, "덮어쓸 메모리가 비어있습니다.");
                return;
            }
            if (_offset + _data.Length > len_)
            {
                Debug.Assert(false, "덮어쓸 메모리 공간이 부족합니다.");
                return;
            }
            Array.Copy(_data, 0, managedBuffer_!, _offset, _data.Length);
        }
    }
}
