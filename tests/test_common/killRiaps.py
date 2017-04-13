import subprocess
import os
import signal

while True:
    try:
        resultsStr = subprocess.check_output(["pgrep", "riaps"]).decode("utf-8")

        # Create list
        resultsList = resultsStr.splitlines()

        if len(resultsList) == 0:
            break
        else:
            for pid in resultsList:
                os.kill(int(pid), signal.SIGKILL)

    except subprocess.CalledProcessError as e:
        print("No process with the given name")
        break
