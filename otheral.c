/*1. Đệ quy (Recursion) - Quản trị Call Stack
Đệ quy là việc một hàm tự gọi lại chính nó. 
Về mặt toán học, nó chia một bài toán lớn thành các bài toán con giống hệt nhau cho đến khi chạm "điểm dừng" (Base Case).
Phân tích Kiến trúc Bộ nhớ:Mỗi lần một hàm đệ quy được gọi, hệ điều hành phải đẩy một Khung ngăn xếp (Stack Frame) mới lên phân vùng Call Stack. 
Khung này chứa: Địa chỉ trả về (Return address), các tham số, và biến cục bộ.
Trong các môi trường ảo hóa chạy trực tiếp trên máy vật lý thông qua Hyper-V, 
hoặc khi lập trình nhân hệ điều hành nhúng, không gian Stack bị giới hạn cực kỳ khắt khe (thường chỉ vài KB đến vài MB). 
Nếu độ sâu đệ quy quá lớn, nó sẽ gây ra lỗi Stack Overflow, đánh sập toàn bộ tiến trình. Do đó, đệ quy thường chỉ được dùng khi làm việc với 
cấu trúc Cây (độ sâu O(log n)), tránh dùng cho mảng tuyến tính O(n).

// Ví dụ: Tính giai thừa. Mỗi lần gọi tiêu tốn thêm ~32 bytes trên Stack.
unsigned long long factorial_recursive(int n) {
    if (n <= 1) return 1; // Base case
    return n * factorial_recursive(n - 1);
}
*/

/*2. Quy hoạch động (Dynamic Programming - DP) - Đánh đổi RAM lấy CPU
Quy hoạch động là giải pháp triệt để cho điểm yếu của Đệ quy. 
Khi đệ quy tính toán lại một bài toán con quá nhiều lần (Overlapping Subproblems), nó gây lãng phí chu kỳ xung nhịp CPU khổng lồ.
DP giải quyết việc này bằng kỹ thuật Ghi nhớ (Memoization) hoặc Lập bảng (Tabulation): 
Tính kết quả một lần, lưu nó vào một Mảng trên RAM, và tái sử dụng cho các lần sau.
Phân tích Kiến trúc Bộ nhớ:
Quy hoạch động bản chất là sự đánh đổi: 
Chấp nhận tốn thêm O(n) không gian bộ nhớ vật lý (Mảng/Bảng) để giảm thời gian thực thi từ mức hàm mũ O(2^n) xuống mức tuyến tính O(n)

// Ví dụ: Dãy Fibonacci - Tối ưu hóa bằng mảng 1 chiều (Bottom-up)
// Thay vì đệ quy O(2^n), ta dùng mảng tốn O(n) RAM nhưng tốc độ là O(n).
unsigned long long fibonacci_dp(int n) {
    if (n <= 1) return n;
    
    // Cấp phát mảng trên Heap để lưu kết quả các bài toán con
    unsigned long long *dp = (unsigned long long*)malloc((n + 1) * sizeof(unsigned long long));
    dp[0] = 0;
    dp[1] = 1;
    
    // CPU chỉ thực hiện phép cộng, không tốn chi phí gọi hàm (Call Stack)
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i - 1] + dp[i - 2]; 
    }
    
    unsigned long long result = dp[n];
    free(dp);
    return result;
}
*/


/*3. Cửa sổ trượt (Sliding Window) - Tối ưu hóa I/O Mảng
Sliding Window là kỹ thuật tối thượng để giải quyết các bài toán tìm mảng con (Sub-array) thỏa mãn một điều kiện nhất định. 
Thay vì dùng hai vòng lặp lồng nhau O(n^2) quét đi quét lại dữ liệu, ta tạo ra một "cửa sổ" bằng hai con trỏ left và right, 
sau đó "trượt" nó trên mảng.
Phân tích Kiến trúc Bộ nhớ:
Kỹ thuật này giữ cho độ phức tạp thời gian luôn ở mức O(n) và không gian ở mức O(1). 
Nó cực kỳ thân thiện với bộ nhớ đệm (L1 Cache) vì các thao tác đọc RAM diễn ra hoàn toàn tuyến tính, không bao giờ quay ngược lại. 
Kỹ thuật này được ứng dụng trực tiếp trong các bộ điều khiển mạng SDN (như kiến trúc POX chạy trên Mininet)
để giám sát và phân tích lưu lượng gói tin (packet streams) trong một khoảng thời gian thực (time window) nhằm phát hiện DDoS hoặc điều phối băng thông.

// Tìm mảng con liên tiếp có kích thước K có tổng lớn nhất
int max_sum_subarray(int arr[], int n, int k) {
    int max_sum = 0, window_sum = 0;

    // Bước 1: Tính tổng của cửa sổ đầu tiên
    for (int i = 0; i < k; i++) {
        window_sum += arr[i];
    }
    max_sum = window_sum;

    // Bước 2: Trượt cửa sổ đi 1 ô
    for (int i = k; i < n; i++) {
        // Thêm phần tử mới vào đầu cửa sổ, trừ đi phần tử cũ ở đuôi cửa sổ
        // CPU chỉ thực hiện 2 phép toán thay vì lặp lại K lần
        window_sum += arr[i] - arr[i - k];
        
        if (window_sum > max_sum) {
            max_sum = window_sum;
        }
    }
    return max_sum;
}
*/


/*4. Bảng băm (Hash Map / Hash Table) - Tra cứu O(1)
Hash Map là cấu trúc dữ liệu kết hợp sức mạnh truy cập ngẫu nhiên của Mảng (Array) và khả năng mở rộng của Danh sách liên kết (Linked List). 
Mục tiêu của nó là biến một Khóa (Key - có thể là chuỗi ký tự) thành một Chỉ số mảng (Index) bằng một Hàm băm (Hash Function).

Phân tích Kiến trúc Bộ nhớ & Bảo mật:
Trái ngược với các thuật toán băm mật mã phức tạp như SHA-512 (vốn được thiết kế để tạo ra các bản tóm tắt 512-bit an toàn tuyệt đối, 
chống dịch ngược và tính toán rất nặng), hàm băm trong cấu trúc Hash Map ưu tiên tính đơn giản, 
tốc độ thực thi (tính bằng nano-giây) và độ phân tán đồng đều.

Mảng vật lý của Hash Map lưu trữ các con trỏ. 
Nếu hai khóa khác nhau băm ra cùng một vị trí (Xung đột - Collision), ta dùng Danh sách liên kết nối đuôi chúng lại (Kỹ thuật Chaining).

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 100

// Nút của Danh sách liên kết dùng để xử lý đụng độ (Collision)
struct HashNode {
    char key[50];
    int value;
    struct HashNode *next;
};

// Hàm băm đơn giản (DJB2 Hash) - Nhanh, nhẹ, phân tán tốt cho Hash Map
unsigned int hash_function(const char *key) {
    unsigned long hash = 5381;
    int c;
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c (Dịch bit để tối ưu CPU)
    }
    return hash % TABLE_SIZE; // Ép về chỉ số hợp lệ của Mảng
}

// Chèn dữ liệu vào Hash Map
void insert_hash_map(struct HashNode *table[], const char *key, int value) {
    unsigned int index = hash_function(key);
    
    // Tạo khối nhớ mới trên Heap
    struct HashNode *new_node = (struct HashNode*)malloc(sizeof(struct HashNode));
    strcpy(new_node->key, key);
    new_node->value = value;
    
    // Nối vào danh sách liên kết tại ô table[index]
    new_node->next = table[index];
    table[index] = new_node;
}

// Tra cứu O(1)
int search_hash_map(struct HashNode *table[], const char *key) {
    unsigned int index = hash_function(key);
    struct HashNode *current = table[index];
    
    // Duyệt danh sách liên kết (thường rất ngắn nếu hàm băm tốt)
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current->value; // Tìm thấy
        }
        current = current->next;
    }
    return -1; // Không tìm thấy
}
*/