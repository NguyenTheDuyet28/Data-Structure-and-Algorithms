/*1. Bubble Sort (Nổi bọt) - Chỉ dùng Mảng
Bản chất là hoán đổi liên tục 2 phần tử kề nhau. Tận dụng Cache cực tốt nhưng tốn chu kỳ CPU.

void bubble_sort_array(int arr[], int n) {
    int i, j, temp;
    int swapped; // Tối ưu: Dừng sớm nếu mảng đã có thứ tự
    for (i = 0; i < n - 1; i++) {
        swapped = 0;
        for (j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                // Hoán vị bộ nhớ
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
                swapped = 1;
            }
        }
        if (swapped == 0) break;
    }
}
*/

/*2. Selection Sort (Chọn) - Chỉ dùng Mảng
Quét toàn bộ mảng để tìm khối nhớ chứa giá trị nhỏ nhất, sau đó ném nó lên đầu.

void selection_sort_array(int arr[], int n) {
    int i, j, min_idx, temp;
    for (i = 0; i < n - 1; i++) {
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j; // Ghi nhớ chỉ số vật lý của giá trị nhỏ nhất
            }
        }
        // Hoán vị 1 lần duy nhất mỗi vòng lặp (Tối ưu hơn Bubble)
        if (min_idx != i) {
            temp = arr[min_idx];
            arr[min_idx] = arr[i];
            arr[i] = temp;
        }
    }
}
*/

/*3. Insertion Sort (Chèn) - Tuyệt vời trên CẢ HAI
Đây là thuật toán O(n^2) mạnh nhất.

Phiên bản Mảng (Memory Shift):

void insertion_sort_array(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        // Kéo các ô nhớ lùi về sau để chừa chỗ trống
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key; // Chèn dữ liệu vào khoảng trống vật lý
    }
}

Phiên bản Linked List (Pointer Rewiring): Tối ưu hơn mảng vì không cần dịch chuyển dữ liệu vật lý, 
chỉ cần rút con trỏ ra và cắm lại (O(1) thao tác vật lý).

struct Node { int data; struct Node *next; };
void insertion_sort_list(struct Node **head_ref) {
    struct Node *sorted = NULL; // Danh sách mới
    struct Node *current = *head_ref;

    while (current != NULL) {
        struct Node *next = current->next; // Giữ lại phần còn lại của danh sách
        
        // Logic chèn vào danh sách đã sắp xếp (sorted)
        if (sorted == NULL || sorted->data >= current->data) {
            current->next = sorted;
            sorted = current;
        } else {
            struct Node *temp = sorted;
            while (temp->next != NULL && temp->next->data < current->data) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }
        current = next;
    }
    *head_ref = sorted;
}
*/

/*4. Quick Sort - VUA CỦA MẢNG (Chỉ dùng Mảng)
Không tốn RAM phụ trợ (In-place). Quét mảng bằng 2 con trỏ chạy ngược chiều. 
Áp dụng lên Linked List là ác mộng vì không có khả năng truy cập ngẫu nhiên.

void swap(int* a, int* b) { int t = *a; *a = *b; *b = t; }

// Cơ chế Phân vùng (Lomuto Partition)
int partition(int arr[], int low, int high) {
    int pivot = arr[high]; // Chọn phần tử cuối làm Trục
    int i = (low - 1);     // Con trỏ theo dõi ranh giới nhỏ hơn pivot

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]); // Đưa Trục vào đúng vị trí
    return (i + 1);
}

void quick_sort_array(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_array(arr, low, pi - 1);
        quick_sort_array(arr, pi + 1, high);
    }
}
*/

/*5. Heap Sort - Chỉ dùng Mảng
Lợi dụng tính chất đại số chỉ số (2*i+1) của Mảng để mô phỏng Cây Nhị Phân mà không tốn byte con trỏ nào.

void heapify(int arr[], int n, int i) {
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left] > arr[largest]) largest = left;
    if (right < n && arr[right] > arr[largest]) largest = right;

    if (largest != i) {
        swap(&arr[i], &arr[largest]);
        heapify(arr, n, largest);
    }
}

void heap_sort_array(int arr[], int n) {
    // 1. Xây dựng Max-Heap từ dưới lên
    for (int i = n / 2 - 1; i >= 0; i--) heapify(arr, n, i);

    // 2. Rút Max đưa về cuối mảng
    for (int i = n - 1; i > 0; i--) {
        swap(&arr[0], &arr[i]);
        heapify(arr, i, 0); // Vun đống lại phần còn lại
    }
}
*/

/*6. Merge Sort - Dùng được CẢ HAI (Nhưng là Vua của Linked List)
Trên mảng, thuật toán này tốn thêm O(n) RAM để tạo mảng tạm (Auxiliary Array). 
Trên Linked List, nó chỉ tốn O(1) RAM vì chỉ việc đổi hướng con trỏ.

Phiên bản Mảng (Yêu cầu cấp phát thêm bộ nhớ):

void merge_array(int arr[], int l, int m, int r) {
    int n1 = m - l + 1; int n2 = r - m;
    int L[n1], R[n2]; // Mảng tạm trên Stack (hoặc malloc nếu mảng quá lớn)
    
    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++];
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void merge_sort_array(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_array(arr, l, m);
        merge_sort_array(arr, m + 1, r);
        merge_array(arr, l, m, r);
    }
}
Phiên bản Linked List (Tối ưu tuyệt đối, không tốn RAM phụ):

struct Node* merge_list(struct Node* a, struct Node* b) {
    if (!a) return b;
    if (!b) return a;
    struct Node* result = NULL;
    if (a->data <= b->data) {
        result = a;
        result->next = merge_list(a->next, b);
    } else {
        result = b;
        result->next = merge_list(a, b->next);
    }
    return result;
}

void split_list(struct Node* source, struct Node** front, struct Node** back) {
    struct Node *fast = source->next, *slow = source;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    *front = source;
    *back = slow->next;
    slow->next = NULL; // Bẻ gãy Cây con trỏ
}

void merge_sort_list(struct Node** head_ref) {
    struct Node* head = *head_ref;
    if (!head || !head->next) return;
    struct Node *a, *b;
    split_list(head, &a, &b);
    merge_sort_list(&a);
    merge_sort_list(&b);
    *head_ref = merge_list(a, b); // Nối lại thành 1
}
*/

/*7. Counting Sort - Chỉ dùng Mảng
Siêu nhanh nhưng ngốn RAM khủng khiếp nếu dải giá trị lớn (ví dụ: sắp xếp các số từ 1 đến 1 tỷ cần cấp phát mảng 4GB RAM).

// Yêu cầu: Biết trước dải giá trị từ 0 đến max_val
void counting_sort_array(int arr[], int n, int max_val) {
    int *count = (int *)calloc(max_val + 1, sizeof(int));
    int *output = (int *)malloc(n * sizeof(int));

    // 1. Ánh xạ dữ liệu thành chỉ số mảng và đếm
    for (int i = 0; i < n; i++) count[arr[i]]++;

    // 2. Tính tổng tiền tố để biết vị trí thực tế
    for (int i = 1; i <= max_val; i++) count[i] += count[i - 1];

    // 3. Đổ dữ liệu ra (Duyệt ngược để đảm bảo tính Ổn định - Stable)
    for (int i = n - 1; i >= 0; i--) {
        output[count[arr[i]] - 1] = arr[i];
        count[arr[i]]--;
    }

    // 4. Cập nhật lại và dọn RAM
    for (int i = 0; i < n; i++) arr[i] = output[i];
    free(count); free(output);
}
*/

/*8. Radix Sort - Chỉ dùng Mảng
Khắc phục điểm yếu ngốn RAM của Counting Sort bằng cách phân tách từng chữ số (Hàng đơn vị, hàng chục, hàng trăm...).

// Hàm đếm tần suất cấu hình riêng cho từng chữ số (exp = 1, 10, 100...)
void counting_sort_for_radix(int arr[], int n, int exp) {
    int *output = (int *)malloc(n * sizeof(int));
    int count[10] = {0}; // Hệ thập phân chỉ có 10 chữ số (0-9)

    for (int i = 0; i < n; i++) count[(arr[i] / exp) % 10]++;
    for (int i = 1; i < 10; i++) count[i] += count[i - 1];
    
    for (int i = n - 1; i >= 0; i--) {
        int digit = (arr[i] / exp) % 10;
        output[count[digit] - 1] = arr[i];
        count[digit]--;
    }
    for (int i = 0; i < n; i++) arr[i] = output[i];
    free(output);
}

void radix_sort_array(int arr[], int n) {
    // 1. Tìm giá trị lớn nhất để biết số lượng chữ số
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }

    // 2. Sắp xếp lần lượt từng hàng: Đơn vị, Chục, Trăm...
    for (int exp = 1; max / exp > 0; exp *= 10) {
        counting_sort_for_radix(arr, n, exp);
    }
}
*/