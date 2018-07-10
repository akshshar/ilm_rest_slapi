FROM ubuntu:16.04 

ARG https_proxy_arg=$https_proxy_arg
ARG http_proxy_arg=$http_proxy_arg
ARG hostname=$hostname
ARG outgoing_ip=$ip

RUN export https_proxy=$https_proxy_arg && apt-get update && apt-get install -y iproute2 git
RUN export https_proxy=$https_proxy_arg && git clone https://github.com/Cisco-Service-Layer/service-layer-objmodel.git /root/service-layer-objmodel && cd /root/service-layer-objmodel/grpc/cpp  && ./build_libiosxrsl.sh

RUN export https_proxy=$https_proxy_arg && git clone https://github.com/akshshar/openr-xr.git /root/openr && cd /root/openr/build && /bin/bash ./openr_dependencies_short.sh

RUN rm -r /root/service-layer-objmodel /root/openr

COPY code /root/code

RUN export https_proxy=$https_proxy_arg && git clone https://github.com/ivanmejiarocha/micro-service.git /root/micro-service/ && cd /root/micro-service/libs && sed -i 's/g++-4.9/g++-5/g' ./build_dependencies.sh && cd /root/micro-service/libs && ./build_dependencies.sh && cp -r /root/code/* /root/micro-service/ && cd /root/micro-service/ && make

RUN apt-get install -y iputils-ping nettools gdb
RUN echo "127.0.0.1 localhost" > /etc/hosts && echo "$outgoing_ip $hostname" > /etc/hosts
