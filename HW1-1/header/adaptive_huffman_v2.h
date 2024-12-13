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
	node* parent; // 부모를 가리키는 포인터 추가
};

vector<node> nodeArray; //트리 구성에 직접 관여x, 심볼 및 빈도수 관리
node* root = nullptr; // 트리의 루트를 가리키는 포인터
node* latestNewNode = nullptr;

void get_adaptive_Huffman_Code();
void processSymbol(char symbol);
void update_FREQ(const string& symbol);
void addToTree(node* newNode);
void depthFirstSearch(node* tempRoot, string s);
void updateFreqAndPosit(node* targetNode);
void swapNodes(node* nodeA, node* nodeB);
void findSwapCandidate(node* currentNode, node* targetNode, int& minDepth, node*& swapCandidate, int depth = 0);
void findSymbolInTree(node* currentNode, const string& symbol, node*& targetNode);

void get_adaptive_Huffman_Code(){
	// 초기 (root)new 노드
	root = new node;
	root->characters = "NEW";
	root->frequency = 0;
	root->leftChild = nullptr;
	root->rightChild = nullptr;
	root->parent = nullptr;
	
	latestNewNode = root;

	char alphabet; // 문자 종류의 갯수와 빈도수가 정해져있지 않음.
	cout << "Enter symbols one by one. Use '$' to end input : ";
	while (true){ // 종료심볼이 입력되기 전까지 계속해서 트리 재구성.
		cin >> alphabet;
		// 종료심볼 '$'가 들어오면 함수 종료.
		if (alphabet == '$') 
			break;
		// 새로운 심볼 처리
		processSymbol(alphabet);	
	}
}

void processSymbol(char symbol){
	bool exists = false;

	// nodeArray에 심볼이 존재하는지 확인
	for (auto& node : nodeArray) {
		// 비교를 위해 symbol => string으로 변환
		if (node.characters == string(1, symbol)){
			node.frequency++;
			exists = true;
			// 빈도갱신 및 트리업데이트 함수 호출 
			update_FREQ(node.characters);
			break;
		}
	}

	// 새로운 심볼이면 nodeArray에 추가
	if (!exists) {
		node newNode;
		newNode.characters = symbol; //새 심볼 추가
		newNode.frequency = 1; //초기 빈도값 설정
		newNode.leftChild = nullptr;
		newNode.rightChild = nullptr;
		newNode.parent = nullptr;
		nodeArray.push_back(newNode);
		cout << "Current nodeArray contents:" << endl;
    	

		//트리에 새로운 심볼 추가하는 함수
		addToTree(&newNode);
	}
	// for (const auto& n : nodeArray) {
    //     cout << "Character: " << n.characters << ", Frequency: " << n.frequency << endl;
   	// 	 }
	// 확인 과정
	cout << endl << endl;
	cout << "input: " << symbol << endl;
	cout << '\t' << "data" << '\t' << "count" << endl;
	cout << '\t' << "----------------------" << endl;
	for (auto& node : nodeArray){
		cout << '\t' << node.characters << '\t' << node.frequency << endl;
	}
	cout << endl << endl;
	cout << '\t' << "data" << '\t' << "code" << endl;
	cout << '\t' << "----------------------" << endl;
	

	// Adaptive_Huffman_Coding table 생성
	depthFirstSearch(root, "");
}

void addToTree(node* newNode) {
	if (root == nullptr){
		cerr << "Error: 'NEW' root node not initialized." << endl;
		return;
	}
	// 새로운 'NEW' 노드와 심볼 노드를 생성하여 연결
	node* newLeftChild = new node;
	node* newRightChild = new node;

	// Child가 포인터로 선언되었으니 화살표 연산자 사용.
	newLeftChild->characters = "NEW";
	newLeftChild->frequency = 0;
	newLeftChild->leftChild = nullptr;
	newLeftChild->rightChild = nullptr;
	newLeftChild->parent = latestNewNode;

	*newRightChild = *newNode;
	newRightChild->leftChild = nullptr;
	newRightChild->rightChild = nullptr;
	newRightChild->parent = latestNewNode;

	latestNewNode->leftChild = newLeftChild;
	latestNewNode->rightChild = newRightChild;

	//기존 new 노드의 characters를 EMPTY로 변경
	latestNewNode->characters = "EMPTY";
	latestNewNode = newLeftChild;

	// 빈도수 갱신 및 형제노드 위치조절
	updateFreqAndPosit(newRightChild);

	// Added check
	cout << "Added new symbol to tree under 'NEW' node." << endl;
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

void updateFreqAndPosit(node* targetNode) {
    if (targetNode == nullptr) return;

    node* currentNode = targetNode;

    while (currentNode != root) {
        node* parent = currentNode->parent;
        if (parent == nullptr) break;

        // 현재 노드가 부모의 왼쪽 자식인지 오른쪽 자식인지 확인
        node* sibling = (parent->leftChild == currentNode) ? parent->rightChild : parent->leftChild;

        // 교환 조건 확인
        if ((currentNode == parent->leftChild && sibling && currentNode->frequency > sibling->frequency) ||
            (currentNode == parent->rightChild && sibling && currentNode->frequency < sibling->frequency)) {
            swapNodes(sibling, currentNode);  // 형제 노드와 교환하여 빈도 조건 유지
        }

        // 상위 노드 빈도 갱신
        parent->frequency = parent->leftChild->frequency + parent->rightChild->frequency;
        currentNode = parent; // 상위로 이동하여 반복
    }
}

void swapNodes(node* nodeA, node* nodeB) {
    if (nodeA == nullptr || nodeB == nullptr) return;

    node* parentA = nodeA->parent;
    node* parentB = nodeB->parent;

    // 부모가 동일한 경우, 형제 노드 위치를 바꿉니다.
    if (parentA == parentB) {
        if (parentA->leftChild == nodeA) {
            parentA->leftChild = nodeB;
            parentA->rightChild = nodeA;
        } else {
            parentA->leftChild = nodeA;
            parentA->rightChild = nodeB;
        }
    }
    // 부모가 다를 경우, 각각의 부모 노드가 자식 포인터를 교환하도록 합니다.
    else {
        if (parentA) {
            if (parentA->leftChild == nodeA) {
                parentA->leftChild = nodeB;
            } else {
                parentA->rightChild = nodeB;
            }
        }
        if (parentB) {
            if (parentB->leftChild == nodeB) {
                parentB->leftChild = nodeA;
            } else {
                parentB->rightChild = nodeA;
            }
        }
    }

    // 각 노드의 부모 포인터를 업데이트
    nodeA->parent = parentB;
    nodeB->parent = parentA;
}

void update_FREQ(const string& symbol) {
    // 1. 트리에서 해당 심볼 노드 찾기
    node* targetNode = nullptr;
    findSymbolInTree(root, symbol, targetNode);

    // 해당 심볼을 찾지 못한 경우 종료
    if (targetNode == nullptr) {
        cerr << "Error: Symbol not found in the tree." << endl;
        return;
    }

	targetNode->frequency++;

	// 2. 교환 후보 찾기
    node* swapCandidate = nullptr;
    int minDepth = INT_MAX; // 가장 낮은 깊이를 찾기 위해 초기값을 최대값으로 설정
    findSwapCandidate(root, targetNode, minDepth, swapCandidate);

    // 3. 교환 후보가 있는 경우 교환
    if (swapCandidate && swapCandidate->frequency < targetNode->frequency) {
        swapNodes(swapCandidate, targetNode);
    }

    // 4. 빈도 조건에 맞추기 위해 위치 조정 및 빈도 전파
    updateFreqAndPosit(targetNode);
}

// 교환 후보 노드를 찾기 위한 함수
void findSwapCandidate(node* currentNode, node* targetNode, int& minDepth, node*& swapCandidate, int depth) {
    if (currentNode == nullptr) return;

    // 심볼이 있는 노드만 교환 후보로 고려, "EMPTY" 노드는 제외
    if (!currentNode->characters.empty() && currentNode->characters != "EMPTY" &&
        currentNode != targetNode &&
        currentNode->frequency < targetNode->frequency && depth < minDepth) {
        swapCandidate = currentNode;
        minDepth = depth;
        return;
    }

    // DFS로 왼쪽, 오른쪽 자식 순회
    findSwapCandidate(currentNode->leftChild, targetNode, minDepth, swapCandidate, depth + 1);
    findSwapCandidate(currentNode->rightChild, targetNode, minDepth, swapCandidate, depth + 1);
}

void findSymbolInTree(node* currentNode, const string& symbol, node*& targetNode) {
    if (currentNode == nullptr || targetNode != nullptr) return;

    // 심볼이 일치하는 노드를 찾으면 targetNode에 할당
    if (currentNode->characters == symbol) {
        targetNode = currentNode;
        return;
    }

    // 왼쪽과 오른쪽 자식으로 재귀적 탐색
    findSymbolInTree(currentNode->leftChild, symbol, targetNode);
    findSymbolInTree(currentNode->rightChild, symbol, targetNode);
}