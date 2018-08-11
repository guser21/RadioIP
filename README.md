# Internet Radio over IP.

It is an internet radio over multicast UDP. There are three parts for this project, the server, the client and the telnet UI for the client.

## Server

The server is quite simple it streams on a multicast udp group on which clients later register to and receive the stream. You should specify the multicast group address with flag
“-a”. Just make sure that they are all different among the stations you run.
You can also specify a name for your radio station.

`sox -S numb.mp3 -r 44100 -b 16 -e signed-integer -c 2 -t raw - | pv -q -L $((44100*4)) | ./Server -a 239.10.11.12 -n "Radio Yerevan"`

| Option | Description |
| --- | --- |
|  -h|Available params|
|  -a arg| set multicast ip address |
|  -P arg (=21501) |set data data_port |
|  -C arg (=31501)|set control data_port |
|  -p arg (=1024)|set packet size |
|  -f arg (=131072)|set fifo buffer size |
|  -R arg (=250)|retransmission period in milliseconds |
|  -n arg (=Unnamed Station) |  station name |

## Client

The client supports multiple radio stations streaming at the same time. You can easily connect to the client with telnet and change the station with the ascii ui :D. The client also supports multiple ui connections simultaneously. To set the UI port for the telnet to connect use the flag “-U” .

To turn the stream into audio you also need to pipe it into play program or you can just run it like this
`./Client -U 1111 | play -t raw -c 2 -r 44100 -b 16 -e signed-integer --buffer 32768 -`

| Option | Description |
| --- | --- |
|  -h |                       Available params|
| -d arg (=255.255.255.255) |set discover address|
| -C arg (=31501)           |set control data_port|
| -U arg (=11501)           |set ui data_port|
| -B arg (=524288)          |set buffer size|
| -n arg                    |set desired station name|
| -R arg (=250)             |lookup request period in seconds|



## UI

The telnet ui is pretty simple one, you just connect with it through the right port and it all there is to it.
With the up and down arrows you can switch between stations.


E.g.
`telnet localhost 11111`

![Radio UI](images/RadioUI.png?raw=true)



You can run it between multiple computers if your router is configured not to drop multicast packets. The protocol also supports recovery from packet loss so with the right buffer and packet sizes you should get pretty decent sound for all your receivers.
