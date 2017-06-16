# DiskExplorer

## 编译环境
* Qt 5.5.1 MSVC2013 64bit（Windows平台）或者Qt 5.6.1 GCC 64bit（Linux平台）
* OpenSSL
    * Win64 [下载](https://slproweb.com/products/Win32OpenSSL.html) （我下载的是1.0.1u light版）
    * Linux 未知（Ubuntu默认就可以了）

这是一个连接到Yandex网盘API的工具，可以按照片墙的形式显示出图片，程序初次启动会在当前目录下生成config.ini文件，通过File->Authorize菜单来登录Yandex网站，获取应用授权。
> Yandex的网盘速度不快

## 使用方法
1. 在Yandex上注册自己的账号。
1. File->Authorize 登录到Yandex的授权网站，输入用户名和密码，一路确认，窗口自动消失说明已经成功获取token。
1. File->Connect 连接到自己的网盘。
1. 下拉列表中的是文件夹，只能显示顶层的。
1. 切换文件夹，下方列表显示图片名称和预览图，双击显示到右边，右侧的布局是流式布局。
1. GetAll会将该文件夹下的所有图片显示出来，小心你的内存。
1. 在图片上或者列表中点击鼠标右键有移动和删除图片的功能。

 ![界面预览](http://onmdsye1w.bkt.clouddn.com/preview.png)


## 可能的问题
* Linux下编译提示缺少GL/gl.h，应该是需要安装mesa-dev相关的包，里面有OPENGL的内容。
