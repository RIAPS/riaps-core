import os
import time
from inotify_simple import INotify, flags


inotify = INotify()
watch_flags = flags.MODIFY
p = os.getcwd()
p = os.path.join(p, "source")
print('Watching: ' + p)
wd = inotify.add_watch(p, watch_flags)

# And see the corresponding events:
while True:
    time.sleep(1)
    for event in inotify.read():
        print(event)
        for flag in flags.from_mask(event.mask):
            print('    ' + str(flag))
            os.system('make html')