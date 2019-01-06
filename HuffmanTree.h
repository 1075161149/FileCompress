#pragma once
#include <queue>

using namespace std;

//���������Ľڵ�
template<class W>
struct  HuffmanTreeNode
{
	HuffmanTreeNode<W>* left_;
	HuffmanTreeNode<W>* right_;
	HuffmanTreeNode<W>* parent_;
	W w_;//�ڵ��Ȩֵ weight
	HuffmanTreeNode(const W& w)
		:w_(w)
		, left_(NULL)
		, right_(NULL)
		, parent_(NULL)
	{
	}
};

//��������
//����ע����ɾ������
//���캯���Ȱ�һ�����鹹���ɹ�������
template<class W>
class HuffmanTree
{
	typedef HuffmanTreeNode<W> Node;
public:
	HuffmanTree()
		:root(NULL)
	{}

	//�Ƚϵķº���
	struct NodeCmp
	{
		//��������
		bool operator()(const Node* l, const Node* r) 
		{
				return l->w_ > r->w_; //����������Ӿ�Ҫ����,���ָ�С���������
		}
	};

	//����ÿ��ֵ���������Ƿ�ֵ��û��Ҫ����,����Ƿ�ֵ��ʵ����count����0
	//�������һ�������Լ�����ĳ���
	HuffmanTree(W* w,size_t n,const W& invalid)
	{
		//���ȼ����зŵ��ǽڵ�ĵ�ַ
		//��һ�����������ȼ����е�ÿ��Ԫ�ص�����
		//�ڶ���������ʹ�õ�������
		//�������ǱȽϵķ�ʽ����������ʹ�÷º���
		priority_queue<Node*,vector<Node*>,NodeCmp> minheap;//С��
		for (size_t i = 0; i < n; ++i)
		{
			//����������ȼ�����
			if (w[i]!=invalid)//������count=0��
			minheap.push(new Node(w[i]));
		}
		
		//̰���㷨����
		while (minheap.size()>1)
		{
			//�ó���һ����Сֵ
			Node* left = minheap.top();
			minheap.pop();
			//�ó��ڶ�����Сֵ
			Node* right = minheap.top();
			minheap.pop();
			//��������Сֵ��ӣ�����������
			Node* parent = new Node(left->w_+right->w_);
			parent->left_ = left;
			left->parent_ = parent;
			parent->right_ = right;
			right->parent_ = parent;
			minheap.push(parent);
		}
		root_ = minheap.top();//���һ���ڵ���Ǹ��ڵ�
	}

	~HuffmanTree()
	{
		Destroy(root_);
	}

	//�ݹ�ɾ��
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
	//������
	HuffmanTree(const HuffmanTree<W>& t);
	//����ֵ
	HuffmanTree<W>& operator=(const HuffmanTree<W>& t);

protected:
	Node* root_;//���ڵ�
};

