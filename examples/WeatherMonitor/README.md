## Synopsis

This is the C++ version of pycom's WeatherMonitor test. The wmonitor.riaps and wmonitorDT.dt files are created using the riaps-dsml tool. The DSML then generated all the component wrapper classes in /test_WeatherMonitor/WeatherMonitor folder. DSML generated empty pub/sub/timer handler functions in the wrapper. The app developer can put what they desire in those functions. Please see the handler function definitions in /WeatherMonitor/src/TempMonitor.cc and /WeatherMonitor/src/TempSensor.cc. 

TempMonitor.cc contains a subscriber handler function. TempSensor contains a publish handler function and a timer handler function.


## Manually Running the Test

Building the test will create 2 .so (libtempmonitor.so, libtempsensor.so). They need to be copied to bbb's /home/riaps/riaps_app/test_WeatherMonitor/WeatherMonitor folder. Then execute the below command on 3 terminal windows.

May need to do this on the bbb nodes: export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/riaps/armhf/lib/

Terminal 1:
rdiscoveryd

Terminal 2:
start_actor WeatherMonitor.json WeatherReceiver

Terminal_3:
start_actor WeatherMonitor.json WeatherIndicator
