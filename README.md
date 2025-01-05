# dump内存--bypass360


![image](https://github.com/user-attachments/assets/8e81d5fa-5fa0-4b9b-80e2-c9439ef44c4a)



成功dump内存
![image](https://github.com/user-attachments/assets/328784fc-0549-4aeb-9f52-6198148e677f)




！！注意此方法不能dump lsass  只能dump一些普通应用如todesk向日葵。本来想写一个通过dump todesk内存读取密码来着后来发现规则太难写了，误报严重不如dump下来自己查来的实在，然后就有了第二个代码：读取内存前 10000000 字节（todesk的密码存在这个范围内）。然后下载下来手动分析吧。
使用读取exe内存获取todesk的内存
然后使用zip.exe进行压缩
拷出来
之后使用正则搜索
\b\d{9}\b
之后查看上下文就好
