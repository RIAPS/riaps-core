from fabric.api import env, run, parallel, local, runs_once, lcd, put

env.hosts = ['192.168.1.120', '192.168.1.121', '192.168.1.122']

files = [("./json-gen/Grouptest_app.json", "~/riaps_apps/Grouptest/Grouptest_app.json"),
         ("./build/armhf/bin/libgroupcomp.so", "~/riaps_apps/Grouptest/libgroupcomp.so")]

@runs_once
def build():
    local("rm -rf ./build")
    local("mkdir ./build")
    with lcd("build"):
        local("cmake -DCMAKE_TOOLCHAIN_FILE=.toolchain.arm-linux-gnueabihf.cmake -DCMAKE_INSTALL_PREFIX=\"/\" ../")
        local("make")

@parallel
def deployApp():
    run("rm -rf ~/riaps_apps/Grouptest")
    run("mkdir ~/riaps_apps/Grouptest")
    copy()

def copy():
    for copyItem in files:
        put(copyItem[0], copyItem[1])
