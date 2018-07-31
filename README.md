# RadioIP

To start the transmission 


sox -S "BeeGeesStayinAlive.mp3" -r 44100 -b 16 -e signed-integer -c 2 -t raw - | pv -q -L $((44100*4)) | ./nadajnik -a 239.10.11.12 -n "Radio Yerevan"


To start the client


./odbiornik | play -t raw -c 2 -r 44100 -b 16 -e signed-integer --buffer 32768 -


To change the channel in client from ascii UI


telnet localhost 11506
