#pragma once
#include <assert.h>
#include <iostream>
#include <string>
#include <fstream>
#include "HuffmanTree.h"

using namespace std;
//����һ�����ӳ������ͣ�����ͳ�ƴ���
typedef long long longType;

struct CharInfo
{
	char ch_;  //�ַ�
	longType count_; //����
	string code_; //����	
	//֧��CharInfo�ļӷ�
	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret.count_ = count_ + info.count_;
		return ret;
	}
	//֧��CharInfo�ıȽ�
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
		printf("����\n");
	}
	//��ʼ��
	FileCompress()
	{
		for (size_t i = 0; i < 256; i++)
		{
			hashInfos_[i].ch_ = i; //ÿ��info�����Ӧ���ַ�
			hashInfos_[i].count_ = 0;//ÿ���ַ���Ӧ���ֵĴ���
		}
	}


		//ѹ��
	void Compress(const char* file)
	{
		//1.ͳ���ļ����ַ����ֵĴ���
		ifstream ifs(file);//���ļ���ifs �����Լ��رգ�������ر� ���Զ����쳣
		char ch;
		while (ifs.get(ch))
		{
			++hashInfos_[ch].count_;//ch���ֵĴ�������
		}
		CharInfo invalid;
		invalid.count_ = 0;
		//2.����huffman tree
		HuffmanTree<CharInfo> tree(hashInfos_,256,invalid);
		//3.����huffman code ����������
		GenerateHuffmanCode2(tree.GetRoot());//�����
		//4.ѹ��
		string compressfile = file;
		compressfile += ".huffman";
		ofstream ofs(compressfile.c_str());
		//5.ѹ��ǰд���ַ����������������ѹ���ؽ�huffman��
		for (size_t i = 0; i < 256; i++)
		{
			if (hashInfos_[i].count_>0)
			{
				ofs.write((char*)&hashInfos_[i], sizeof(CharInfo));
			}
		}
		//���д��һ������ʾ��β
		CharInfo end;
		end.count_ = 0;
		end.ch_ = 'a';
		ofs.write((char*)&end, sizeof(CharInfo));
		//д�����ķ���
		//1.����дһ�������ļ�
		//2.��д��һ����С

		int pos = 0;
		char value = 0;
		ifs.clear();//���һ��״̬
		ifs.seekg(0);//�ļ�ָ�����õ�0��λ�ã����¿�ʼ��
		while (ifs.get(ch))//��ȡ�ַ���ch�в�����״̬��ʾ�Ƿ��ļ�ĩβ
		{
			//�õ���Ӧ�ַ��ı���
			string& code = hashInfos_[ch].code_;
			//44607
			for (size_t i = 0; i < code.size(); ++i)
			{
				if (code[i] == '0')
					value &=~(1 << pos++);//����Ĵ��ҵ���д��
				else if (code[i] == '1')
				{
					value |= (1 << pos++);
				}
				else
					assert(false);
				if (pos == 8)//˵���Ѿ�д��һ����λ��������д��һ������λ
				{
					ofs.write(&value,sizeof(value));//������д��
					value = 0;
					pos = 0;
				}
			}
		}
		//˵������ûд��ģ�д�����һ������
		if (value > 0)
		{
			ofs << value;
		}
	}

	//����2 
	void GenerateHuffmanCode2(Node* root)
	{
		if (root == NULL)
			return;

		if (root->left_ != NULL)
		{
			//��ǰ�ڵ����������code=��ǰ�ڵ��code+0
			root->left_->w_.code_ = root->w_.code_ + '0';
			GenerateHuffmanCode2(root->left_);
		}

		if (root->left_ != NULL)
		{
			//��ǰ�ڵ����������code=��ǰ�ڵ��code+1
			root->right_->w_.code_ = root->w_.code_ + '1';
			GenerateHuffmanCode2(root->right_);
		}

		//�������Ѿ����ɵı���ת�Ƶ�hashInfo��
		if (root->left_ == NULL&&root->left_ == NULL)
		{
			hashInfos_[root->w_.ch_].code_=root->w_.code_;
		}
		
	}

	//�������ķ������ɱ���
	void GenerateHuffmanCode1(Node* root)
	{
		if (root == NULL)
			return;

		GenerateHuffmanCode1(root->left_);
		GenerateHuffmanCode1(root->right_);

		//������һ��Ҷ�ӽڵ��ʱ�򣬾�������
		if (root->left_ == NULL
			&& root->right_ == NULL)
		{
			string& code = hashInfos_[root->w_.ch_].code_;//����ʹ�����ñ���һ�����
			Node* cur = root;//��ǰ�ڵ�
			Node* parent = cur->parent_;//��ǰ�ڵ�ָ����
			while (parent)//���˸��ڵ�ͻ�ͣ
			{
				//�����ǰ�ڵ��Ǹ��������������ͼ�¼0����֮Ϊ1
				if (cur == parent->left_)
					code += '0';
				else
					code += '1';

				//������
				cur = parent;
				parent = parent->parent_;
			}

			reverse(code.begin(),code.end());//�ַ���
		}
	}

	//��ѹ��
	void UnCompress(const char* file)
	{
		//1.�����ļ�������
		string uncompressfile = file;
		ifstream ifs(file);//��
		int pos = uncompressfile.rfind('.');
		uncompressfile.erase(pos);
//#ifdef _DEBUG //����ǵ���ģʽ�������������
		uncompressfile += ".unhuffman";
//#endif
		ofstream ofs(uncompressfile.c_str());//����һ��
		//2.��ȡ������Ϣ
		
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
		
		//3.���½�����
		CharInfo invalid;
		invalid.count_ = 0;
		HuffmanTree<CharInfo> tree(hashInfos_, 256, invalid);
		

		Node* root = tree.GetRoot();
		longType file_count = root->w_.count_;//���ڵ��Ȩֵ��ʵ�������ַ�����
		printf("%d\n", file_count);
		Node* cur = root;
		char ch;
		while (ifs.get(ch))//����Ҫ��ѹ���ļ��ж�ȡ����
		{
			
			pos = 0;
			while (pos < 8)
			{
				//bitλΪ1��������
				if (0x01 & ch >> pos)
				{
					cur = cur->right_;
				}
				else
				{
					cur = cur->left_;
				}
				//ͨ�������ҵ��ַ���
				if (cur->left_ == NULL
					&&cur->right_ == NULL)
				{
					ofs << cur->w_.ch_;//д������Ӧ���ַ������л�ԭ
					cur = root;//�ٴ�Ѱ�ұ���
					printf("%d\n", file_count);
					if (--file_count <=0 )
					{
						//д���ַ�������ԭ������ַ���,��ֹͣд��
						break;
					}
				}
				pos++;//�ܹ���ȡ8��λ
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