#include <iostream> 
#include <vector>
using namespace std;

struct node
{
	string characters;
	unsigned int frequency;
	string code;
	node* leftChild;
	node* rightChild;
};

vector<node> nodeArray; 

void getHuffmanCode();
node getHuffmanTree();
node extractMin();
void depthFirstSearch(node* tempRoot, string s); 


void getHuffmanCode()
{
	int size;
	unsigned int tempInt;
	char alphabet;

	// 데이터에 사용되는 문자의 종류 갯수 입력
	cout << endl;
	cout << "\t" << "Huffman Tree : ";
	cin >> size;

	cout << endl << endl;
	cout << "\t" << "symbol freq" << endl;
	cout << "\t" << "----------------------" << endl; 

	// 각 문자별 빈도 수 노드 생성
	for (int i = 0; i < size; i++)
	{
		cout << "\t";
		node tempNode;
		cin >> alphabet;
		cin >> tempInt;

		tempNode.characters = alphabet;
		tempNode.frequency = tempInt;
		tempNode.leftChild = NULL;
		tempNode.rightChild = NULL;
		nodeArray.push_back(tempNode);
	}

	//Huffman Tree 생성 
	node root = getHuffmanTree();

	cout << endl << endl;
	cout << "\t" << "symbol" << "\t" << "Code" << endl;
	cout << "\t" << "----------------------" << endl;

	//Huffman Coding Table 생성
	depthFirstSearch(&root, "");
}

node getHuffmanTree()
{
	while (!nodeArray.empty())
	{
		node* tempNode = new node;
		node* tempNode1 = new node;
		node* tempNode2 = new node;
		*tempNode1 = extractMin();
		*tempNode2 = extractMin();

		tempNode->leftChild = tempNode1;
		tempNode->rightChild = tempNode2;
		tempNode->frequency = tempNode1->frequency + tempNode2->frequency;
		nodeArray.push_back(*tempNode);

		//Root Node만 남았으므로 Huffman Tree 생성
		if (nodeArray.size() == 1)
			break;
	}

	return nodeArray[0];
}

node extractMin()
{
	unsigned int min = UINT_MAX;
	vector<node>::iterator iter, position;
	for (iter = nodeArray.begin(); iter != nodeArray.end(); iter++)
	{
		if (min > (*iter).frequency)
		{
			position = iter;
			min = (*iter).frequency;
		}
	}

	node tempNode = (*position);
	nodeArray.erase(position);

	return tempNode;
} 

void depthFirstSearch(node* tempRoot, string s)
{
	node* root1 = tempRoot;

	root1->code = s; 
	if (root1 == NULL)
	{

	} 
	else if (root1->leftChild == NULL && root1->rightChild == NULL)
	{
		cout << "\t" << root1->characters << "\t" << root1->code << endl; 
	} 
	else
	{
		root1->leftChild->code = s.append("0"); 
		s.erase(s.end() - 1); 
		root1->rightChild->code = s.append("1"); 
		s.erase(s.end() - 1); 

		depthFirstSearch(root1->leftChild, s.append("0")); 
		s.erase(s.end() - 1); 
		depthFirstSearch(root1->rightChild, s.append("1")); 
		s.erase(s.end() - 1);
	}
}