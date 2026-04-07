/* STRUCT
1. Khai báo, Khởi tạo và Hiện tượng Căn chỉnh bộ nhớ (Memory Alignment)
Bản chất của struct là một kiểu dữ liệu phức hợp (composite data type) 
cho phép nhóm các biến có kiểu dữ liệu khác nhau vào một khối bộ nhớ liên tục.

#include <stdio.h>

// Định nghĩa cấu trúc SensorData
struct SensorData {
    char id;        // 1 byte
    int value;      // 4 bytes
    char status;    // 1 byte
};

int main() {
    // Khởi tạo biến cấu trúc
    struct SensorData sensor1 = {'A', 1024, 'K'};
    
    // Truy xuất thành viên thông qua toán tử dấu chấm (.)
    printf("ID: %c, Value: %d\n", sensor1.id, sensor1.value);
    
    // Phân tích kích thước bộ nhớ
    printf("Size of char: %lu\n", sizeof(char)); // Thường là 1
    printf("Size of int: %lu\n", sizeof(int));   // Thường là 4
    printf("Size of struct: %lu\n", sizeof(struct SensorData)); 
    
    return 0;
}
Phân tích kỹ thuật:Theo toán học thông thường, kích thước của SensorData phải là 1 + 4 + 1 = 6 bytes. 
Tuy nhiên, kết quả in ra của sizeof(struct SensorData) trên hầu hết các kiến trúc (như x86_64 hoặc ARM) sẽ là 12 bytes.
Nguyên lý Alignment và Padding:CPU đọc bộ nhớ theo các "từ máy" (Word boundaries), ví dụ 4 bytes trên kiến trúc 32-bit. 
Việc đọc một biến int (4 bytes) nằm vắt chéo giữa hai chu kỳ đọc của CPU sẽ gây giảm hiệu năng nghiêm trọng, hoặc thậm chí 
gây lỗi phần cứng (Hardware Exception).
Để ngăn chặn điều này, Trình biên dịch tự động chèn các byte đệm (Padding) vào giữa các thành viên.
Sơ đồ bộ nhớ thực tế của sensor1 sẽ là: [1 byte id] + [3 bytes padding] + [4 bytes value] + [1 byte status] + [3 bytes padding ở cuối để căn chỉnh cho mảng struct].
Tối ưu hóa: Để tiết kiệm bộ nhớ, lập trình viên hệ thống luôn phải sắp xếp các biến trong struct theo thứ tự kích thước giảm dần (ví dụ: int, sau đó đến các char đứng cạnh nhau).


2. Con trỏ cấu trúc và Toán tử Mũi tên (->)
Khi thao tác với struct thông qua cấp phát động hoặc truyền vào hàm, chúng ta bắt buộc phải sử dụng con trỏ.

#include <stdio.h>
#include <stdlib.h>
struct Point {
    int x;
    int y;
};

int main() {
    // Cấp phát động một khối nhớ cho cấu trúc trên Heap
    struct Point *p = (struct Point *)malloc(sizeof(struct Point));
    
    if (p != NULL) {
        // Ghi dữ liệu vào vùng nhớ thông qua con trỏ
        (*p).x = 10; // Cách 1: Giải tham chiếu (Dereference) rồi truy cập
        p->y = 20;   // Cách 2: Sử dụng toán tử mũi tên (Cú pháp chuẩn)
        
        printf("Point: (%d, %d)\n", p->x, p->y);
        free(p); // Giải phóng bộ nhớ
    }
    return 0;
}
Phân tích kỹ thuật:Phép toán (*p).x yêu cầu dấu ngoặc đơn vì toán tử . có độ ưu tiên cao hơn toán tử *. 
Nếu viết *p.x, trình biên dịch sẽ hiểu sai thành "lấy giá trị mà biến con trỏ x đang trỏ tới" (điều này gây lỗi cú pháp vì x là int, không phải con trỏ).
Toán tử -> được ngôn ngữ C cung cấp để giải quyết sự cồng kềnh này. 
Câu lệnh p->y chỉ thị trực tiếp cho CPU: "Đi đến địa chỉ bộ nhớ mà p đang giữ, cộng thêm một độ dời (offset) tương ứng với vị trí của y trong cấu trúc, và ghi giá trị vào đó".


3. Cấu trúc trong Lời gọi hàm (Pass by Value vs Pass by Pointer)

#include <stdio.h>

struct Config {
    int mode;
    int threshold;
};

// Truyền tham trị (Pass by Value) - KHÔNG KHUYÊN DÙNG CHO STRUCT LỚN
void updateConfig_Value(struct Config c) {
    c.mode = 1; // Chỉ thay đổi trên bản sao
}

// Truyền qua con trỏ (Pass by Pointer) - TIÊU CHUẨN THỰC TẾ
void updateConfig_Pointer(struct Config *c) {
    c->mode = 1; // Can thiệp trực tiếp vào bản gốc
}

int main() {
    struct Config myConf = {0, 100};
    
    updateConfig_Value(myConf);
    printf("After Value pass: %d\n", myConf.mode); // Vẫn là 0
    
    updateConfig_Pointer(&myConf);
    printf("After Pointer pass: %d\n", myConf.mode); // Đã thành 1
    
    return 0;
}
Phân tích kỹ thuật:
Khi gọi updateConfig_Value(myConf), toàn bộ khối bộ nhớ của myConf (8 bytes) bị sao chép (copy) và đẩy lên đỉnh ngăn xếp (Stack) để tạo thành một biến cục bộ c. 
Mọi thao tác ghi chỉ tác động lên vùng Stack tạm thời này. 
Nếu struct có kích thước lớn (ví dụ mảng 1000 phần tử bên trong), thao tác này gây tràn ngăn xếp (Stack Overflow) và cạn kiệt chu kỳ CPU.
Khi gọi updateConfig_Pointer(&myConf), hệ thống chỉ đẩy một con trỏ (8 bytes trên máy 64-bit) lên Stack.
Hàm sử dụng con trỏ này để truy xuất ngược về vùng nhớ nguyên bản của myConf và sửa đổi trực tiếp (Mutation). 
Đây là tiêu chuẩn hiệu năng bắt buộc trong lập trình C.


4. Cấu trúc tự trỏ (Self-referential Structures)
Đây là nền tảng kỹ thuật bắt buộc để triển khai các cấu trúc dữ liệu động (Danh sách liên kết, Cây nhị phân)

#include <stdio.h>

struct Node {
    int data;
    // struct Node next;      // LỖI: Biên dịch không thể xác định kích thước
    struct Node *next;        // HỢP LỆ: Kích thước con trỏ là cố định
};

int main() {
    struct Node node1 = {10, NULL};
    struct Node node2 = {20, NULL};
    
    // Liên kết hai khối nhớ
    node1.next = &node2;
    
    // Duyệt dữ liệu qua con trỏ
    printf("Node 1: %d\n", node1.data);
    printf("Node 2 (via Node 1): %d\n", node1.next->data);
    
    return 0;
}
Phân tích kỹ thuật:
Trình biên dịch C hoạt động theo cơ chế Single-pass (đọc từ trên xuống dưới). 
Khi nó phân tích định nghĩa struct Node, nó phải tính toán được sizeof(struct Node) ngay lập tức.
Nếu bạn khai báo thành viên struct Node next;, cấu trúc này sẽ chứa chính nó, dẫn đến một định nghĩa đệ quy vô hạn về mặt kích thước bộ nhớ. 
Trình biên dịch sẽ báo lỗi.
Khi bạn khai báo struct Node *next;, trình biên dịch biết chắc chắn rằng thành viên next chỉ là một địa chỉ vật lý (chiếm đúng 8 bytes trên máy 64-bit), bất kể cấu trúc Node to cỡ nào. 
Nhờ vậy, kích thước của toàn bộ struct Node được xác định thành công 4 bytes data + 4 bytes padding + 8 bytes pointer = 16 bytes.
*/


/* LINK LIST
Mã nguồn C Hoàn chỉnh (Full Implementation)
C
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int data;
    struct Node *next;
};

// Yêu cầu OS cấp phát vùng nhớ Heap cho Node mới
struct Node* create_node(int new_data) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (new_node == NULL) {
        fprintf(stderr, "Lỗi: Hệ điều hành từ chối cấp phát bộ nhớ (Heap exhausted)!\n");
        exit(1);
    }
    new_node->data = new_data;
    new_node->next = NULL;
    return new_node;
}

// Hàm duyệt và in danh sách ra stdout
void print_list(struct Node *node) {
    printf("Danh sách hiện tại: ");
    while (node != NULL) {
        printf("%d -> ", node->data);
        node = node->next;
    }
    printf("NULL\n");
}

// Giải phóng toàn bộ danh sách trả về cho OS
void free_list(struct Node **head_ref) {
    struct Node *current = *head_ref;
    struct Node *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    *head_ref = NULL;
}


// Chèn Đầu (O(1))
void insert_at_head(struct Node **head_ref, int new_data) {
    struct Node *new_node = create_node(new_data);
    new_node->next = *head_ref; // Node mới trỏ vào node đầu cũ
    *head_ref = new_node;       // Cập nhật con trỏ gốc
    printf("[+] Đã chèn %d vào ĐẦU.\n", new_data);
}


// Chèn Giữa (Sau một giá trị key xác định) (O(n))
void insert_after_value(struct Node *head, int key, int new_data) {
    struct Node *temp = head;
    while (temp != NULL && temp->data != key) {
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("[-] Không tìm thấy giá trị %d để chèn %d vào sau.\n", key, new_data);
        return;
    }
    struct Node *new_node = create_node(new_data);
    new_node->next = temp->next; // Bắt buộc: Lưu địa chỉ phần sau
    temp->next = new_node;       // Ngắt phần trước trỏ vào node mới
    printf("[+] Đã chèn %d vào GIỮA (sau %d).\n", new_data, key);
}


// Chèn Cuối (O(n))
void insert_at_tail(struct Node **head_ref, int new_data) {
    struct Node *new_node = create_node(new_data);
    if (*head_ref == NULL) { // Nếu danh sách rỗng
        *head_ref = new_node;
        printf("[+] Đã chèn %d vào CUỐI (Node đầu tiên).\n", new_data);
        return;
    }
    struct Node *last = *head_ref;
    while (last->next != NULL) { // Quét tìm node cuối
        last = last->next;
    }
    last->next = new_node;
    printf("[+] Đã chèn %d vào CUỐI.\n", new_data);
}

// Xóa Đầu (O(1))
void delete_at_head(struct Node **head_ref) {
    if (*head_ref == NULL) {
        printf("[-] Danh sách rỗng, không thể xóa ĐẦU.\n");
        return;
    }
    struct Node *temp = *head_ref; // Lưu trữ địa chỉ node cần xóa
    *head_ref = temp->next;        // Dịch con trỏ gốc sang node số 2
    free(temp);                    // Trả bộ nhớ vật lý về cho OS
    printf("[-] Đã xóa node ĐẦU.\n");
}


// Xóa Giữa (Theo giá trị key) (O(n))
void delete_middle_by_value(struct Node **head_ref, int key) {
    struct Node *temp = *head_ref;
    struct Node *prev = NULL;

    if (temp == NULL) {
        printf("[-] Danh sách rỗng, không thể xóa %d.\n", key);
        return;
    }
    // Trường hợp key nằm ngay tại Head
    if (temp != NULL && temp->data == key) {
        *head_ref = temp->next;
        free(temp);
        printf("[-] Đã xóa node GIỮA có giá trị %d (nằm ở Head).\n", key);
        return;
    }
    // Quét tìm node chứa key và giữ lại node liền trước (prev)
    while (temp != NULL && temp->data != key) {
        prev = temp;
        temp = temp->next;
    }
    if (temp == NULL) {
        printf("[-] Không tìm thấy giá trị %d để xóa.\n", key);
        return;
    }
    // Bỏ qua node hiện tại để nối danh sách
    prev->next = temp->next; 
    free(temp); // Giải phóng vùng nhớ
    printf("[-] Đã xóa node GIỮA có giá trị %d.\n", key);
}


// Xóa Cuối (O(n))
void delete_at_tail(struct Node **head_ref) {
    if (*head_ref == NULL) {
        printf("[-] Danh sách rỗng, không thể xóa CUỐI.\n");
        return;
    }
    // Nếu danh sách chỉ có 1 node
    if ((*head_ref)->next == NULL) {
        free(*head_ref);
        *head_ref = NULL;
        printf("[-] Đã xóa node CUỐI (Node duy nhất).\n");
        return;
    }
    // Tìm node KẾ CUỐI
    struct Node *second_last = *head_ref;
    while (second_last->next->next != NULL) {
        second_last = second_last->next;
    }
    free(second_last->next);   // Xóa node cuối cùng
    second_last->next = NULL;  // Cập nhật node kế cuối thành node cuối
    printf("[-] Đã xóa node CUỐI.\n");
}

int main() {
    struct Node *head = NULL;

    printf("--- THỬ NGHIỆM CHÈN (INSERTION) ---\n");
    insert_at_head(&head, 20);      // 20
    insert_at_head(&head, 10);      // 10 -> 20
    insert_at_tail(&head, 40);      // 10 -> 20 -> 40
    insert_at_tail(&head, 50);      // 10 -> 20 -> 40 -> 50
    insert_after_value(head, 20, 30); // 10 -> 20 -> 30 -> 40 -> 50
    print_list(head);

    printf("\n--- THỬ NGHIỆM XÓA (DELETION) ---\n");
    delete_at_head(&head);          // Xóa 10
    print_list(head);

    delete_at_tail(&head);          // Xóa 50
    print_list(head);

    delete_middle_by_value(&head, 30); // Xóa 30
    print_list(head);

    printf("\n--- DỌN DẸP BỘ NHỚ TRƯỚC KHI THOÁT ---\n");
    free_list(&head);
    print_list(head);

    return 0;
}
2. Tổng kết Nguyên lý Kỹ thuật cốt lõi
Khi bạn nhìn vào toàn bộ cấu trúc mã này, hãy chú ý đến 3 quy tắc bất biến trong lập trình C cấp thấp:

Thay đổi cấu trúc bắt buộc dùng Con trỏ cấp 2: 
Bất kỳ hàm nào có khả năng làm thay đổi địa chỉ của node đầu tiên (Chèn đầu, Chèn cuối khi mảng rỗng, Xóa đầu, Xóa giữa nếu key ở đầu, Xóa cuối khi mảng có 1 phần tử)
 đều bắt buộc phải nhận tham số struct Node **head_ref. 
 Nếu chỉ truyền con trỏ cấp 1, bạn đang thao tác trên một bản sao của địa chỉ.

Định tuyến trước, Ngắt kết nối sau: 
Trong các hàm insert_after_value và delete_middle_by_value, bạn phải luôn thao tác với con trỏ của phần tử phía sau trước. 
Nếu bạn làm mất địa chỉ của temp->next, phần còn lại của bộ nhớ trên Heap sẽ vĩnh viễn không thể truy cập được.

Vòng lặp nhìn trước 2 bước (Look-ahead Traversal): 
Trong hàm delete_at_tail, việc quét last->next != NULL là không đủ vì bạn sẽ dừng lại ở node cuối và không thể gỡ bỏ liên kết từ node kế cuối. 
Bắt buộc phải dùng second_last->next->next != NULL để tiến trình dừng lại ở vị trí áp chót, từ đó mới có thể can thiệp vào liên kết phía sau nó.
*/


/* STACK
Triển khai Stack bằng Mảng động (Array-based Stack)
Cách tiếp cận này yêu cầu cấp phát một khối RAM liên tục. Biến top đóng vai trò là con trỏ logic (offset) để truy xuất mảng.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct ArrayStack {
    int top;
    int capacity;
    int *array;
};

struct ArrayStack* create_array_stack(int capacity) {
    struct ArrayStack *stack = (struct ArrayStack *)malloc(sizeof(struct ArrayStack));
    if (!stack) return NULL;
    stack->capacity = capacity;
    stack->top = -1; 
    stack->array = (int *)malloc(stack->capacity * sizeof(int));
    return stack;
}

void array_push(struct ArrayStack *stack, int item) {
    if (stack->top == stack->capacity - 1) {
        printf("[-] Lỗi: Stack Overflow (Ngăn xếp đã đầy, sức chứa: %d)!\n", stack->capacity);
        return;
    }
    stack->array[++stack->top] = item;
    printf("[+] Push: %d\n", item);
}

int array_pop(struct ArrayStack *stack) {
    if (stack->top == -1) {
        printf("[-] Lỗi: Stack Underflow (Ngăn xếp rỗng)!\n");
        return INT_MIN;
    }
    return stack->array[stack->top--]; 
}

int array_peek(struct ArrayStack *stack) {
    if (stack->top == -1) return INT_MIN;
    return stack->array[stack->top];
}

void free_array_stack(struct ArrayStack *stack) {
    if (stack) {
        free(stack->array);
        free(stack);
    }
}

int main() {
    printf("=== KIỂM THỬ STACK BẰNG MẢNG ĐỘNG ===\n");
    
    // Cấp phát Stack với sức chứa giới hạn là 3 phần tử
    struct ArrayStack *stack = create_array_stack(3);

    // 1. Kiểm thử đẩy dữ liệu và vi phạm ranh giới bộ nhớ (Overflow)
    array_push(stack, 10);
    array_push(stack, 20);
    array_push(stack, 30);
    array_push(stack, 40); // Hành vi này sẽ bị chặn lại do kiểm tra capacity

    // 2. Đọc đỉnh Stack
    printf("Phần tử đỉnh hiện tại (Peek): %d\n", array_peek(stack));

    // 3. Rút dữ liệu (LIFO)
    printf("[-] Pop: %d\n", array_pop(stack)); // Rút 30
    printf("[-] Pop: %d\n", array_pop(stack)); // Rút 20

    // 4. Kiểm tra trạng thái sau khi rút
    printf("Phần tử đỉnh sau khi Pop (Peek): %d\n", array_peek(stack)); // Còn lại 10

    // 5. Trả bộ nhớ về cho hệ điều hành
    free_array_stack(stack);
    printf("Đã dọn dẹp Heap an toàn.\n");

    return 0;
}
Đặc điểm Kỹ thuật (Array Stack):

Hiệu năng CPU (Ưu điểm lớn nhất): Tốc độ thực thi tuyệt đối. 
Do mảng là một khối nhớ liên tục, toàn bộ Stack sẽ được nạp vào bộ nhớ đệm (L1/L2 Cache) của CPU. Không có độ trễ tìm kiếm địa chỉ (Cache Hit).

Quản lý giới hạn (Nhược điểm): Phải khai báo capacity từ đầu. 
Nếu cấu trúc dữ liệu phình to vượt giới hạn, bạn phải gọi lệnh realloc để OS tìm một vùng nhớ mới lớn hơn và sao chép toàn bộ dữ liệu sang,
gây sụt giảm hiệu năng đột ngột (Performance Spike).


2. Triển khai Stack bằng Danh sách liên kết (Linked List-based Stack)
Cách tiếp cận này không dùng mảng tĩnh. Mỗi lần gọi lệnh push, hệ thống sẽ yêu cầu hệ điều hành cấp phát một khối nhớ nhỏ (Node) trên phân vùng Heap. 
Bản chất của push chính là lệnh insert_at_head, và pop chính là delete_at_head.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Node {
    int data;
    struct Node *next;
};

void list_push(struct Node **top_ref, int new_data) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (!new_node) {
        printf("[-] Lỗi: Cạn kiệt bộ nhớ Heap!\n");
        return;
    }
    new_node->data = new_data;
    new_node->next = *top_ref; 
    *top_ref = new_node;       
    printf("[+] Push: %d\n", new_data);
}

int list_pop(struct Node **top_ref) {
    if (*top_ref == NULL) {
        printf("[-] Lỗi: Stack Underflow (Ngăn xếp rỗng)!\n");
        return INT_MIN;
    }
    struct Node *temp = *top_ref; 
    int popped_data = temp->data; 
    
    *top_ref = temp->next;        
    free(temp);                   
    return popped_data;
}

int list_peek(struct Node *top) {
    if (top == NULL) return INT_MIN;
    return top->data;
}

void free_list_stack(struct Node **top_ref) {
    struct Node *current = *top_ref;
    struct Node *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    *top_ref = NULL;
}

int main() {
    printf("=== KIỂM THỬ STACK BẰNG DANH SÁCH LIÊN KẾT ===\n");
    
    // Con trỏ quản lý đỉnh Stack. Khởi tạo bằng NULL (Rỗng)
    struct Node *top = NULL;

    // 1. Kiểm thử chèn dữ liệu. Sẽ không có lỗi Overflow vì cấp phát động từng Node
    list_push(&top, 100);
    list_push(&top, 200);
    list_push(&top, 300);
    list_push(&top, 400); // Hoàn toàn hợp lệ

    // 2. Đọc đỉnh Stack
    printf("Phần tử đỉnh hiện tại (Peek): %d\n", list_peek(top)); // Kết quả: 400

    // 3. Rút dữ liệu (LIFO) và giải phóng vật lý vùng nhớ
    printf("[-] Pop: %d\n", list_pop(&top)); // Rút 400
    printf("[-] Pop: %d\n", list_pop(&top)); // Rút 300

    // 4. Kiểm tra trạng thái sau khi rút
    printf("Phần tử đỉnh sau khi Pop (Peek): %d\n", list_peek(top)); // Còn lại 200

    // 5. Kiểm thử rút cạn Stack (Underflow)
    list_pop(&top); // Rút 200
    list_pop(&top); // Rút 100
    list_pop(&top); // Cố tình rút từ Stack rỗng -> Kích hoạt cảnh báo Underflow

    // 6. Dọn dẹp tài nguyên
    // (Lúc này top đã là NULL do đã rút hết, nhưng gọi hàm free vẫn an toàn)
    free_list_stack(&top);
    printf("Đã dọn dẹp Heap an toàn.\n");

    return 0;
}

Đặc điểm Kỹ thuật (Linked List Stack):

Không giới hạn dung lượng (Ưu điểm lớn nhất): Kích thước Stack mở rộng vô hạn (chỉ dừng lại khi hết RAM vật lý). 
Không bao giờ xảy ra lỗi Stack Overflow do logic chương trình.

Chi phí ẩn (Overhead): Mỗi phần tử tốn thêm 8 bytes (trên máy 64-bit) để lưu con trỏ next. 
Ngoài ra, việc gọi malloc và free liên tục cho từng phần tử sẽ làm phân mảnh vùng Heap (Heap Fragmentation) và làm mất tính cục bộ của bộ nhớ đệm (Cache Miss), 
khiến tốc độ thực thi tổng thể chậm hơn so với dùng mảng.
*/


/* QUEUE
1. Triển khai bằng Mảng động: Hàng đợi vòng (Circular Queue)
Nếu dùng mảng tuyến tính thông thường, khi bạn nạp (enqueue) và rút (dequeue) liên tục, con trỏ front và rear sẽ cùng tiến về cuối mảng. 
Dù các ô nhớ ở đầu mảng đã bị rút trống, hệ thống vẫn báo mảng đầy.
Giải pháp vật lý là sử dụng phép toán chia lấy dư (%) để nối liền điểm đầu và điểm cuối của mảng, tạo thành một Bộ đệm vòng (Ring Buffer).


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct CircularQueue {
    int front;    // Chỉ số của phần tử chờ được rút ra (Dequeue)
    int rear;     // Chỉ số của phần tử vừa được nạp vào (Enqueue)
    int size;     // Số lượng phần tử hiện tại đang có trong Queue
    int capacity; // Sức chứa vật lý tối đa của mảng
    int *array;   // Khối nhớ tuyến tính trên Heap
};

struct CircularQueue* create_queue(int capacity) {
    struct CircularQueue *queue = (struct CircularQueue *)malloc(sizeof(struct CircularQueue));
    if (!queue) return NULL;
    
    queue->capacity = capacity;
    queue->front = 0;           // Bắt đầu đọc từ ô số 0
    queue->size = 0;            // Ban đầu chưa có dữ liệu
    queue->rear = capacity - 1; // Khởi tạo rear ở cuối mảng để khi enqueue lần đầu vòng lại ô số 0
    
    queue->array = (int *)malloc(queue->capacity * sizeof(int));
    return queue;
}


// Kiểm tra trạng thái
int is_full(struct CircularQueue *queue) {
    return (queue->size == queue->capacity);
}

int is_empty(struct CircularQueue *queue) {
    return (queue->size == 0);
}

// ENQUEUE: Nạp dữ liệu vào cuối (Rear)
void enqueue(struct CircularQueue *queue, int item) {
    if (is_full(queue)) {
        printf("[-] Lỗi: Queue Overflow (Hàng đợi đầy)!\n");
        return;
    }
    // Thuật toán quay vòng: Nếu rear chạm đáy mảng, % capacity sẽ ép nó về 0
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->array[queue->rear] = item;
    queue->size++;
    printf("[Array] Enqueue: %d\n", item);
}

// DEQUEUE: Rút dữ liệu từ đầu (Front)
int dequeue(struct CircularQueue *queue) {
    if (is_empty(queue)) {
        printf("[-] Lỗi: Queue Underflow (Hàng đợi rỗng)!\n");
        return INT_MIN;
    }
    int item = queue->array[queue->front];
    // Thuật toán quay vòng cho front
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;
    return item;
}

void free_queue(struct CircularQueue *queue) {
    if (queue) {
        free(queue->array);
        free(queue);
    }
}

int main() {
    printf("=== KIỂM THỬ HÀNG ĐỢI VÒNG (CIRCULAR ARRAY) ===\n");
    struct CircularQueue *queue = create_queue(3);

    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);
    enqueue(queue, 40); // Bị chặn do tràn (Overflow)

    printf("[-] Dequeue: %d\n", dequeue(queue)); // Rút 10. Ô số 0 trống.
    
    // Nạp thêm 50. Thuật toán % sẽ nạp 50 vào chính ô số 0 vừa trống.
    enqueue(queue, 50); 
    
    printf("[-] Dequeue: %d\n", dequeue(queue)); // Rút 20
    printf("[-] Dequeue: %d\n", dequeue(queue)); // Rút 30
    printf("[-] Dequeue: %d\n", dequeue(queue)); // Rút 50

    free_queue(queue);
    return 0;
}

Phân tích Kỹ thuật Cấp thấp:Cơ chế (index + 1) % capacity là kỹ thuật thao tác số học con trỏ kinh điển. 
Nó giải quyết triệt để sự lãng phí không gian của mảng tĩnh mà không cần phải thực hiện vòng lặp dịch chuyển mảng tốn thời gian O(n).
Do sử dụng mảng tĩnh, cấu trúc này bảo toàn được đặc tính Cache Locality (tính cục bộ bộ nhớ đệm) rất tốt, CPU không bị trễ do tìm kiếm địa chỉ rời rạc. 
Đây chính là cấu trúc của các Ring Buffer trong driver card mạng máy tính (NIC).

2. Triển khai bằng Danh sách liên kết (Linked List Queue)
Để Queue bằng Linked List đạt được hiệu năng O(1) cho cả thao tác nạp và rút, 
chúng ta bắt buộc phải duy trì hai con trỏ song song: front (trỏ vào phần tử đầu để rút) và rear (trỏ vào phần tử cuối để nạp). 
Nếu không có con trỏ rear, mỗi lần Enqueue bạn sẽ mất thời gian $O(n)$ để quét đến cuối mảng.

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

struct Node {
    int data;
    struct Node *next;
};

// Cấu trúc gói gọn 2 con trỏ quản lý Queue
struct LinkedListQueue {
    struct Node *front;
    struct Node *rear;
};

struct LinkedListQueue* create_list_queue() {
    struct LinkedListQueue *queue = (struct LinkedListQueue *)malloc(sizeof(struct LinkedListQueue));
    if (!queue) return NULL;
    queue->front = queue->rear = NULL; // Ban đầu hàng đợi rỗng
    return queue;
}

struct Node* create_node(int new_data) {
    struct Node *new_node = (struct Node *)malloc(sizeof(struct Node));
    if (!new_node) exit(1);
    new_node->data = new_data;
    new_node->next = NULL;
    return new_node;
}


// ENQUEUE: Thêm vào cuối (Tail)
void list_enqueue(struct LinkedListQueue *queue, int item) {
    struct Node *new_node = create_node(item);

    // Nếu Queue rỗng, node mới vừa là front vừa là rear
    if (queue->rear == NULL) {
        queue->front = queue->rear = new_node;
        printf("[List] Enqueue: %d\n", item);
        return;
    }

    // Liên kết node mới vào phía sau rear hiện tại, rồi dịch rear lên
    queue->rear->next = new_node;
    queue->rear = new_node;
    printf("[List] Enqueue: %d\n", item);
}

// DEQUEUE: Rút từ đầu (Head)
int list_dequeue(struct LinkedListQueue *queue) {
    if (queue->front == NULL) {
        printf("[-] Lỗi: Queue Underflow!\n");
        return INT_MIN;
    }

    struct Node *temp = queue->front;
    int dequeued_data = temp->data;

    // Dịch con trỏ front sang node kế tiếp
    queue->front = queue->front->next;

    // Trường hợp biên: Nếu rút xong mà Queue rỗng, phải set rear về NULL
    if (queue->front == NULL) {
        queue->rear = NULL;
    }

    free(temp); // Giải phóng vật lý
    return dequeued_data;
}

void free_list_queue(struct LinkedListQueue *queue) {
    struct Node *current = queue->front;
    struct Node *next_node;
    while (current != NULL) {
        next_node = current->next;
        free(current);
        current = next_node;
    }
    free(queue); // Giải phóng cấu trúc quản lý
}

int main() {
    printf("=== KIỂM THỬ HÀNG ĐỢI LIÊN KẾT (LINKED LIST) ===\n");
    struct LinkedListQueue *queue = create_list_queue();

    list_enqueue(queue, 100);
    list_enqueue(queue, 200);
    list_enqueue(queue, 300);

    printf("[-] Dequeue: %d\n", list_dequeue(queue)); // Rút 100
    printf("[-] Dequeue: %d\n", list_dequeue(queue)); // Rút 200

    list_enqueue(queue, 400); // 300 -> 400
    
    printf("[-] Dequeue: %d\n", list_dequeue(queue)); // Rút 300
    printf("[-] Dequeue: %d\n", list_dequeue(queue)); // Rút 400
    
    // Thử rút khi rỗng
    list_dequeue(queue); 

    free_list_queue(queue);
    return 0;
}
Phân tích Kỹ thuật Cấp thấp: Thiết kế Cấu trúc lồng (Nested Structs): 
Khác với Linked List thông thường nơi hàm main giữ con trỏ head, ở đây chúng ta tạo một struct LinkedListQueue lưu trữ cả front và rear. 
Thiết kế này cho phép truyền tham chiếu toàn bộ cấu trúc Queue vào các hàm chỉ với một con trỏ duy nhất (struct LinkedListQueue *queue), 
loại bỏ hoàn toàn sự cồng kềnh của Con trỏ cấp 2 (**).
Đồng bộ hóa con trỏ biên: Lỗ hổng thường gặp nhất khi lập trình Queue bằng Liên kết là quên cập nhật rear = NULL khi thực hiện Dequeue cái Node cuối cùng. 
Nếu không có lệnh kiểm tra if (queue->front == NULL), biến rear sẽ trở thành con trỏ lơ lửng (Dangling Pointer) mang địa chỉ của khối RAM vừa bị free(). 
Lần Enqueue tiếp theo sẽ lập tức ghi dữ liệu vào vùng cấm và gây lỗi bộ nhớ.
*/


/* MAX HEAP
 - Phân vùng nhớ Heap (Heap Memory Segment): Là vùng bộ nhớ vật lý tuyến tính do hệ điều hành quản lý, nơi các hàm malloc, calloc xin cấp phát.
Chúng ta đã thao tác với nó trong suốt các phần trước.
 - Cấu trúc dữ liệu Heap (Heap Data Structure): Là một mô hình tổ chức dữ liệu logic ở dạng Cây nhị phân hoàn chỉnh (Complete Binary Tree), 
tuân thủ nghiêm ngặt Thuộc tính Heap (Heap Property).
Trong thiết kế hệ điều hành và hệ thống nhúng, cấu trúc dữ liệu Heap là cơ sở hạ tầng bắt buộc để triển khai Hàng đợi ưu tiên (Priority Queue), 
phục vụ cho bộ lập lịch tiến trình (Task Scheduler) hoặc xử lý ngắt cứng (Hardware Interrupts), 
nơi các luồng dữ liệu không được xử lý theo kiểu FIFO truyền thống mà dựa trên mức độ ưu tiên.


1. Ánh xạ Cây phi tuyến tính vào Mảng tuyến tính 
Khác với Cây nhị phân tìm kiếm (BST) thường dùng các con trỏ left và right trỏ đến các khối nhớ phân tán trên bộ nhớ, 
Heap luôn là một Cây nhị phân hoàn chỉnh. Điều này có nghĩa là mọi cấp của cây đều được lấp đầy từ trái sang phải.
Nhờ đặc tính vật lý này, chúng ta không cần dùng con trỏ. Toàn bộ cấu trúc cây 2D được "ép phẳng" (flatten) vào một mảng 1D liên tục trên RAM. 
Hệ thống định tuyến các nút (nodes) thông qua đại số chỉ số (Index Arithmetic).
Tại nút ở chỉ số i:Nút cha (Parent): Nằm tại chỉ số (i - 1) / 2.
Nút con trái (Left Child): Nằm tại chỉ số 2 * i + 1.
Nút con phải (Right Child): Nằm tại chỉ số 2 * i + 2.
Lưu ý tối ưu phần cứng: Trình biên dịch C thường dịch các phép nhân/chia cho 2 này thành các toán tử dịch bit (>> 1 và << 1) để tiết kiệm chu kỳ xung nhịp CPU.

2. Triển khai Mã nguồn C (Max-Heap)

#include <stdio.h>
#include <stdlib.h>

struct MaxHeap {
    int *array;    // Con trỏ quản lý khối nhớ liên tục
    int capacity;  // Sức chứa tối đa của mảng
    int size;      // Số lượng phần tử hiện tại
};

struct MaxHeap* create_heap(int capacity) {
    struct MaxHeap *heap = (struct MaxHeap *)malloc(sizeof(struct MaxHeap));
    if (!heap) exit(1);
    
    heap->capacity = capacity;
    heap->size = 0;
    heap->array = (int *)malloc(heap->capacity * sizeof(int));
    if (!heap->array) exit(1);
    
    return heap;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Sử dụng khi chèn phần tử mới vào cuối mảng để duy trì tính chất Max-Heap
void heapify_up(struct MaxHeap *heap, int index) {
    // Tính toán chỉ số nút cha bằng thao tác dịch bit: (index - 1) / 2
    int parent = (index - 1) >> 1;

    // Vòng lặp đẩy giá trị lớn lên trên, dừng lại khi chạm gốc (index == 0)
    // hoặc khi nút hiện tại đã nhỏ hơn nút cha
    while (index > 0 && heap->array[index] > heap->array[parent]) {
        swap(&heap->array[index], &heap->array[parent]);
        index = parent; // Cập nhật vị trí hiện tại
        parent = (index - 1) >> 1; // Tính lại nút cha mới
    }
}

void insert_heap(struct MaxHeap *heap, int data) {
    if (heap->size == heap->capacity) {
        printf("[-] Lỗi: Heap Overflow.\n");
        return;
    }
    
    // Bước 1: Đặt phần tử mới vào ô trống cuối cùng của mảng
    int index = heap->size;
    heap->array[index] = data;
    heap->size++;
    
    // Bước 2: Tái cân bằng Heap từ dưới lên
    heapify_up(heap, index);
    printf("[+] Đã chèn %d vào Heap.\n", data);
}

// Sử dụng sau khi rút phần tử gốc để duy trì tính chất Max-Heap
void heapify_down(struct MaxHeap *heap, int index) {
    int largest = index;
    int left = (index << 1) + 1;  // 2*index + 1
    int right = (index << 1) + 2; // 2*index + 2

    // Kiểm tra ranh giới mảng và so sánh với con trái
    if (left < heap->size && heap->array[left] > heap->array[largest]) {
        largest = left;
    }
    // Kiểm tra ranh giới mảng và so sánh với con phải
    if (right < heap->size && heap->array[right] > heap->array[largest]) {
        largest = right;
    }

    // Nếu nút cha không phải là giá trị lớn nhất, thực hiện hoán đổi
    if (largest != index) {
        swap(&heap->array[index], &heap->array[largest]);
        // Đệ quy đẩy tiếp xuống các cấp thấp hơn
        heapify_down(heap, largest);
    }
}

int extract_max(struct MaxHeap *heap) {
    if (heap->size <= 0) {
        printf("[-] Lỗi: Heap Underflow.\n");
        return -1; 
    }
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }

    // Bước 1: Lưu trữ giá trị gốc (Max) để trả về
    int root = heap->array[0];

    // Bước 2: Lấy phần tử cuối cùng của mảng đè lên vị trí gốc, sau đó giảm kích thước
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    // Bước 3: Tái cân bằng Heap từ trên xuống
    heapify_down(heap, 0);

    return root;
}

// Giải phóng bộ nhớ
void free_heap(struct MaxHeap *heap) {
    if (heap) {
        free(heap->array);
        free(heap);
    }
}

int main() {
    printf("=== KIỂM THỬ MAX-HEAP CẤP PHÁT ĐỘNG ===\n");
    struct MaxHeap *heap = create_heap(10);

    insert_heap(heap, 15);
    insert_heap(heap, 20); // 20 sẽ nổi lên gốc
    insert_heap(heap, 10);
    insert_heap(heap, 40); // 40 sẽ nổi lên gốc
    insert_heap(heap, 50); // 50 sẽ nổi lên gốc

    printf("\n--- KIỂM TRA QUY TRÌNH RÚT DỮ LIỆU ĐÃ ĐƯỢC ƯU TIÊN ---\n");
    // Phải luôn rút ra được phần tử lớn nhất hiện tại
    printf("Extract Max: %d\n", extract_max(heap)); // Kết quả: 50
    printf("Extract Max: %d\n", extract_max(heap)); // Kết quả: 40
    printf("Extract Max: %d\n", extract_max(heap)); // Kết quả: 20

    free_heap(heap);
    return 0;
}


3. Phân tích Kỹ thuật Cấp thấp và Tối ưu hóa
Độ phức tạp O(log n) và Hoán vị tại chỗ (In-place Mutation): Cấu trúc cây có chiều cao là log_2 n. 
Do đó, quá trình heapify_up hoặc heapify_down tốn tối đa log_2 n phép so sánh. 
Khác với Danh sách liên kết phải gọi malloc hoặc free cho mỗi lần chèn/xóa, thuật toán Heap trên mảng thực hiện thay đổi dữ liệu 
hoàn toàn tại chỗ (in-place) bằng cách hoán đổi giá trị (swap) của các khối nhớ có sẵn, loại bỏ hoàn toàn chi phí tương tác với Nhân hệ điều hành (Kernel).
Tính cục bộ của bộ nhớ đệm (Cache Locality): Khác biệt lớn nhất giữa Heap và BST (Cây nhị phân tìm kiếm dùng con trỏ) nằm ở kiến trúc phần cứng. 
Trong heapify_up, CPU thực thi vòng lặp truy cập các chỉ số index và parent. 
Vì toàn bộ cây nằm trên một mảng tuyến tính liền kề, bộ điều khiển Cache của CPU sẽ tự động nạp trước (prefetch) các khối dữ liệu vào L1 Cache. 
Điều này làm cho tốc độ thực thi thực tế của thuật toán Heap mảng nhanh hơn đáng kể so với cây con trỏ truyền thống.

Quy trình thay thế rễ (Extract Root): 
Kỹ thuật chuyển phần tử cuối cùng heap->array[heap->size - 1] lên làm gốc gốc là một thao tác toán học hoàn hảo để bảo toàn cấu trúc Cây nhị phân hoàn chỉnh. 
Nó ngăn chặn sự xuất hiện của bất kỳ "lỗ hổng" vật lý nào bên trong khối mảng, đảm bảo tính liên tục của dữ liệu.
*/


/* MIN HEAP
Hệ quả vật lý của thuộc tính này là: Phần tử tại chỉ số 0 (Gốc của cây) luôn chứa giá trị nhỏ nhất trong toàn bộ tập dữ liệu.
Dưới đây là mã nguồn hoàn chỉnh cho Min-Heap. Sự khác biệt logic chỉ nằm ở toán tử so sánh (< thay vì >) trong hai hàm heapify_up và heapify_down.

1. Triển khai Mã nguồn C (Min-Heap)

#include <stdio.h>
#include <stdlib.h>

struct MinHeap {
    int *array;    // Con trỏ quản lý khối nhớ tuyến tính trên Heap
    int capacity;  // Giới hạn phần tử tối đa
    int size;      // Số lượng phần tử hiện hành
};

struct MinHeap* create_min_heap(int capacity) {
    struct MinHeap *heap = (struct MinHeap *)malloc(sizeof(struct MinHeap));
    if (!heap) exit(1);
    
    heap->capacity = capacity;
    heap->size = 0;
    heap->array = (int *)malloc(heap->capacity * sizeof(int));
    if (!heap->array) {
        free(heap);
        exit(1);
    }
    
    return heap;
}

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_up(struct MinHeap *heap, int index) {
    int parent = (index - 1) >> 1; // Tương đương (index - 1) / 2

    // Vòng lặp đẩy giá trị NHỎ lên trên
    // Điều kiện: index > 0 VÀ giá trị nút hiện tại NHỎ HƠN giá trị nút cha
    while (index > 0 && heap->array[index] < heap->array[parent]) {
        swap(&heap->array[index], &heap->array[parent]);
        index = parent;
        parent = (index - 1) >> 1;
    }
}

// O(logn)
void insert_min_heap(struct MinHeap *heap, int data) {
    if (heap->size == heap->capacity) {
        printf("[-] Lỗi: Tràn Heap (Overflow).\n");
        return;
    }
    
    // Ghi dữ liệu vào cuối mảng
    int index = heap->size;
    heap->array[index] = data;
    heap->size++;
    
    // Khôi phục tính chất Min-Heap từ dưới lên
    heapify_up(heap, index);
    printf("[+] Đã chèn %d vào Min-Heap.\n", data);
}

void heapify_down(struct MinHeap *heap, int index) {
    int smallest = index;
    int left = (index << 1) + 1;  // Con trái
    int right = (index << 1) + 2; // Con phải

    // Tìm giá trị NHỎ NHẤT giữa nút cha và 2 nút con
    if (left < heap->size && heap->array[left] < heap->array[smallest]) {
        smallest = left;
    }
    if (right < heap->size && heap->array[right] < heap->array[smallest]) {
        smallest = right;
    }

    // Nếu nút cha không phải là nhỏ nhất, hoán vị với nút con nhỏ nhất
    if (smallest != index) {
        swap(&heap->array[index], &heap->array[smallest]);
        // Tiếp tục đệ quy xuống dưới
        heapify_down(heap, smallest);
    }
}

int extract_min(struct MinHeap *heap) {
    if (heap->size <= 0) {
        printf("[-] Lỗi: Cạn Heap (Underflow).\n");
        return -1; 
    }
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }

    int root = heap->array[0]; // Lưu giá trị nhỏ nhất (nằm ở Gốc)

    // Đưa phần tử cuối cùng lên làm Gốc
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;

    // Khôi phục tính chất Min-Heap từ trên xuống
    heapify_down(heap, 0);

    return root;
}

// Giải phóng tài nguyên
void free_min_heap(struct MinHeap *heap) {
    if (heap) {
        free(heap->array);
        free(heap);
    }
}

int main() {
    printf("=== KIỂM THỬ MIN-HEAP CẤP PHÁT ĐỘNG ===\n");
    struct MinHeap *heap = create_min_heap(10);

    insert_min_heap(heap, 40);
    insert_min_heap(heap, 10); // 10 sẽ trồi lên gốc
    insert_min_heap(heap, 30);
    insert_min_heap(heap, 50);
    insert_min_heap(heap, 5);  // 5 sẽ trồi lên gốc

    printf("\n--- TRÍCH XUẤT DỮ LIỆU TỪ MIN-HEAP ---\n");
    // Kết quả in ra bắt buộc phải theo thứ tự tăng dần
    printf("Extract Min: %d\n", extract_min(heap)); // Kết quả: 5
    printf("Extract Min: %d\n", extract_min(heap)); // Kết quả: 10
    printf("Extract Min: %d\n", extract_min(heap)); // Kết quả: 30
    printf("Extract Min: %d\n", extract_min(heap)); // Kết quả: 40
    printf("Extract Min: %d\n", extract_min(heap)); // Kết quả: 50

    free_min_heap(heap);
    return 0;
}
2. Tầm quan trọng của Min-Heap trong Lập trình Hệ thống
Mặc dù mã nguồn của Min-Heap và Max-Heap gần như giống hệt nhau, nhưng ứng dụng thực tế của chúng trong hệ điều hành và mạng máy tính lại hoàn toàn khác biệt:

Xử lý Bộ hẹn giờ Hệ thống (OS Timers): Trong nhân Linux, hệ thống cần duy trì hàng triệu bộ hẹn giờ (timers) ngắt (interrupts). 
Kernel sử dụng Min-Heap để lưu trữ các bộ hẹn giờ này. Giá trị (key) là thời gian kích hoạt. 
Khi đó, bộ hẹn giờ cần kích hoạt sớm nhất (giá trị thời gian nhỏ nhất) sẽ luôn nằm ở chỉ số array[0], cho phép CPU trích xuất và thực thi ngắt với độ trễ tối thiểu (O(1) để đọc, O(log n) để cập nhật).

Định tuyến Mạng (Network Routing): Các thuật toán tìm đường đi ngắn nhất như Dijkstra sử dụng Min-Heap làm Hàng đợi ưu tiên (Priority Queue). 
Tại mỗi bước tiến của thuật toán, bộ định tuyến (Router) phải luôn chọn ra con đường có độ trễ (latency) hoặc chi phí (cost) vật lý thấp nhất để duyệt tiếp. 
Việc dùng mảng thường tốn O(n) để quét tìm giá trị nhỏ nhất, trong khi Min-Heap giảm chu kỳ xử lý này xuống chỉ còn O(log n).

Quản lý Băng thông (Bandwidth Management): 
Triển khai thuật toán nén dữ liệu cấp thấp như Huffman Coding yêu cầu phải liên tục nhóm 2 ký tự có tần suất xuất hiện thấp nhất lại với nhau. 
Việc duy trì một Min-Heap đảm bảo luồng đọc dữ liệu từ tệp tin không bị thắt cổ chai (bottleneck).
*/




