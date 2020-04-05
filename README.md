# HuaWei_CodeCraft_2020
---
## 题目分析
- 要求：通过资金流水，识别循环转账，在保证监测结果准确的情况下，尽可能减少用时。
- 输入：本端账号ID，对端账号ID，转账金额。(读取文件要使用只读的方式打开)
	
  **数据特征**：
    
        ID和转账金额为一个32位的正整数
    
		转账记录最多28W条
    
		每个账号平均转账记录数小于10
    
		账号A给账号B最多转账一次
 ---
	输入路径：
		/data/test_data.txt
- 输出：
	
  
  第一行：满足限制条件的循环转账个数（循环转账个数小于300W）
	
  第二行：输出所有的循环路径（ID最小的第一个输出，总体按照循环转账路径长度升序排列；同一长度的路径长度下循环转账账号ID序列，按照字典序升序）
限制条件：
	
  循环转账的路径长度最小为3，最大为7
	
  输出路径：/projects/student/result.txt
  
- 编译：g++ -O3 main.cpp -o test -lpthread
- 执行：./test
---
## 思路：
1、通过邻接表建图

2、通过拓扑排序遍历整个图

3、通过七层套娃检测环，并进行剪枝，最后DFS
---
## 参考：
[1][图的详细介绍]( https://segmentfault.com/a/1190000010794621)

[2][邻接矩阵与邻接表的比较]( https://blog.csdn.net/qq_29134495/article/details/51376580)

[3][C++文件的输入和输出]( https://blog.csdn.net/kingstar158/article/details/6859379)

[4][图的实现](https://blog.csdn.net/mind_v/article/details/75941252)

[5][基于邻接表建图](https://blog.csdn.net/u012717411/article/details/46386503)
