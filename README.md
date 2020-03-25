# websocket-gateway
一，this is an websocket gateway code with c++ 
二，基于c++封装的websocket库项目，可用于网络游戏，以及其h5应用的服务器网关模块，其他模块的也可以基于此去做业务开发。
三，开发之需要在centos或者其他linux主机上安装好redis,mysql，grpc,protobuf环境，然后可以直接执行makefile编译出执行文件。
四，网络socket层采用epoll/ET模式，可配置线程池和连接池。建立连接，收到数据，断开连接都通过回调函数返回给应用层。
五，服务器框架开发者可以自定义，以及自行处理粘包。
六，已经有编译好的netbase.a网络库可直接使用。
