/*Nếu Danh sách liên kết, Stack, Queue là các khối xây dựng cơ bản, và Cây (Trees) đại diện cho kiến trúc phân cấp, 
thì Đồ thị là mô hình trừu tượng tối thượng để giải quyết các bài toán lưới phức tạp. 
Trong thực tế phát triển các hệ thống truyền dẫn viễn thông hay giao thức mạng (như thuật toán định tuyến OSPF), 
Đồ thị chính là cấu trúc bộ nhớ cốt lõi để lưu trữ bản đồ topology của toàn bộ mạng lưới các router.
Dưới góc nhìn lập trình C cấp thấp, một Đồ thị bao gồm Tập các Đỉnh (Vertices - V) và Tập các Cạnh (Edges - E). 
Bài toán đầu tiên là làm sao biểu diễn chúng trên RAM vật lý.

1. Ánh xạ Đồ thị vào Bộ nhớ (Graph Representations)
Có hai phương pháp kiến trúc bộ nhớ hoàn toàn trái ngược nhau để lưu trữ Đồ thị:

A. Ma trận kề (Adjacency Matrix)Bản chất: Là một mảng 2 chiều (Array 2D) kích thước V x V. 
Nếu có đường đi từ đỉnh i đến đỉnh j, matrix[i][j] = 1 (hoặc bằng trọng số của cạnh).
Đặc tính phần cứng: Tiêu tốn O(V^2) RAM vật lý bất kể số lượng cạnh ít hay nhiều. 
Truy vấn một liên kết cực nhanh O(1) nhờ tính cục bộ của bộ nhớ đệm (Cache). 
Tuy nhiên, với một mạng máy tính có 10,000 node nhưng mỗi node chỉ nối với 3 node khác, ma trận kề sẽ gây lãng phí hàng trăm MB bộ nhớ chứa toàn số 0.


B. Danh sách kề (Adjacency List)Bản chất: Một Mảng 1 chiều chứa V con trỏ. 
Mỗi con trỏ quản lý một Danh sách liên kết đơn (Singly Linked List) chứa các đỉnh kề với nó.
Đặc tính phần cứng: Tiêu tốn O(V + E) RAM. Cực kỳ tối ưu cho các đồ thị thưa (Sparse Graphs). 
Đây là cấu trúc tiêu chuẩn trong các hệ thống nhúng bị giới hạn về bộ nhớ.
Dưới đây là mã nguồn C khởi tạo Đồ thị bằng Danh sách kề:

#include <stdio.h>
#include <stdlib.h>

// Nút của Danh sách liên kết
struct AdjListNode {
    int dest;
    int weight; // Trọng số (ví dụ: độ trễ của đường truyền)
    struct AdjListNode* next;
};

// Mảng chứa các con trỏ quản lý danh sách
struct AdjList {
    struct AdjListNode *head; 
};

// Cấu trúc Đồ thị bao trùm
struct Graph {
    int V;
    struct AdjList* array; 
};

// Cấp phát Đồ thị động trên Heap
struct Graph* create_graph(int V) {
    struct Graph* graph = (struct Graph*)malloc(sizeof(struct Graph));
    graph->V = V;
    graph->array = (struct AdjList*)malloc(V * sizeof(struct AdjList));
    
    // Khởi tạo các danh sách kề là rỗng
    for (int i = 0; i < V; ++i)
        graph->array[i].head = NULL;
    return graph;
}

// Thêm cạnh (Đồ thị vô hướng)
void add_edge(struct Graph* graph, int src, int dest, int weight) {
    // Chèn dest vào danh sách của src (Chèn vào đầu - Insert at Head)
    struct AdjListNode* new_node = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
    new_node->dest = dest;
    new_node->weight = weight;
    new_node->next = graph->array[src].head;
    graph->array[src].head = new_node;

    // Chèn src vào danh sách của dest
    new_node = (struct AdjListNode*)malloc(sizeof(struct AdjListNode));
    new_node->dest = src;
    new_node->weight = weight;
    new_node->next = graph->array[dest].head;
    graph->array[dest].head = new_node;
}
2. Các Giải thuật Duyệt Đồ thị Cốt lõi
Để quét qua toàn bộ cấu trúc mạng lưới rắc rối này mà không bị kẹt vào vòng lặp vô hạn (Infinite Loop), 
chúng ta bắt buộc phải dùng mảng phụ trợ visited[] để đánh dấu các đỉnh đã đi qua.

A. Duyệt theo chiều sâu (DFS - Depth First Search)
Kiến trúc: Dùng Ngăn xếp (Stack). Trong thực tế, dùng luôn Call Stack của CPU thông qua Đệ quy (Recursion) là cách thanh lịch nhất. 
DFS rẽ nhánh sâu nhất có thể trước khi quay lui (Backtracking).
Ứng dụng: Phát hiện chu trình (Cycle Detection) trong mạng lưới, Sắp xếp tô-pô (Topological Sort) cho cây phụ thuộc gói phần mềm trên Linux.

void dfs_util(struct Graph* graph, int v, int visited[]) {
    visited[v] = 1; // Đánh dấu đỉnh hiện tại
    printf("%d ", v);

    // Quét toàn bộ danh sách liên kết của đỉnh v
    struct AdjListNode* current = graph->array[v].head;
    while (current != NULL) {
        int connected_vertex = current->dest;
        if (!visited[connected_vertex]) {
            dfs_util(graph, connected_vertex, visited); // Gọi đệ quy đẩy lên Stack
        }
        current = current->next;
    }
}

void dfs(struct Graph* graph, int start_vertex) {
    int* visited = (int*)calloc(graph->V, sizeof(int));
    printf("[+] Kết quả Duyệt DFS: ");
    dfs_util(graph, start_vertex, visited);
    printf("\n");
    free(visited);
}
B. Duyệt theo chiều rộng (BFS - Breadth First Search)
Kiến trúc: Bắt buộc dùng Hàng đợi (Queue). Cấu trúc sẽ lan tỏa theo từng cấp độ (level) tính từ đỉnh gốc, y hệt như những vòng sóng nước.
Ứng dụng: Tìm đường đi ngắn nhất trong Đồ thị không có trọng số (Unweighted Graph).
(BFS yêu cầu tích hợp nguyên bộ mã nguồn của Cấu trúc Queue bằng Linked List ở các phần trước. 
Thuật toán là: Đưa gốc vào Queue -> Lặp: Rút Queue ra, đánh dấu Visited, và nhét toàn bộ các đỉnh kề chưa visit vào lại Queue).

3. Giải thuật Ứng dụng: Đường đi ngắn nhất (Dijkstra)
Đây là "chén thánh" của thuật toán Đồ thị. 
Dijkstra là lõi của các giao thức định tuyến trạng thái liên kết (Link-State Routing Protocols). 
Nhiệm vụ của nó là tìm đường đi có tổng trọng số thấp nhất từ một router gốc đến mọi router khác.
Cấu trúc dữ liệu yêu cầu: Dijkstra sử dụng thuật toán Tham lam (Greedy). 
Ở mỗi bước, nó cần lấy ra đỉnh có khoảng cách nhỏ nhất chưa được xử lý.
Triển khai thực tế trên viễn thông: Dùng Min-Heap (Priority Queue) để tìm đỉnh min trong O(log V), đẩy tổng thời gian xuống O((V + E) log V).
Triển khai nền tảng: Dùng Mảng tuyến tính tốn O(V) để tìm đỉnh min, tổng thời gian O(V^2). 
(Dưới đây là mã nguồn dùng ma trận kề cho dễ quan sát logic toán học).

#include <limits.h>
#include <stdbool.h>

#define V_DIJKSTRA 9 // Số lượng đỉnh trong mạng

// Hàm tìm đỉnh có khoảng cách nhỏ nhất từ mảng dist
int min_distance(int dist[], bool sptSet[]) {
    int min = INT_MAX, min_index;
    for (int v = 0; v < V_DIJKSTRA; v++)
        if (sptSet[v] == false && dist[v] <= min) {
            min = dist[v];
            min_index = v;
        }
    return min_index;
}

// Thuật toán Dijkstra trên Ma trận kề
void dijkstra(int graph[V_DIJKSTRA][V_DIJKSTRA], int src) {
    int dist[V_DIJKSTRA];     // Mảng lưu khoảng cách ngắn nhất từ src đến i
    bool sptSet[V_DIJKSTRA];  // Đánh dấu các đỉnh đã được chốt (Shortest Path Tree)

    // Khởi tạo tất cả khoảng cách là Vô cực (INT_MAX)
    for (int i = 0; i < V_DIJKSTRA; i++) {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }

    // Khoảng cách từ đỉnh gốc đến chính nó luôn bằng 0
    dist[src] = 0;

    // Tìm đường đi cho toàn bộ các đỉnh
    for (int count = 0; count < V_DIJKSTRA - 1; count++) {
        // Lấy đỉnh có khoảng cách nhỏ nhất trong số các đỉnh chưa chốt
        int u = min_distance(dist, sptSet);
        
        // Đưa đỉnh này vào tập đã chốt
        sptSet[u] = true;

        // Cập nhật khoảng cách cho các đỉnh kề của u (Thuật toán Thư giãn - Relaxation)
        for (int v = 0; v < V_DIJKSTRA; v++) {
            // Điều kiện cập nhật:
            // 1. Chưa bị chốt
            // 2. Có đường nối vật lý (graph[u][v] != 0)
            // 3. Đường đi thông qua u ngắn hơn đường đi trực tiếp hiện tại
            if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX 
                && dist[u] + graph[u][v] < dist[v]) {
                dist[v] = dist[u] + graph[u][v];
            }
        }
    }

    // In bảng định tuyến (Routing Table)
    printf("Đỉnh \t Khoảng cách từ Gốc\n");
    for (int i = 0; i < V_DIJKSTRA; i++)
        printf("%d \t %d\n", i, dist[i]);
}
Bằng việc kết thúc phần Đồ thị, bộ khung nền tảng về tổ chức bộ nhớ và xử lý dữ liệu của bạn đã hoàn thiện từ A đến Z.
*/