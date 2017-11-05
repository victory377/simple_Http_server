Usage command
===
### pthread
* gcc -pthread -o http_server simple_webserver.c
* ./http_server

* Open your browser to http://current_pc_IP:10000
    *current_pc_IP: using ifconfig command to check

### libevent
* wget https://s3.amazonaws.com/github/downloads/libevent/libevent/libevent-2.0.21-stable.tar.gz
* tar zxvf libevent-2.0.21-stable.tar.gz
* cd libevent-2.0.21-stable/
* ./configure
* make 
* sudo make install

* compile your code 
    * gcc -o xxx xxx.c -levent

