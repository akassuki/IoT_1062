import serial
import threading
import time

PORT = "COM19"
BAUDRATE = 9600

PROMPT = "Nhap ky tu / chuoi de gui (go 'exit' de thoat): "


def reader(ser, stop_event):
    while not stop_event.is_set():

        if ser.in_waiting:
            data = ser.read(ser.in_waiting)

            print("\nRX HEX <-", data.hex(" "))

            try:
                print("RX ASCII <-", data.decode(errors="replace"))
            except:
                pass

            print(PROMPT, end="", flush=True)

        time.sleep(0.01)


def main():

    ser = serial.Serial(
        PORT,
        BAUDRATE,
        timeout=0.1
    )

    print(f"Da mo cong {PORT} @ {BAUDRATE}")
    print()

    stop_event = threading.Event()

    t = threading.Thread(
        target=reader,
        args=(ser, stop_event),
        daemon=True
    )

    t.start()

    try:

        while True:

            text = input(PROMPT)

            if text.strip().lower() == "exit":
                break

            data = (text + "\r\n").encode()

            print("TX HEX ->", data.hex(" "))

            ser.write(data)

            print("TX ASCII ->", repr(text + "\r\n"))

    except KeyboardInterrupt:
        pass

    finally:

        stop_event.set()
        ser.close()

        print()
        print("Da dong cong Serial.")


if __name__ == "__main__":
    main()
    