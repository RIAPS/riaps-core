import os.path
import errno

tmppath = "/tmp"
founddisco = False
for (path, dirs, files) in os.walk(tmppath):
    if tmppath == path:
        for file in files:
            if file.startswith("riaps-disco") and len(file) == 23:
                founddisco = True
                break;
    if founddisco:
        break;

if not founddisco:
    raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT), "riaps-disco")