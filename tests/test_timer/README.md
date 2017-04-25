**Contents**

The test_timer application demonstrates the time-sync capabilities of the RIAPS Framework.
The application implements two components: Generator, Receiver. The generator produces a 60Hz sin wave.
The samples are used to drive a PWM output on the node (PIN 9.14).
The samples with timestamps are published on the generator component's port.
The Receiver component is running on another node and subscribes to the samples.
From the received values and timestamps the Receiver estimates the timepoint of the next zero crossing and schedules an event at this timepoint.
When the event is triggered, the receiver toggles GPIO_PORT XXXXX.

If the time-sync is properly configured on the node we expect that the port toggleing happens exactly (todo: +- microsec) at the zero crossing. 

**Deploy**

- Install the latest RIAPS Externals (armhf) release to the node: https://github.com/RIAPS/riaps-externals
- Install the latest RIAPS Core release (armhf) to the node: https://github.com/RIAPS/riaps-core
- Build the latest test RIAPS Applications: https://github.com/RIAPS/riaps-core

```
git clone https://github.com/RIAPS/riaps-core
cd riaps-core
mkdir build
cd build
cmake ../ -Darch=armhf
```
- Copy the libgenerator.so, libreceiver.so and TimerTest.json to the node.

```
scp ./libgenerator.so riaps@<ipaddress>:/home/riaps/riaps_apps/testtimer
scp ./libreceiver.so riaps@<ipaddress>:/home/riaps/riaps_apps/testtimer
scp ./TestTimer.json riaps@<ipaddress>:/home/riaps/riaps_apps/testtimer
```

- Start the RIAPS discovery service in each node.
```
rdiscoveryd
```
- Start the Generator and the Receiver with `sudo`.
```
sudo /opt/riaps/armhf/bin/start_actor TimerTest.json RActor
sudo /opt/riaps/armhf/bin/start_actor TimerTest.json GActor
```

[Expected results](./scope2.png)

We used the bbb-19bb as master (generator), and the bbb-918 as slave (receiver);