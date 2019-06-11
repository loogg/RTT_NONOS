# RTT_NONOS

RTT_NONOS基于RT-Thread 4.0.1，是RT-Thread的裸机版本。使用 **状态机编程** 。

目前RT_NONOS支持的组件有 **shell** 、 **AT Client** 、 **Ymode** 、 **SFUD** 等组件。

目前支持的软件包测试过的有 **FAL** 、 **EASYFLASH**。

[Project](./Project) 文件夹下的是目前所实现的工程。

工程中所实现的功能如下：

开机首先倒计时，如果没有输入，则执行bootcmd的指令，如果有输入则转入shell接口。

![bootcmd](./figures/1.gif)

使用Ymode下载如下。

![ymode](./figures/2.gif)

    