import subprocess
import os
import signal
import time

while True:
    try:
        resultsStr = subprocess.check_output(["pgrep", "start_actor"]).decode("utf-8")

        # Create list
        resultsList = resultsStr.splitlines()

        if len(resultsList) == 0:
            break
        else:
            for pid in resultsList:
                os.kill(int(pid), signal.SIGINT)
                time.sleep(5)


    except subprocess.CalledProcessError as e:
        print("No process with the given name")
        break
