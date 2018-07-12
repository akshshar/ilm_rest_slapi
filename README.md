# ilm_rest_slapi

## Code still in progress. Currently a quick demonstration ##

Build the docker image with the REST server and the c++ gRPC client embedded inside:



```
$ git clone https://github.com/akshshar/ilm_rest_slapi
$ cd ilm_rest_slapi/docker

$ docker build --squash -t slapi_rest \
    --build-arg http_proxy_arg="http://proxy-server:port" \ 
    --build-arg https_proxy_arg="https://proxy-server:port" \
    --build-arg hostname="rtr1" --build-arg outgoing_ip="11.11.11.23" .
```



The setup used for this purpose looks something like this:

![topology](/images/topology_ncs5500_ilm_rest.png)


Deploy the built image on rtr1 (Follow instructions here:  
<https://xrdocs.io/application-hosting/tutorials/2017-02-26-running-docker-containers-on-ios-xr-6-1-2/>)

The docker run command will look something like this:  


```
docker run -itd --name slapi_rest --hostname=rtr1 -v /var/run/netns:/var/run/netns -v /misc/app_host:/root/mounted --cap-add=SYS_ADMIN --cap-add=NET_ADMIN 11.11.11.2:5000/slapi_rest bash

```

Typically, the entrypoint command will be used to automatically start the service. For demo purposes, I exec and start the service.


```

[rtr1:~]$ 
[rtr1:~]$ docker exec -it slapi_rest bash
root@rtr1:/# 
root@rtr1:/# 
root@rtr1:/# 
root@rtr1:/# ip netns exec global-vrf bash
root@rtr1:/# 
root@rtr1:/# 
root@rtr1:/# /root/mounted/run.sh 127.0.0.1 57777
WARNING: Logging before InitGoogleLogging() is written to STDERR
I0711 04:18:22.335196   483 main.cpp:86] Connecting to IOS-XR gRPC server at 127.0.0.1:57777
I0711 04:18:22.382359   483 main.cpp:131] Press control-c to quit
Modern C++ Microservice now listening for requests at: http://11.11.11.23:6502/



```



The micro-service will depend on /etc/hosts file to figure out the IP to bind to - in this case the Management IP of rtr1

Now simply leverage <http://11.11.11.23:6502/route/add> to push the route into RIB and create ILM entry in the label switch database. Use <http://11.11.11.23:6502/route/delete> to delete the route.

More instructions coming soon. See gif below for a quick snapshot of the behavior:


![operation_gif](/images/ilm_rest_manipulation.gif)
