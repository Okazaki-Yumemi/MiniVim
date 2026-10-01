import os
import pty
import fcntl
import termios
import struct
import subprocess
import re
import time
import sys


def drain(fd):
    while True:
        try:
            data = os.read(fd, 65536)
            if not data:
                return
        except BlockingIOError:
            return
        except OSError:
            return


master, slave = pty.openpty()

# 模拟 24 x 80 终端
fcntl.ioctl(
    slave,
    termios.TIOCSWINSZ,
    struct.pack("HHHH", 24, 80, 0, 0)
)

process = subprocess.Popen(
    ["./code","input.txt"],
    stdin=slave,
    stdout=slave,
    stderr=slave,
    close_fds=True
)

os.close(slave)
os.set_blocking(master, False)

script = open(sys.argv[1], encoding="utf-8").read()
parts = re.split(r'(<[^>\n]+>)', script)

mapping = {
    "<SP>": b" ",
    "<TAB>": b"\t",
    "<CR>": b"\r",
    "<BS>": b"\x7f",
    "<ESC>": b"\x1b",
    "<L>": b"<",
}

for part in parts:
    if not part:
        continue

    if part == "<E>":
        break

    if part == "<EX>":
        os.write(master, b"\x1b")
        time.sleep(0.05)

    elif part.startswith("<X"):
        m = re.fullmatch(r"<X(\d*)>", part)
        ms = int(m.group(1)) if m and m.group(1) else 50
        time.sleep(ms / 1000)

    elif part.startswith("<P-") or part == "<CURP>":
        # 这些是 vtemu 的截图指令，不发送给 MiniVim
        pass

    elif part in mapping:
        os.write(master, mapping[part])
        if part == "<ESC>":
            time.sleep(0.05)

    elif part.startswith("<"):
        print("unknown token:", part)

    else:
        # .in 文件本身的换行只是脚本排版
        raw = part.replace("\n", "").replace("\r", "")
        if raw:
            os.write(master, raw.encode())

    time.sleep(0.005)
    drain(master)


deadline = time.time() + 1.0
while process.poll() is None and time.time() < deadline:
    drain(master)
    time.sleep(0.01)

if process.poll() is None:
    print("program still running")
    process.terminate()
else:
    print("program exited:", process.returncode)