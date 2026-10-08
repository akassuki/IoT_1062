import serial
import threading

PORT = "COM17"   # Đổi cho đúng máy bạn
BAUD = 9600

# Địa chỉ module này (PC) — theo Address/Channel đã set trong RF Setting
MY_ADDH, MY_ADDL, MY_CHAN = 0x00, 0x00, 21

# Địa chỉ module bên kia (ESP32) cần gửi tới
PEER_ADDH, PEER_ADDL, PEER_CHAN = 0x02, 0x00, 21

ser = serial.Serial(PORT, BAUD, timeout=0.1)


def send_message(text: str):
    packet = bytes([PEER_ADDH, PEER_ADDL, PEER_CHAN]) + text.encode("utf-8") + b"\n"
    ser.write(packet)
    print(f"[Đã gửi] {text}")


def reader_thread():
    buf = b""
    while True:
        data = ser.read(ser.in_waiting or 1)
        if not data:
            continue
        buf += data
        while b"\n" in buf:
            line, buf = buf.split(b"\n", 1)
            if len(line) >= 3 and line[0] == MY_ADDH and line[1] == MY_ADDL and line[2] == MY_CHAN:
                line = line[3:]
            text = line.decode("utf-8", errors="replace")
            if text:
                print(f"\n[Nhận được] {text}\n> ", end="", flush=True)


def main():
    t = threading.Thread(target=reader_thread, daemon=True)
    t.start()
    print(f"Đã mở {PORT} @ {BAUD}bps. Gõ tin nhắn và Enter để gửi (Ctrl+C để thoát).")
    try:
        while True:
            text = input("> ")
            if text.strip():
                send_message(text)
    except KeyboardInterrupt:
        print("\nDừng chương trình.")
    finally:
        ser.close()


if __name__ == "__main__":
    main()

