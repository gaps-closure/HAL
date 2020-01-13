This directory will include the Hardware Abstraction Layer (HAL) Service which
abstracts the TA1 API for use by the cross-domain programs. The HAL Service
runs as a daemon (typically started by a systemd script at boot time).  

It allows application to send and receive cross-domain data and tags over
ZeroMQ. It opens and manages cross domain guard devices (real or emualted). It
mediates the exchange between the application and the guard devices, handling
the encoding/decoding and multiplexing/demultiplexing as applicable.

Make sure to install pre-requisites.
```
sudo apt install -y libzmq3-dev
sudo apt install -y libconfig-dev
```

```
# Example basic testing (run each command in separate window in hal directory):
# Start HAL, sender, and receiver; type in sender window some text lines with an EOF at the end (or ^D)
./hal sample.cfg
zc/zc -v sub ipc://halpub
zc/zc -v -dEOF pub ipc://halsub

# To write PDU from application enter a string preceeded by a haljson tag
# in the zc pub window. For example:
tag-app1-m1-d1 ABCDEFGHIJK
tag-app4-m1-d1 ABCDEFGHIJK
```

```
# If devices are enabled in sample.cfg, then corresponding devices must be configured
netcat -4 -l -k 127.0.0.1 1234
sudo socat -d -d -lf socat_log.txt pty,link=/dev/vcom1,raw,ignoreeof,unlink-close=0,echo=0 tcp:127.0.0.1:1234,ignoreeof &
sudo chmod 777 /dev/vcom1
echo "hello you" > /dev/vcom1
cat /dev/vcom1
```

```
# If using bkend model, then pass netcat (binary): a) output through od
# and b) input through xxd 
stdbuf -oL xxd -r -p | netcat -4 -l -k 127.0.0.1 1234 2>&1 | od -t x1
# XXX Need to remove buffering on output from above command...

# To write PDU from bkend enter hex input (into netcast, via xxd). For example:
00 00 00 04 00 00 00 02 00 00 00 01 00 00 00 03 00 00 00 0c 61 62 63 64 65 66 67 68 69 6a 6b 0a
```

**This directory needs to be moved out of emulator into its own repository, as it
is a CLOSURE component that applies to both real and emulated systems.**
