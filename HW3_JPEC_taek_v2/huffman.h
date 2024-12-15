#include <iostream> 
#include <vector>
#include <map>

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

void getHuffmanCode(const std::map<int, int>& frequencyMap);
void getHuffmanCodeForAC(const std::map<std::pair<int, int>, int>& frequencyMap);
node getHuffmanTree();
node extractMin();
void depthFirstSearch(node* tempRoot, string s); 


void getHuffmanCode(const std::map<int, int>& frequencyMap) {
	nodeArray.clear();

    // 빈도 맵을 노드 배열로 변환
    for (const auto& entry : frequencyMap) {
        node tempNode;
        tempNode.characters = std::to_string(entry.first); // SIZE 또는 AC 키 값
        tempNode.frequency = entry.second;
        tempNode.leftChild = nullptr;
        tempNode.rightChild = nullptr;
        nodeArray.push_back(tempNode);
    }

	//Huffman Tree 생성 
	node root = getHuffmanTree();

	std::cout << "\nHuffman Coding Table:\n";
    std::cout << "Symbol\tCode\n";
    std::cout << "-----------------\n";

	//Huffman Coding Table 생성
	depthFirstSearch(&root, "");
}

void getHuffmanCodeForAC(const std::map<std::pair<int, int>, int>& frequencyMap) {
    nodeArray.clear();
    for (const auto& entry : frequencyMap) {
        node tempNode;
        tempNode.characters = "(" + std::to_string(entry.first.first) + "," + std::to_string(entry.first.second) + ")";
        tempNode.frequency = entry.second;
        tempNode.leftChild = NULL;
        tempNode.rightChild = NULL;
        nodeArray.push_back(tempNode);
    }

    // Huffman Tree 생성
    node root = getHuffmanTree();

    // Huffman 코드 출력
    std::cout << "AC Huffman Coding Table:\n";
    std::cout << "Symbol\tCode\n";
    std::cout << "-----------------\n";
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