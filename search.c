/*1. Tìm kiếm Tuyến tính (Linear Search)
Cả hai cấu trúc đều thực hiện Tìm kiếm Tuyến tính với độ phức tạp toán học là O(n).
A. Trên Mảng (Array)Bản chất: Quét qua một khối nhớ liên tục. 
Tận dụng tối đa bộ nhớ đệm (L1 Cache) của CPU.

int linear_search_array(int arr[], int size, int target) {
    for (int i = 0; i < size; i++) {
        if (arr[i] == target) {
            return i; // Trả về chỉ số
        }
    }
    return -1; // Không tìm thấy
}

B. Trên Danh sách liên kết (Linked List)Bản chất: Nhảy cóc qua các địa chỉ vật lý rời rạc trên phân vùng Heap. 
Gây ra hiện tượng Cache Miss liên tục, khiến tốc độ thực tế chậm hơn mảng rất nhiều.

struct Node {
    int data;
    struct Node *next;
};

struct Node* linear_search_list(struct Node *head, int target) {
    struct Node *current = head;
    while (current != NULL) {
        if (current->data == target) {
            return current; // Trả về con trỏ chứa dữ liệu
        }
        current = current->next; // Dịch chuyển sang khối nhớ kế tiếp
    }
    return NULL; // Không tìm thấy
}
*/

/*2. Tìm kiếm Nhị phân (Binary Search)
Để thuật toán này hoạt động chính xác, dữ liệu đầu vào bắt buộc phải được sắp xếp từ trước.
A. Trên Mảng (Array) - Giải pháp Chuẩn mựcĐộ phức tạp: O(log n). 
Tính toán phần tử ở giữa chỉ tốn 1 phép cộng và chia nhị phân.

int binary_search_array(int arr[], int size, int target) {
    int left = 0;
    int right = size - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2; // Chống tràn số nguyên

        if (arr[mid] == target) return mid;
        
        // Thu hẹp không gian tìm kiếm vật lý
        if (arr[mid] < target) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

B. Trên Danh sách liên kết (Linked List) - Giải pháp Học thuậtVấn đề kỹ thuật: Chúng ta không có chỉ số left và right. 
Thay vào đó, chúng ta phải dùng con trỏ. 
Để tìm điểm giữa (mid), bắt buộc phải dùng Thuật toán Rùa và Thỏ (Tortoise and Hare): Con trỏ nhanh chạy 2 bước, con trỏ chậm chạy 1 bước. 
Khi con nhanh chạm đích, con chậm sẽ ở đúng vị trí giữa.Độ phức tạp: Thời gian tìm điểm giữa là O(n/2), 
khiến tổng thời gian của phương pháp này bị giáng cấp thê thảm xuống O(n), tương đương Linear Search.

// Hàm hỗ trợ: Tìm nút ở giữa bằng kỹ thuật 2 con trỏ (Rùa và Thỏ)
struct Node* get_middle(struct Node *start, struct Node *last) {
    if (start == NULL) return NULL;

    struct Node *slow = start;
    struct Node *fast = start->next;

    while (fast != last) {
        fast = fast->next;
        if (fast != last) {
            slow = slow->next; // Rùa đi 1 bước
            fast = fast->next; // Thỏ đi 2 bước
        }
    }
    return slow; // Khi Thỏ chạm mốc 'last', Rùa đang ở giữa
}

// Thuật toán tìm kiếm nhị phân trên Linked List
struct Node* binary_search_list(struct Node *head, int target) {
    struct Node *start = head;
    struct Node *last = NULL; // last ban đầu là điểm kết thúc (NULL)

    // Điều kiện lặp: Còn không gian để tìm kiếm
    do {
        // Tìm khối nhớ ở giữa
        struct Node *mid = get_middle(start, last);

        // Trường hợp mảng rỗng hoặc lỗi
        if (mid == NULL) return NULL;

        // Trúng đích
        if (mid->data == target) return mid;

        // Nếu giá trị ở giữa nhỏ hơn mục tiêu, loại bỏ nửa bên trái
        // Bằng cách thu hẹp điểm bắt đầu (start)
        else if (mid->data < target) {
            start = mid->next;
        } 
        // Nếu giá trị ở giữa lớn hơn mục tiêu, loại bỏ nửa bên phải
        // Bằng cách thu hẹp điểm kết thúc (last)
        else {
            last = mid;
        }

    } while (last == NULL || last != start);

    return NULL; // Không tìm thấy
}
Bài học Hệ thống:Từ góc độ tổ chức dữ liệu thực tế, không ai dùng Binary Search trên Singly Linked List. 
Nếu một hệ thống vừa cần kích thước động (như Linked List) lại vừa cần tốc độ tra cứu O(log n) (như Binary Search), 
các kỹ sư sẽ bỏ qua cấu trúc này và chuyển sang sử dụng Cây nhị phân tìm kiếm (BST) hoặc Skip List (Danh sách nhảy).
*/


