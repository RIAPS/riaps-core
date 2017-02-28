import zopkio.runtime as runtime
import zopkio.test_utils as testutilities
import os
import perf
from time import sleep


def test_pub_send_pub_first():

    print("Start test: pub_send_pub_first")

    pubActorName = "ActorTest1p"
    subActorName = "ActorTest1s"

    testId = "pubfirst_" + pubActorName
    deployer = runtime.get_deployer(pubActorName)
    deployer.start(testId, configs={"sync": False,
                                    'args': [runtime.get_active_config('app_dir'),
                                             runtime.get_active_config('app_dir') + '.json',
                                             pubActorName,
                                             '--logfile="' + testId + '.log"']})

    # start test, publisher comes first
    #for target in runtime.get_active_config("targets"):
    #    if target["actor"] == pubActorName:
    #        deployer = runtime.get_deployer(target["actor"])
    #        deployer.start(target["actor"], configs={"sync": False})

    sleep(2)

    testId = "pubfirst_" + subActorName
    deployer = runtime.get_deployer(subActorName)
    deployer.start(testId, configs={"sync": False,
                                    'args': [runtime.get_active_config('app_dir'),
                                             runtime.get_active_config('app_dir') + '.json',
                                             subActorName,
                                             '--logfile="' + testId + '.log"']})


    # start subscriber
    #for target in runtime.get_active_config("targets"):
    #    if target["actor"] == subActorName:
    #        deployer = runtime.get_deployer(target["actor"])
    #        deployer.start(target["actor"], configs={"sync": False})

    sleep(30)


def test_pub_send_sub_first():

    pubActorName = "ActorTest1p"
    subActorName = "ActorTest1s"

    # start test, subscriber comes first
    #for target in runtime.get_active_config("targets"):
    #    if target["actor"] == subActorName:
    #        deployer = runtime.get_deployer(target["actor"])
    #        deployer.start(target["actor"], configs={"sync": False})
    testId = "subfirst_" + subActorName
    deployer = runtime.get_deployer(subActorName)
    deployer.start(testId, configs={"sync": False,
                                    'args': [runtime.get_active_config('app_dir'),
                                             runtime.get_active_config('app_dir') + '.json',
                                             subActorName,
                                             '--logfile="' + testId + '.log"']})

    sleep(2)



    # start publisher
    testId = "subfirst_" + pubActorName
    deployer = runtime.get_deployer(pubActorName)
    deployer.start(testId, configs={"sync": False,
                                    'args': [runtime.get_active_config('app_dir'),
                                             runtime.get_active_config('app_dir') + '.json',
                                             pubActorName,
                                             '--logfile="' + testId + '.log"']})


    #for target in runtime.get_active_config("targets"):
    #    if target["actor"] == pubActorName:
    #        deployer = runtime.get_deployer(target["actor"])
     #       deployer.start(target["actor"], configs={"sync": False})

    sleep(30)


def validate_pub_send_pub_first():
    print("Validate, pubsub 1:1 pub first")

    pubActorName = "ActorTest1p"
    subActorName = "ActorTest1s"

    testcase = "pubfirst_" + pubActorName

    pub_log_name = "{0}-{1}_{2}.log".format(testcase, "pubfirst", pubActorName)
    pub_log_file = os.path.join(perf.LOGS_DIRECTORY, pub_log_name)
    pub_logs = testutilities.get_log_for_test("test_pub_send_pub_first", pub_log_file, "12:00:00")
    assert "Sent messages: 1" in pub_logs, "Publisher couldn't send any messages"
    assert "Sent messages: 5" in pub_logs, "Publisher couldn't send at least 5 messages"

    testcase = "pubfirst_" + subActorName
    sub_log_name = "{0}-{1}.log".format(testcase, testcase)
    sub_log_file = os.path.join(perf.LOGS_DIRECTORY, sub_log_name)
    sub_logs = testutilities.get_log_for_test("test_pub_send_pub_first", sub_log_file, "12:00:00")

    assert "Received messages: 10" in sub_logs, "Subscriber didn't get 10 messages"

def validate_pub_send_sub_first():
    print("Validate, pubsub 1:1 sub first")

    pubActorName = "ActorTest1p"
    subActorName = "ActorTest1s"

    testcase = "subfirst_" + pubActorName

    pub_log_name = "{0}-{1}.log".format(testcase, testcase)
    pub_log_file = os.path.join(perf.LOGS_DIRECTORY, pub_log_name)
    pub_logs = testutilities.get_log_for_test("test_pub_send_sub_first", pub_log_file, "12:00:00")
    assert "Sent messages: 1" in pub_logs, "Publisher couldn't send any messages"
    assert "Sent messages: 5" in pub_logs, "Publisher couldn't send 5 messages"

    testcase = "subfirst_" + subActorName
    sub_log_name = "{0}-{1}.log".format(testcase, testcase)
    sub_log_file = os.path.join(perf.LOGS_DIRECTORY, sub_log_name)
    sub_logs = testutilities.get_log_for_test("test_pub_send_pub_first", sub_log_file, "12:00:00")

    assert "Received messages: 10" in sub_logs, "Subscriber didn't get 10 messages"


'''
def validate_pub_send():
    print("Validate")

    pubActorName = "ActorTest1p"
    subActorName = "ActorTest1s"
    appName = "test_1_1"

    # Check the configuration, please adjust the names according to the config.json and components log (if necessary)
    # We make sure, that we collect the right logs
    assert runtime.get_active_config("targets")[0]["actor"] == pubActorName, "Publisher actor name doesn't match"
    assert runtime.get_active_config("targets")[1]["actor"] == subActorName, "Subscriber actor name doesn't match"
    assert runtime.get_active_config("app_dir") == appName, "Application name doesn't match"

    pub_log_name = "{0}-{1}_{2}.log".format(pubActorName, appName,pubActorName)
    pub_log_file = os.path.join(perf.LOGS_DIRECTORY, pub_log_name)
    pub_logs = testutilities.get_log_for_test("test_pub_send", pub_log_file, "12:00:00")
    assert "Sent messages: 1" in pub_logs, "Publisher couldn't send any messages"
    assert "Sent messages: 5" in pub_logs, "Publisher couldn't send 5 messages"

    sub_log_name = "{0}-{1}_{2}.log".format(subActorName, appName, subActorName)
    sub_log_file = os.path.join(perf.LOGS_DIRECTORY, sub_log_name)
    sub_logs = testutilities.get_log_for_test("test_pub_send", sub_log_file, "12:00:00")

    assert "Received messages: 1" in sub_logs, "Subscriber didn't get any messages"
    assert "Received messages: 5" in sub_logs, "Subscriber didn't get 5 messages"
'''