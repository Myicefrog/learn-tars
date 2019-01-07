# 腾讯开源框架Tars源码学习笔记---重写Tars rpc

# 前言

这个项目试图从最基本的epoll模型开始，一步步向官方Tars RPC靠拢，尽量记录下关键的结点。之所以选择这样做，是亲手实现一遍代码比单纯调试代码印象更为深刻。

选择将这个过程分享出来，是感觉这份笔记对于想短时间了解Tars源码主要逻辑脉络的人来说，会有一些帮助

使用的系统环境：

* 操作系统版本：Ubuntu 18.04.1 LTS (GNU/Linux 4.15.0-42-generic x86_64)

* GCC版本：gcc version 7.3.0 (Ubuntu 7.3.0-27ubuntu1~18.04) 

怎样看这份笔记？

* [代码提交记录](https://github.com/Myicefrog/learn-tars/commits/master)可以很好的追踪代码变化。并且下面笔记中1.1、 1.2的内容与代码提交记录中的1.1、1.2是完全匹配的，可以对照理解


> 这只是实验性质的代码，因为更多关注主体逻辑，很多细节可能忽略掉了

>  代码编译只需要运行./run.sh，大多数人都可以轻松编译调试


# 正文

[1 建立Tars RPC Server 异步模型框架](https://github.com/Myicefrog/learn-tars/wiki/1-%E5%BB%BA%E7%AB%8BTars-RPC-Server-%E5%BC%82%E6%AD%A5%E6%A8%A1%E5%9E%8B%E6%A1%86%E6%9E%B6)

[2 建立RPC client异步框架](https://github.com/Myicefrog/learn-tars/wiki/2-%E5%BB%BA%E7%AB%8BRPC-client%E5%BC%82%E6%AD%A5%E6%A1%86%E6%9E%B6)

[3 RPC Server加入协程](https://github.com/Myicefrog/learn-tars/wiki/3-RPC-Server%E5%8A%A0%E5%85%A5%E5%8D%8F%E7%A8%8B)

[4 进一步完善RPC Client（同步）](https://github.com/Myicefrog/learn-tars/wiki/4-%E8%BF%9B%E4%B8%80%E6%AD%A5%E5%AE%8C%E5%96%84RPC-Client)

[5 进一步完善RPC Client（异步）](https://github.com/Myicefrog/learn-tars/wiki/5-%E8%BF%9B%E4%B8%80%E6%AD%A5%E5%AE%8C%E5%96%84RPC-Client%EF%BC%88%E5%BC%82%E6%AD%A5%EF%BC%89)

[6 进一步完善RPC Server](https://github.com/Myicefrog/learn-tars/wiki/6-%E8%BF%9B%E4%B8%80%E6%AD%A5%E5%AE%8C%E5%96%84RPC-Server)

[7 Framework篇：NodeServer](https://github.com/Myicefrog/learn-tars/wiki/7-Framework%E7%AF%87%EF%BC%9ANodeServer)

[8 Framework篇：AdminRegistryServer](https://github.com/Myicefrog/learn-tars/wiki/8-Framework%E7%AF%87%EF%BC%9AAdminRegistryServer)

[更多](https://github.com/Myicefrog/learn-tars/wiki)


作者：路小饭  

微信号：luguangfamily 

邮箱490925498@qq.com

![微信](https://github.com/Myicefrog/tars-img/blob/master/img/mmqrcode1544579415883.png)

参考资料：https://github.com/TarsCloud/Tars

