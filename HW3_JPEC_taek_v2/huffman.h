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

void getHuffmanCode(const std::map<int, int>& frequencyMap, std::map<int, std::string>& dcHuffmanTable);
void getHuffmanCodeForAC(const std::map<std::pair<int, int>, int>& frequencyMap, std::map<std::pair<int, int>, std::string>& acHuffmanTable);
node getHuffmanTree();
node extractMin();
void DCdepthFirstSearch(node* tempRoot, std::string s, std::map<int, std::string>& dcHuffmanTable);
void ACdepthFirstSearch(node* tempRoot, std::string s, std::map<std::pair<int, int>, std::string>& acHuffmanTable);
std::pair<int, int> ParseRunValue(const std::string& str); // 문자열 파싱 함수



void getHuffmanCode(const std::map<int, int>& frequencyMap, std::map<int, std::string>& dcHuffmanTable) {
	nodeArray.clear();

    // 빈도 맵을 노드 배열로 변환
    for (const auto& entry : frequencyMap) {
        node tempNode;
        tempNode.characters = std::to_string(entry.first);
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
	DCdepthFirstSearch(&root, "", dcHuffmanTable);
}

// 허프만 코드를 테이블에 저장
// void createHuffmanTable(node* root, const std::string& code, std::map<int, std::string>& dcHuffmanTable) {
//     if (root == nullptr) return;

//     if (root->leftChild == nullptr && root->rightChild == nullptr) { // 리프 노드
//         int symbol = std::stoi(root->characters);
//         dcHuffmanTable[symbol] = code;
//         std::cout << symbol << "\t" << code << "\n";
//     } else {
//         createHuffmanTable(root->leftChild, code + "0", dcHuffmanTable);
//         createHuffmanTable(root->rightChild, code + "1", dcHuffmanTable);
//     }
// }

void getHuffmanCodeForAC(const std::map<std::pair<int, int>, int>& frequencyMap, std::map<std::pair<int, int>, std::string>& acHuffmanTable) {
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
    ACdepthFirstSearch(&root, "", acHuffmanTable);

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
		cout << "\t" << root1->characters << "\t" << root1->code << endl; // 터미널에 출력
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

void ACdepthFirstSearch(node* tempRoot, string s, std::map<std::pair<int, int>, std::string>& acHuffmanTable)
{
	node* root1 = tempRoot;

	root1->code = s; 
	if (root1 == NULL)
	{

	} 
	else if (root1->leftChild == NULL && root1->rightChild == NULL)
	{
		std::pair<int, int> key = ParseRunValue(root1->characters); // 타입 변환
        acHuffmanTable[key] = s; // 허프만 코드 저장
		cout << "\t" << root1->characters << "\t" << root1->code << endl; // 터미널에 출력
	} 
	else
	{
		root1->leftChild->code = s.append("0"); 
		s.erase(s.end() - 1); 
		root1->rightChild->code = s.append("1"); 
		s.erase(s.end() - 1); 

		ACdepthFirstSearch(root1->leftChild, s.append("0"), acHuffmanTable); 
		s.erase(s.end() - 1); 
		ACdepthFirstSearch(root1->rightChild, s.append("1"), acHuffmanTable); 
		s.erase(s.end() - 1);
	}
}

// "(Run, Value)" 형태의 문자열을 std::pair<int, int>로 변환하는 파싱함수
std::pair<int, int> ParseRunValue(const std::string& str) {
    int run = 0, value = 0;
    sscanf(str.c_str(), "(%d,%d)", &run, &value); // 문자열에서 숫자 두 개를 추출
    return {run, value};
}