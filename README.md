# APP_nav_robot
## 介绍
### CLEAN_POOL
这是一个可以用c++写的、可以与ROS通信的手机APP！  
功能模块有连接主机、视频回传、摇杆操控、自主导航以及船体健康检测。  
### POOL_CLEANMan
这是一个电脑上位机！  
功能模块有电流检测、温度湿度检测、物联网以及AI水质评估。
## 环境
+ ubutnu 20.04
+ Ros noetic
+ Qt 6.4.1
## 运行
### CLEAN_POOL 
```
roslaunch rosbridge_server rosbridge_websocket.launch
```
### POOL_CLEANMan  
修改为可连接的ip地址即可
## 个人教程
Qt配置安卓：https://www.yuque.com/g/ruolinyuzhenghao/xlhh7p/drssvthbvdx5uddp/collaborator/join?token=x2fD53UGcXOSNnPz&source=doc_collaborator#
## 引用声明
本项目控件风格引用地址为[qt-material-widgets](https://gitcode.com/laserpants/qt-material-widgets.git)
