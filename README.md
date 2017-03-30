# DiskExplorer

## 编译环境
* Qt 5.5.1 MSVC2013 64bit
* Qt Creator 4.2.1
* OpenSSL-Win64

这是一个连接到Yandex网盘API的工具，可以按照片墙的形式显示出图片，程序初次启动会在当前目录下生成config.ini文件，需要把你自己的Token填写进去，也可以通过File->Token菜单来修改。
> Yandex的网盘速度不快

## 使用方法
1. 想办法获取自己的Token。
1. File->Token 设置自己的Token，Token的格式应该是“OAuth AAAAAAAAAAAAAAAAAAAAAAAAAAAA”这样的格式，注意中间有一个空格。
1. File->Connect 连接到自己的网盘。
1. 下拉列表中的是文件夹，只能显示顶层的。
1. 切换文件夹，下方列表显示图片名称和预览图，双击显示到右边，右侧的布局是流式布局。
1. GetAll会将该文件夹下的所有图片显示出来，小心你的内存。
1. 在图片上或者列表中点击鼠标右键有移动和删除图片的功能。

 ![界面预览](http://onmdsye1w.bkt.clouddn.com/preview.png)
