from fabric.api import env, run, parallel, local, runs_once, lcd, put
import os
import subprocess

env.hosts = ['192.168.1.120', '192.168.1.121', '192.168.1.122']

appName = "Groupmsgtest"
files = ["libcompone.so", "libcomptwo.so", "libcompthree.so"]

paths = [("./json-gen/%s_app.json" % appName, "~/riaps_apps/%s/%s_app.json" % (appName, appName))]

for file in files:
    paths.append(("./build/armhf/bin/%s"%file, "~/riaps_apps/%s/%s"%(appName, file)))

@runs_once
def build():
    local("rm -rf ./build")
    local("mkdir ./build")
    with lcd("build"):
        local("cmake -DCMAKE_TOOLCHAIN_FILE=.toolchain.arm-linux-gnueabihf.cmake -DCMAKE_INSTALL_PREFIX=\"/\" ../")
        local("make")

@runs_once
def compileCapnp():
    with lcd("./messages-gen"):
        local("capnp compile -oc++ *.capnp")       

@parallel
def deploy():
    run("rm -rf ~/riaps_apps/%s" % appName)
    run("mkdir ~/riaps_apps/%s" % appName)
    copy()

def copy():
    for copyItem in paths:
        put(copyItem[0], copyItem[1])
