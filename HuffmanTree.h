#pragma once
#include <queue>

using namespace std;

//哈夫曼树的节点
template<class W>
struct  HuffmanTreeNode
{
	HuffmanTreeNode<W>* left_;
	HuffmanTreeNode<W>* right_;
	HuffmanTreeNode<W>* parent_;
	W w_;//节点的权值 weight
	HuffmanTreeNode(const W& w)
		:w_(w)
		, left_(NULL)
		, right_(NULL)
		, parent_(NULL)
	{
	}
};

//哈夫曼树
//不关注增加删除查找
//构造函数先把一个数组构建成哈夫曼树
template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:root(NULL)
	{}

	//比较的仿函数
	struct NodeCmp
	{
		//重载括号
		bool operator()(const Node* l, const Node* r) 
		{
				return l->w_ > r->w_; //如果父大于子就要交换,保持父小，建立大堆
		}
	};

	//不能每个值都构建，非法值就没必要构建,这里非法值其实就是count等于0
	//传入的是一个数组以及数组的长度
	HuffmanTree(W* w,size_t n,const W& invalid)
	{
		//优先级队列放的是节点的地址
		//第一个参数是优先级队列的每个元素的类型
		//第二个参数是使用的适配器
		//第三个是比较的方式，这里我们使用仿函数
		priority_queue<Node*,vector<Node*>,NodeCmp> minheap;//小堆
		for (size_t i = 0; i < n; ++i)
		{
			//数组插入优先级队列
			if (w[i]!=invalid)//不插入count=0的
			minheap.push(new Node(w[i]));
		}
		
		//贪心算法过程
		while (minheap.size()>1)
		{
			//拿出第一个最小值
			Node* left = minheap.top();
			minheap.pop();
			//拿出第二个最小值
			Node* right = minheap.top();
			minheap.pop();
			//将两个最小值相加，并建立树形
			Node* parent = new Node(left->w_+right->w_);
			parent->left_ = left;
			left->parent_ = parent;
			parent->right_ = right;
			right->parent_ = parent;
			minheap.push(parent);
		}
		root_ = minheap.top();//最后一个节点就是根节点
	}

	~HuffmanTree()
	{
		Destroy(root_);
	}

	//递归删除
	void Destroy(Node* root)
	{
		if (root == NULL)
		{
			return;
		}
		Destroy(root->left_);
		Destroy(root->right_);
		delete root;
	}
	
	Node* GetRoot()
	{
		return root_;
	}

private:
	//防拷贝
	HuffmanTree(const HuffmanTree<W>& t);
	//防赋值
	HuffmanTree<W>& operator=(const HuffmanTree<W>& t);

protected:
	Node* root_;//根节点
};

