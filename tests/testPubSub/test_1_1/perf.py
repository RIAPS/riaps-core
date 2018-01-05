
import os
import zopkio.runtime as runtime

LOGS_DIRECTORY = "/tmp/riaps_test/collected_logs/testPubSub/test_1_1/"
OUTPUT_DIRECTORY = "/tmp/riaps_test/results/"

def machine_logs():

  results = {}

  for target in runtime.get_active_config("targets"):
    pubfirstKey = "pubfirst_" + target["actor"]
    subfirstKey = "subfirst_" + target["actor"]

    logpath = "/tmp/{0}.log".format(pubfirstKey)
    results[pubfirstKey] = [logpath]

    logpath = "/tmp/{0}.log".format(subfirstKey)
    results[subfirstKey] = [logpath]

    #for testcase in runtime.get_active_config("testcases"):
    #  logpath = "/tmp/{0}_{1}_{2}.log".format(runtime.get_active_config("app_dir"), target["actor"], testcase)
    #  results[target["actor"]].append(logpath)


  return results
  # return {
  #   "modelDeployer": ["/tmp/test_1_1.log"]
  # }

def naarad_logs():
  return {

  }

def naarad_config():
  return os.path.join(os.path.dirname(os.path.abspath(__file__)), "naarad.cfg")
