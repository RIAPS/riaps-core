from subprocess import check_output
import subprocess
import os
import signal

def get_pid(name):
    return check_output(["pidof",name])

try:
    pid = get_pid("rdiscoveryd").decode("utf-8")

    if pid != "":
        os.kill(int(pid), signal.SIGINT)
except subprocess.CalledProcessError as e:
    print("No process with the given name")
