/* Cây nhị phân tìm kiếm (BST) - Hoàn chỉnh với Chèn, Xóa và Duyệt
Chương trình này minh họa thuật toán cập nhật con trỏ đệ quy khi xóa một khối nhớ có 2 nhánh con (Trường hợp phức tạp nhất của BST).

#include <stdio.h>
#include <stdlib.h>

struct BSTNode {
    int data;
    struct BSTNode *left;
    struct BSTNode *right;
};

struct BSTNode* create_node(int data) {
    struct BSTNode *new_node = (struct BSTNode*)malloc(sizeof(struct BSTNode));
    if (!new_node) exit(1);
    new_node->data = data;
    new_node->left = new_node->right = NULL;
    return new_node;
}

struct BSTNode* insert(struct BSTNode* root, int data) {
    if (root == NULL) return create_node(data);
    if (data < root->data)
        root->left = insert(root->left, data);
    else if (data > root->data)
        root->right = insert(root->right, data);
    return root;
}

struct BSTNode* find_min(struct BSTNode* node) {
    while (node && node->left != NULL) node = node->left;
    return node;
}

struct BSTNode* delete_node(struct BSTNode* root, int key) {
    if (root == NULL) return root;

    if (key < root->data) {
        root->left = delete_node(root->left, key);
    } else if (key > root->data) {
        root->right = delete_node(root->right, key);
    } else {
        // Khớp giá trị cần xóa
        if (root->left == NULL) {
            struct BSTNode *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            struct BSTNode *temp = root->left;
            free(root);
            return temp;
        }
        // Nút có 2 con: Thay thế bằng giá trị nhỏ nhất của nhánh phải
        struct BSTNode *temp = find_min(root->right);
        root->data = temp->data; // Chỉ chép dữ liệu, giữ nguyên liên kết
        root->right = delete_node(root->right, temp->data); // Xóa nút thay thế
    }
    return root;
}

void inorder(struct BSTNode* root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d ", root->data);
        inorder(root->right);
    }
}

void free_bst(struct BSTNode* root) {
    if (root != NULL) {
        free_bst(root->left);
        free_bst(root->right);
        free(root);
    }
}

int main() {
    printf("=== KIỂM THỬ BINARY SEARCH TREE ===\n");
    struct BSTNode *root = NULL;
    
    root = insert(root, 50);
    insert(root, 30); insert(root, 20); insert(root, 40);
    insert(root, 70); insert(root, 60); insert(root, 80);

    printf("[+] Duyệt Inorder (Tăng dần): ");
    inorder(root); 
    printf("\n");

    printf("[-] Xóa 20 (Nút lá)\n");
    root = delete_node(root, 20);
    
    printf("[-] Xóa 30 (Nút có 1 con)\n");
    root = delete_node(root, 30);
    
    printf("[-] Xóa 50 (Nút có 2 con - Root)\n");
    root = delete_node(root, 50);

    printf("[+] Duyệt Inorder sau khi xóa: ");
    inorder(root);
    printf("\n");

    free_bst(root);
    return 0;
}
*/

/*2. Cây AVL - Hoàn chỉnh với Thuật toán Cân bằngChương trình triển khai 4 phép quay con trỏ (Left-Left, Right-Right, Left-Right, Right-Left) để duy trì chiều cao logarit O(log n)

#include <stdio.h>
#include <stdlib.h>

struct AVLNode {
    int key;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
};

int get_height(struct AVLNode *n) { return (n == NULL) ? 0 : n->height; }
int get_max(int a, int b) { return (a > b) ? a : b; }
int get_balance(struct AVLNode *n) { return (n == NULL) ? 0 : get_height(n->left) - get_height(n->right); }

struct AVLNode* create_node(int key) {
    struct AVLNode* node = (struct AVLNode*)malloc(sizeof(struct AVLNode));
    if (!node) exit(1);
    node->key = key;
    node->left = node->right = NULL;
    node->height = 1; // Nút mới luôn ở cấp 1
    return node;
}

// Phép quay phải 
struct AVLNode* right_rotate(struct AVLNode *y) {
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;
    
    x->right = y;
    y->left = T2;
    
    y->height = get_max(get_height(y->left), get_height(y->right)) + 1;
    x->height = get_max(get_height(x->left), get_height(x->right)) + 1;
    return x;
}

// Phép quay trái 
struct AVLNode* left_rotate(struct AVLNode *x) {
    struct AVLNode *y = x->right;
    struct AVLNode *T2 = y->left;
    
    y->left = x;
    x->right = T2;
    
    x->height = get_max(get_height(x->left), get_height(x->right)) + 1;
    y->height = get_max(get_height(y->left), get_height(y->right)) + 1;
    return y;
}

struct AVLNode* insert(struct AVLNode* node, int key) {
    // 1. Chèn BST thông thường
    if (node == NULL) return create_node(key);
    if (key < node->key) node->left = insert(node->left, key);
    else if (key > node->key) node->right = insert(node->right, key);
    else return node; // Không chấp nhận khóa trùng

    // 2. Cập nhật chiều cao
    node->height = 1 + get_max(get_height(node->left), get_height(node->right));

    // 3. Kiểm tra hệ số cân bằng và Quay
    int balance = get_balance(node);

    // Mất cân bằng Trái-Trái
    if (balance > 1 && key < node->left->key) return right_rotate(node);
    // Mất cân bằng Phải-Phải
    if (balance < -1 && key > node->right->key) return left_rotate(node);
    // Mất cân bằng Trái-Phải
    if (balance > 1 && key > node->left->key) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    // Mất cân bằng Phải-Trái
    if (balance < -1 && key < node->right->key) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    return node;
}

void preorder(struct AVLNode *root) {
    if (root != NULL) {
        printf("%d ", root->key);
        preorder(root->left);
        preorder(root->right);
    }
}

void free_avl(struct AVLNode *root) {
    if (root) { free_avl(root->left); free_avl(root->right); free(root); }
}

int main() {
    printf("=== KIỂM THỬ AVL TREE (TỰ CÂN BẰNG) ===\n");
    struct AVLNode *root = NULL;

    // Chèn dữ liệu theo thứ tự tăng dần (Gây suy biến ở BST thường)
    root = insert(root, 10);
    root = insert(root, 20);
    root = insert(root, 30); // Sẽ kích hoạt Left Rotate tại 10
    root = insert(root, 40);
    root = insert(root, 50); // Sẽ kích hoạt Left Rotate tại 30
    root = insert(root, 25); // Sẽ kích hoạt Right-Left Rotate

    // Duyệt Preorder (Gốc -> Trái -> Phải) để kiểm chứng gốc đã được cấu trúc lại
    printf("[+] Duyệt Preorder của AVL: ");
    preorder(root); // Kết quả kỳ vọng: 30 20 10 25 40 50
    printf("\n");

    free_avl(root);
    return 0;
}
*/

/*3. Ternary Search Tree (TST) - Hoàn chỉnh quản lý Từ vựng
Tối ưu hóa không gian bộ nhớ khi lưu trữ từ điển (Dictionary) thay vì dùng mảng cấp phát 26 con trỏ của kiến trúc Trie chuẩn.

#include <stdio.h>
#include <stdlib.h>

struct TSTNode {
    char data;
    unsigned int is_end : 1; // Bit-field tiết kiệm RAM
    struct TSTNode *left, *eq, *right;
};

struct TSTNode* create_node(char data) {
    struct TSTNode* node = (struct TSTNode*)malloc(sizeof(struct TSTNode));
    if (!node) exit(1);
    node->data = data;
    node->is_end = 0;
    node->left = node->eq = node->right = NULL;
    return node;
}

struct TSTNode* insert(struct TSTNode *root, char *word) {
    if (root == NULL) root = create_node(*word);

    if (*word < root->data) {
        root->left = insert(root->left, word);
    } else if (*word > root->data) {
        root->right = insert(root->right, word);
    } else {
        if (*(word + 1) != '\0') {
            root->eq = insert(root->eq, word + 1);
        } else {
            root->is_end = 1;
        }
    }
    return root;
}

// Thuật toán duyệt đệ quy để in toàn bộ các từ có trong TST
void traverse_tst(struct TSTNode* root, char* buffer, int depth) {
    if (root) {
        traverse_tst(root->left, buffer, depth);
        
        buffer[depth] = root->data;
        if (root->is_end) {
            buffer[depth + 1] = '\0';
            printf("- %s\n", buffer);
        }
        
        traverse_tst(root->eq, buffer, depth + 1);
        traverse_tst(root->right, buffer, depth);
    }
}

void free_tst(struct TSTNode* root) {
    if (root) {
        free_tst(root->left);
        free_tst(root->eq);
        free_tst(root->right);
        free(root);
    }
}

int main() {
    printf("=== KIỂM THỬ TERNARY SEARCH TREE ===\n");
    struct TSTNode *root = NULL;

    root = insert(root, "cat");
    insert(root, "cats");
    insert(root, "bug");
    insert(root, "up");

    char buffer[50];
    printf("[+] Các chuỗi được lưu trữ trong TST:\n");
    traverse_tst(root, buffer, 0);

    free_tst(root);
    return 0;
}
*/

/*4. N-ary Tree - Kiến trúc Left-Child Right-Sibling
Chuyển đổi cây đa phân (N nhánh) thành cấu trúc nhị phân để đảm bảo kích thước cấp phát tĩnh cho từng khối nhớ struct, 
giải quyết bài toán cạn kiệt RAM do mảng con trỏ.

#include <stdio.h>
#include <stdlib.h>

struct NaryNode {
    int data;
    struct NaryNode *first_child;
    struct NaryNode *next_sibling;
};

struct NaryNode* create_node(int data) {
    struct NaryNode *node = (struct NaryNode*)malloc(sizeof(struct NaryNode));
    if (!node) exit(1);
    node->data = data;
    node->first_child = NULL;
    node->next_sibling = NULL;
    return node;
}

// Thêm một nút con vào nút cha. 
// Nếu cha đã có con, nút mới sẽ được thêm vào danh sách anh em của đứa con đầu.
void add_child(struct NaryNode *parent, int child_data) {
    struct NaryNode *new_node = create_node(child_data);
    
    if (parent->first_child == NULL) {
        parent->first_child = new_node;
    } else {
        // Duyệt danh sách liên kết của các anh em ruột
        struct NaryNode *sibling = parent->first_child;
        while (sibling->next_sibling != NULL) {
            sibling = sibling->next_sibling;
        }
        sibling->next_sibling = new_node;
    }
}

// Duyệt cây theo chiều sâu (DFS) mô phỏng lại cấu trúc phân cấp
void print_nary_tree(struct NaryNode *root, int depth) {
    if (root == NULL) return;

    // In thụt lề mô phỏng độ sâu
    for (int i = 0; i < depth; i++) printf("  ");
    printf("|-- %d\n", root->data);

    // Duyệt đệ quy đứa con đầu tiên (tăng độ sâu)
    print_nary_tree(root->first_child, depth + 1);
    
    // Duyệt đệ quy người anh em kế tiếp (giữ nguyên độ sâu)
    print_nary_tree(root->next_sibling, depth);
}

void free_nary_tree(struct NaryNode *root) {
    if (root) {
        free_nary_tree(root->first_child);
        free_nary_tree(root->next_sibling);
        free(root);
    }
}

int main() {
    printf("=== KIỂM THỬ N-ARY TREE (FIRST-CHILD / NEXT-SIBLING) ===\n");
    struct NaryNode *root = create_node(1); // Gốc

    add_child(root, 2);
    add_child(root, 3);
    add_child(root, 4);

    // Thêm con cho nút 2
    add_child(root->first_child, 5);
    add_child(root->first_child, 6);

    // Thêm con cho nút 4
    add_child(root->first_child->next_sibling->next_sibling, 7);

    printf("[+] Cấu trúc phân cấp hệ thống:\n");
    print_nary_tree(root, 0);

    free_nary_tree(root);
    return 0;
}
*/

/*1. Red-Black Tree: 
Kiến trúc Cân bằng Cục bộRed-Black Tree không yêu cầu cây phải cân bằng tuyệt đối như AVL. 
Nó duy trì sự cân bằng tương đối thông qua 5 quy tắc nghiêm ngặt về màu sắc, đảm bảo đường đi dài nhất từ gốc đến lá không bao giờ vượt quá 2 lần đường đi ngắn nhất. 
Độ phức tạp luôn được đảm bảo ở mức O(log n).

Mã nguồn C (Cơ chế Chèn và Sửa lỗi Fix-up)
#include <stdio.h>
#include <stdlib.h>

enum Color { RED, BLACK };

struct RBNode {
    int data;
    enum Color color;
    struct RBNode *left, *right, *parent;
};

// Hàm hỗ trợ khởi tạo khối nhớ
struct RBNode* create_node(int data) {
    struct RBNode* node = (struct RBNode*)malloc(sizeof(struct RBNode));
    node->data = data;
    node->color = RED; // Nút mới chèn mặc định phải là màu ĐỎ
    node->left = node->right = node->parent = NULL;
    return node;
}

// Hàm quay trái (Thao tác con trỏ vật lý O(1))
void left_rotate(struct RBNode **root, struct RBNode *x) {
    struct RBNode *y = x->right;
    x->right = y->left;
    if (y->left != NULL) y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NULL) *root = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
}

// Hàm quay phải
void right_rotate(struct RBNode **root, struct RBNode *y) {
    struct RBNode *x = y->left;
    y->left = x->right;
    if (x->right != NULL) x->right->parent = y;
    x->parent = y->parent;
    if (y->parent == NULL) *root = x;
    else if (y == y->parent->left) y->parent->left = x;
    else y->parent->right = x;
    x->right = y;
    y->parent = x;
}

// Thuật toán Fix-up: Khôi phục thuộc tính Đỏ-Đen sau khi chèn
void insert_fixup(struct RBNode **root, struct RBNode *z) {
    // Vòng lặp chạy khi nút cha có màu Đỏ (Vi phạm quy tắc 2 nút Đỏ liên tiếp)
    while (z->parent != NULL && z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            struct RBNode *y = z->parent->parent->right; // Nút chú (Uncle)
            
            // Trường hợp 1: Chú màu Đỏ -> Đổi màu Cha, Chú và Ông nội
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Đẩy vấn đề lên tầng cao hơn
            } else {
                // Trường hợp 2: Chú màu Đen, Z là con phải -> Quay trái
                if (z == z->parent->right) {
                    z = z->parent;
                    left_rotate(root, z);
                }
                // Trường hợp 3: Chú màu Đen, Z là con trái -> Quay phải và đổi màu
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                right_rotate(root, z->parent->parent);
            }
        } else {
            // Logic tương tự cho nhánh phải (Đối xứng)
            struct RBNode *y = z->parent->parent->left;
            if (y != NULL && y->color == RED) {
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else {
                if (z == z->parent->left) {
                    z = z->parent;
                    right_rotate(root, z);
                }
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                left_rotate(root, z->parent->parent);
            }
        }
    }
    (*root)->color = BLACK; // Gốc luôn phải là màu Đen
}

// Chèn dữ liệu cơ bản như BST và gọi Fix-up
void insert_rb(struct RBNode **root, int data) {
    struct RBNode *z = create_node(data);
    struct RBNode *y = NULL;
    struct RBNode *x = *root;

    // Duyệt tìm vị trí chèn
    while (x != NULL) {
        y = x;
        if (z->data < x->data) x = x->left;
        else x = x->right;
    }

    z->parent = y;
    if (y == NULL) *root = z; // Cây rỗng
    else if (z->data < y->data) y->left = z;
    else y->right = z;

    insert_fixup(root, z);
}

// Duyệt cây Inorder để kiểm chứng
void inorder(struct RBNode *root) {
    if (root != NULL) {
        inorder(root->left);
        printf("%d(%c) ", root->data, root->color == RED ? 'R' : 'B');
        inorder(root->right);
    }
}

int main() {
    printf("=== RED-BLACK TREE ===\n");
    struct RBNode *root = NULL;
    
    // Chèn chuỗi gây suy biến để kích hoạt các phép quay và đổi màu
    insert_rb(&root, 10);
    insert_rb(&root, 20);
    insert_rb(&root, 30);
    insert_rb(&root, 40);
    insert_rb(&root, 50);

    printf("[+] Cấu trúc (B=Black, R=Red): ");
    inorder(root); // Kết quả sẽ được cân bằng lại thay vì là một danh sách thẳng
    printf("\n");
    return 0;
}
*/

/*2. B-Tree: Kiến trúc Chỉ mục Ổ đĩa (Disk Indexing)
Thiết kế của B-Tree nhằm mục đích tối đa hóa lượng dữ liệu đọc được trong một chu kỳ truy xuất đĩa (Disk I/O). 
Thay vì phân bổ từng int vào một struct, nó gom nhóm một mảng các khóa (Keys) vào chung một khối. 
Thuật toán cốt lõi nhất của B-Tree là Tách khối (Node Splitting): Khi một nút đầy, nó vỡ ra làm hai và đẩy khóa ở giữa lên nút cha.

Mã nguồn C (Cơ chế Tách khối chủ động)
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define T 2 // Bậc tối thiểu (Minimum degree). Một nút chứa tối đa 2T-1 = 3 khóa.

struct BTreeNode {
    int keys[2 * T - 1];             // Mảng khóa vật lý
    struct BTreeNode *children[2 * T]; // Mảng con trỏ
    int num_keys;                    // Số lượng khóa hiện hành
    bool is_leaf;                    // Cờ xác định nút lá
};

struct BTreeNode* create_btree_node(bool is_leaf) {
    struct BTreeNode* node = (struct BTreeNode*)malloc(sizeof(struct BTreeNode));
    node->num_keys = 0;
    node->is_leaf = is_leaf;
    for (int i = 0; i < 2 * T; i++) node->children[i] = NULL;
    return node;
}

// Tách một nút con (y) đang bị ĐẦY (có 2T-1 khóa). 
// Đẩy khóa trung vị lên nút cha (x).
void split_child(struct BTreeNode *x, int i, struct BTreeNode *y) {
    struct BTreeNode *z = create_btree_node(y->is_leaf);
    z->num_keys = T - 1;

    // 1. Sao chép nửa sau của y sang z
    for (int j = 0; j < T - 1; j++) {
        z->keys[j] = y->keys[j + T];
    }
    if (!y->is_leaf) {
        for (int j = 0; j < T; j++) {
            z->children[j] = y->children[j + T];
        }
    }
    y->num_keys = T - 1;

    // 2. Dịch chuyển các con trỏ của nút cha x để nhường chỗ cho z
    for (int j = x->num_keys; j >= i + 1; j--) {
        x->children[j + 1] = x->children[j];
    }
    x->children[i + 1] = z;

    // 3. Đẩy khóa trung vị của y lên x
    for (int j = x->num_keys - 1; j >= i; j--) {
        x->keys[j + 1] = x->keys[j];
    }
    x->keys[i] = y->keys[T - 1];
    x->num_keys++;
}

// Chèn vào một nút được đảm bảo là CHƯA ĐẦY
void insert_non_full(struct BTreeNode *x, int key) {
    int i = x->num_keys - 1;

    if (x->is_leaf) {
        // Dịch chuyển các khóa vật lý trong mảng để chèn khóa mới
        while (i >= 0 && x->keys[i] > key) {
            x->keys[i + 1] = x->keys[i];
            i--;
        }
        x->keys[i + 1] = key;
        x->num_keys++;
    } else {
        // Định tuyến xuống nút con phù hợp
        while (i >= 0 && x->keys[i] > key) i--;
        i++;
        
        // Nếu nút con đã đầy, tách nó ra trước khi đi xuống tiếp
        if (x->children[i]->num_keys == 2 * T - 1) {
            split_child(x, i, x->children[i]);
            if (key > x->keys[i]) i++;
        }
        insert_non_full(x->children[i], key);
    }
}

// Giao diện hàm chèn chính
struct BTreeNode* insert_btree(struct BTreeNode *root, int key) {
    if (root == NULL) {
        root = create_btree_node(true);
        root->keys[0] = key;
        root->num_keys = 1;
        return root;
    }

    // Nếu nút GỐC đầy, chiều cao của B-Tree sẽ tăng lên 1
    if (root->num_keys == 2 * T - 1) {
        struct BTreeNode *s = create_btree_node(false);
        s->children[0] = root;
        split_child(s, 0, root);
        insert_non_full(s, key);
        return s; // s trở thành Gốc mới
    } else {
        insert_non_full(root, key);
        return root;
    }
}

void print_btree(struct BTreeNode *root, int level) {
    if (root != NULL) {
        printf("Level %d: ", level);
        for (int i = 0; i < root->num_keys; i++) printf("%d ", root->keys[i]);
        printf("\n");
        if (!root->is_leaf) {
            for (int i = 0; i <= root->num_keys; i++) {
                print_btree(root->children[i], level + 1);
            }
        }
    }
}

int main() {
    printf("=== B-TREE (Bậc T=2) ===\n");
    struct BTreeNode *root = NULL;

    // Liên tục chèn để gây hiện tượng tràn Node (Node Overflow)
    int keys[] = {10, 20, 30, 40, 50, 60, 70};
    for(int i = 0; i < 7; i++) {
        root = insert_btree(root, keys[i]);
    }

    printf("[+] Cấu trúc sau khi liên tục Tách khối (Node Splitting):\n");
    print_btree(root, 0);

    return 0;
}
*/