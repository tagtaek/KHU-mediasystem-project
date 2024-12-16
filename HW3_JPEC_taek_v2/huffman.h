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

void getDCHuffmanCode(const std::map<int, int>& frequencyMap, std::map<int, std::string>& dcHuffmanTable);
void getHuffmanCodeForAC(const std::map<std::pair<int, int>, int>& frequencyMap, std::map<std::pair<int, int>, std::string>& acHuffmanTable);
node getHuffmanTree();
node extractMin();
void DCdepthFirstSearch(node* tempRoot, std::string s, std::map<int, std::string>& dcHuffmanTable);
void ACdepthFirstSearch(node* tempRoot, string s, std::map<std::string, std::string>& acHuffmanTable);

void getDCHuffmanCode(const std::map<int, int>& frequencyMap, std::map<int, std::string>& dcHuffmanTable) {
    nodeArray.clear(); // 기존 노드 배열 초기화

    // `frequencyMap`을 순회하여 허프만 트리의 노드 배열 생성
    for (const auto& entry : frequencyMap) {
        if (entry.second > 0) { // 빈도가 0이 아니어야 노드 생성
            node tempNode;
            tempNode.characters = std::to_string(entry.first); // SIZE 값 (심볼)
            tempNode.frequency = entry.second;                // 빈도 수
            tempNode.leftChild = nullptr;
            tempNode.rightChild = nullptr;
            nodeArray.push_back(tempNode);
        }
    }

    // 허프만 트리 생성
    node root = getHuffmanTree();

    // std::cout << "\nHuffman Coding Table:\n";
    // std::cout << "Symbol\tCode\n";
    // std::cout << "-----------------\n";

    // 허프만 코딩 테이블 생성
    DCdepthFirstSearch(&root, "", dcHuffmanTable);
}

void getACHuffmanCode(const std::map<std::string, int>& frequencyMap, std::map<std::string, std::string>& acHuffmanTable) {
    nodeArray.clear();
    for (const auto& entry : frequencyMap) {
        node tempNode;
        tempNode.characters = entry.first; // 예: "skip/size"
        tempNode.frequency = entry.second; // 빈도 수
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
    ACdepthFirstSearch(&root, "", acHuffmanTable);
	// // AC Huffman Coding Table 확인
	// std::cout << "\nGenerated AC Huffman Table:\n";
	// for (const auto& entry : acHuffmanTable) {
   	//  std::cout << "Key: " << entry.first << " -> Code: " << entry.second << std::endl;
// }
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

void DCdepthFirstSearch(node* tempRoot, string s, std::map<int, std::string>& dcHuffmanTable)
{
	node* root1 = tempRoot;

	root1->code = s; 
	if (root1 == NULL)
	{

	} 
	else if (root1->leftChild == NULL && root1->rightChild == NULL)
	{
		dcHuffmanTable[std::stoi(root1->characters)] = s; // 문자열을 int로 변환 후 저장 // 실제 테이블에 저장
		// cout << "\t" << root1->characters << "\t" << root1->code << endl; // 터미널에 출력
	} 
	else
	{
		root1->leftChild->code = s.append("0"); 
		s.erase(s.end() - 1); 
		root1->rightChild->code = s.append("1"); 
		s.erase(s.end() - 1); 

		DCdepthFirstSearch(root1->leftChild, s.append("0"), dcHuffmanTable); 
		s.erase(s.end() - 1); 
		DCdepthFirstSearch(root1->rightChild, s.append("1"), dcHuffmanTable); 
		s.erase(s.end() - 1);
	}
}

void ACdepthFirstSearch(node* tempRoot, string s, std::map<std::string, std::string>& acHuffmanTable)
{
	node* root2 = tempRoot;

	root2->code = s; 
	if (root2 == NULL)
	{

	} 
	else if (root2->leftChild == NULL && root2->rightChild == NULL)
	{
        acHuffmanTable[root2->characters] = s; //실제 저장
		// cout << "\t" << root2->characters << "\t" << root2->code << endl; // 터미널에 출력
	} 
	else
	{
		root2->leftChild->code = s.append("0"); 
		s.erase(s.end() - 1); 
		root2->rightChild->code = s.append("1"); 
		s.erase(s.end() - 1); 

		ACdepthFirstSearch(root2->leftChild, s.append("0"), acHuffmanTable); 
		s.erase(s.end() - 1); 
		ACdepthFirstSearch(root2->rightChild, s.append("1"), acHuffmanTable); 
		s.erase(s.end() - 1);
	}
}