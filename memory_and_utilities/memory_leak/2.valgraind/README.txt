========================================================================================================
http://blog.chinaunix.net/uid-23629988-id-3033741.html
http://www.cnblogs.com/sunyubo/archive/2010/05/05/2282170.html
http://www.oschina.net/translate/valgrind-memcheck
========================================================================================================

Valgrind是一款用于内存调试,内存泄漏检测以及性能分析的软件开发工具。
Valgrind作为一个免费且优秀的工具包，平时大部分人可能都是使用valgrind检测内存问题，如内存泄露，越界等。
其实Valgrind的用途远不止于此，其实际上为一个工具包，除了检查内存问题以外，还有其它多项用途。

	valgrind --track-fds=yes --leak-check=full --undef-value-errors=yes --track-origins=yes /system/bin/updateservice
	
========================================================================================================
使用 Valgrind Memcheck
+++++++++++++++++++++++

memcheck工具的使用方式如下:

	valgrind --tool=memcheck ./a.out

	从上面的命令可以清楚的看到, 主要的命令是valgrind，而我们想使用的工具是通过'-tool'选项来指定的. 
	上面的‘a.out’指的是我们想使用memcheck运行的可执行文件.

该工具可以检测下列与内存相关的问题 :
	未释放内存的使用
	对释放后内存的读/写
	对已分配内存块尾部的读/写
	内存泄露
	不匹配的使用malloc/new/new[] 和 free/delete/delete[]
	重复释放内存

	注意: 上面列出的并不很全面，但却包含了能被该工具检测到的很多普遍的问题.	
========================================================================================================
使用Valgrind 
+++++++++++++

Valgrind的使用非常简单，valgrind命令的格式如下： 

	valgrind [valgrind-options] your-prog [your-prog options] 

一些常用的选项如下： 

	-h			--help				显示帮助信息。 
	--version						显示valgrind内核的版本，每个工具都有各自的版本。 
	-q			--quiet				安静地运行，只打印错误信息。 
	-v			--verbose			打印更详细的信息。 
	--tool= [default: memcheck]		最常用的选项。运行valgrind中名为toolname的工具。如果省略工具名，默认运行memcheck。 
	--db-attach= [default: no]		绑定到调试器上，便于调试错误。 

========================================================================================================
