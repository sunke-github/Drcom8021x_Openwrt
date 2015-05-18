# Drcom8021x_Openwrt

##声明:
Drcom8021x 软件是根据 EasyDrcom软件重构而来,该源码地址:https://github.com/coverxit/EasyDrcom .
本软件遵循GPL v3.0,使用本软件源码请注明原作者,来源和出处.

##功能概述:

该软件可以实现 drcom的 802.1x 认证,适用于部分地区和学校.可在OpenWrt系统mips_AR7xxx_AR9xxx  CPU架构运行.运行同时请将配置文件放于相同目录.<br>省略原EasyDrcom 认证模式和注销功能.设置请修改可执行文件下的 Drcom8021x.conf文件. 


##编译软件:
(1)软件在编译调试过程中如果无法找到配置文件,请修改 def.h 中的路径为绝对路径.<br>
(2)编译时请配置你的交叉工具链.<br>
(3)使用Linux 下的CDT 创建和编译该工程,直接导入该工程即可编译.编译时请在<br> 
  工程->Properities->C/C++ Build->Settings->GCC C++ Compiler 中添加 -std=c++11<br>
  工程->Properities->C/C++ Build->Settings->GCC C++ Linker  Miscellaneous中 添加 -lpcap -L/PATH(你的openwrt libpcap 库的路径)
  


##免责声明:
该软件是做交流学习使用,请勿用作非法用途. 使用本软件带来的法律问题由使用者付全责与软件作者无关.