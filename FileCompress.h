#pragma once
#include <assert.h>
#include <iostream>
#include <string>
#include <fstream>
#include "HuffmanTree.h"

using namespace std;
//定义一个更加长的类型，用于统计次数
typedef long long longType;

struct CharInfo
{
	char ch_;  //字符
	longType count_; //次数
	string code_; //编码	
	//支持CharInfo的加法
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret.count_ = count_ + info.count_;
		return ret;
	}
	//支持CharInfo的比较
	bool operator>(const CharInfo& info) const
	{
		return  count_ > info.count_;
	}

	bool operator!=(const CharInfo& info)
	{
		return count_ != info.count_;
	}
};

class FileCompress
{
	typedef HuffmanTreeNode<CharInfo> Node;
public:
	~FileCompress()
	{
		printf("析构\n");
	}
	//初始化
	FileCompress()
	{
		for (size_t i = 0; i < 256; i++)
		{
			hashInfos_[i].ch_ = i; //每个info对象对应的字符
			hashInfos_[i].count_ = 0;//每个字符对应出现的次数
		}
	}


		//压缩
	void Compress(const char* file)
	{
		//1.统计文件中字符出现的次数
		ifstream ifs(file);//把文件给ifs 不用自己关闭，析构会关闭 会自动抛异常
		char ch;
		while (ifs.get(ch))
		{
			++hashInfos_[ch].count_;//ch出现的次数增加
		}
		CharInfo invalid;
		invalid.count_ = 0;
		//2.生成huffman tree
		HuffmanTree<CharInfo> tree(hashInfos_,256,invalid);
		//3.生成huffman code 哈夫曼编码
		GenerateHuffmanCode2(tree.GetRoot());//轻耦合
		//4.压缩
		string compressfile = file;
		compressfile += ".huffman";
		ofstream ofs(compressfile.c_str());
		//5.压缩前写入字符次数，用来方便解压缩重建huffman树
		for (size_t i = 0; i < 256; i++)
		{
			if (hashInfos_[i].count_>0)
			{
				ofs.write((char*)&hashInfos_[i], sizeof(CharInfo));
			}
		}
		//最后写入一个来表示结尾
		CharInfo end;
		end.count_ = 0;
		end.ch_ = 'a';
		ofs.write((char*)&end, sizeof(CharInfo));
		//写入树的方法
		//1.单独写一个配置文件
		//2.先写入一个大小

		int pos = 0;
		char value = 0;
		ifs.clear();//清除一下状态
		ifs.seekg(0);//文件指针设置到0的位置，重新开始读
		while (ifs.get(ch))//读取字符到ch中并返回状态表示是否到文件末尾
		{
			//得到对应字符的编码
			string& code = hashInfos_[ch].code_;
			//44607
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '0')
					value &=~(1 << pos++);//编码的从右到左写入
				else if (code[i] == '1')
				{
					value |= (1 << pos++);
				}
				else
					assert(false);
				if (pos == 8)//说明已经写满一比特位，可以先写入一个比特位
				{
					ofs.write(&value,sizeof(value));//二进制写入
					value = 0;
					pos = 0;
				}
			}
		}
		//说明还有没写入的，写入最后一个比特
		if (value > 0)
		{
			ofs << value;
		}
	}

	//方法2 
	void GenerateHuffmanCode2(Node* root)
	{
		if (root == NULL)
			return;

		if (root->left_ != NULL)
		{
			//当前节点的左子树的code=当前节点的code+0
			root->left_->w_.code_ = root->w_.code_ + '0';
			GenerateHuffmanCode2(root->left_);
		}

		if (root->left_ != NULL)
		{
			//当前节点的右子树的code=当前节点的code+1
			root->right_->w_.code_ = root->w_.code_ + '1';
			GenerateHuffmanCode2(root->right_);
		}

		//把树中已经生成的编码转移到hashInfo中
		if (root->left_ == NULL&&root->left_ == NULL)
		{
			hashInfos_[root->w_.ch_].code_=root->w_.code_;
		}
		
	}

	//三叉树的方法生成编码
	void GenerateHuffmanCode1(Node* root)
	{
		if (root == NULL)
			return;

		GenerateHuffmanCode1(root->left_);
		GenerateHuffmanCode1(root->right_);

		//当到达一个叶子节点的时候，就向上走
		if (root->left_ == NULL
			&& root->right_ == NULL)
		{
			string& code = hashInfos_[root->w_.ch_].code_;//这里使用引用避免一次深拷贝
			Node* cur = root;//当前节点
			Node* parent = cur->parent_;//当前节点分父结点
			while (parent)//到了根节点就会停
			{
				//如果当前节点是父结点的左子树，就纪录0，反之为1
				if (cur == parent->left_)
					code += '0';
				else
					code += '1';

				//向上走
				cur = parent;
				parent = parent->parent_;
			}

			reverse(code.begin(),code.end());//字符串
		}
	}

	//解压缩
	void UnCompress(const char* file)
	{
		//1.处理文件的名字
		string uncompressfile = file;
		ifstream ifs(file);//读
		int pos = uncompressfile.rfind('.');
		uncompressfile.erase(pos);
//#ifdef _DEBUG //如果是调试模式就生成这个东西
		uncompressfile += ".unhuffman";
//#endif
		ofstream ofs(uncompressfile.c_str());//创建一个
		//2.读取建树信息
		
		while (1)
		{
			CharInfo info;
			ifs.read((char*)&info, sizeof(CharInfo));
			if (info.count_ > 0)
			{
				hashInfos_[info.ch_] = info;
			}
			else
			{
				break;
			}
		} 
		
		//3.重新建立树
		CharInfo invalid;
		invalid.count_ = 0;
		HuffmanTree<CharInfo> tree(hashInfos_, 256, invalid);
		

		Node* root = tree.GetRoot();
		longType file_count = root->w_.count_;//根节点的权值其实就是总字符数量
		printf("%d\n", file_count);
		Node* cur = root;
		char ch;
		while (ifs.get(ch))//从需要解压的文件中读取数据
		{
			
			pos = 0;
			while (pos < 8)
			{
				//bit位为1，向右走
				if (0x01 & ch >> pos)
				{
					cur = cur->right_;
				}
				else
				{
					cur = cur->left_;
				}
				//通过编码找到字符了
				if (cur->left_ == NULL
					&&cur->right_ == NULL)
				{
					ofs << cur->w_.ch_;//写入编码对应的字符，进行还原
					cur = root;//再次寻找编码
					printf("%d\n", file_count);
					if (--file_count <=0 )
					{
						//写入字符数等于原先最大字符数,就停止写入
						break;
					}
				}
				pos++;//总共读取8个位
			}
		}
	}

private:
	CharInfo hashInfos_[256];
};


void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("huffman.txt");
}

void TestFileUnCompress()
{
	FileCompress fc;
	fc.UnCompress("huffman.txt.huffman");
}