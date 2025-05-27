#include <iostream>
using namespace std;

// Konstanta
const int MAX_USERS = 100;
const int MAX_SELLERS = 50;
const int MAX_PRODUCTS = 200;
const int MAX_QUEUE = 50;
const int MAX_STACK = 100;
const int MAX_REVIEWS = 100;
const int MAX_GRAPH_NODES = 20;
const int HASH_SIZE = 101;

// Struktur Data untuk User
struct User
{
    int id;
    char username[50];
    char password[50];
    char alamat[100];
    bool isActive;
};

// Struktur Data untuk Seller
struct Seller
{
    int id;
    char username[50];
    char password[50];
    char nama_toko[100];
    bool isActive;
};

// Struktur Data untuk Produk (Linked List)
struct Product
{
    int id;
    int seller_id;
    char nama[100];
    char kategori[50];
    char subkategori[50];
    double harga;
    int stok;
    double rating;
    int jumlah_review;
    Product *next;
};

// Struktur Data untuk Review (BST)
struct Review
{
    int product_id;
    int user_id;
    char username[50];
    int rating;
    char komentar[200];
    Review *left;
    Review *right;
};

// Struktur Data untuk Transaksi (Stack)
struct Transaction
{
    int id;
    int user_id;
    int product_id;
    char nama_produk[100];
    double harga;
    int quantity;
    char tanggal[20];
};

// Struktur Data untuk Queue Checkout
struct QueueItem
{
    int user_id;
    int product_id;
    int quantity;
    double total_harga;
};

// Struktur Data untuk Graph (Rute Pengiriman)
struct GraphNode
{
    int id;
    char nama_lokasi[50];
    bool visited;
};

struct GraphEdge
{
    int from;
    int to;
    int distance;
};

// Variabel Global - Hash Tables
User userHashTable[HASH_SIZE];
Seller sellerHashTable[HASH_SIZE];

// Variabel Global - Linked List untuk Produk
Product *productHead = nullptr;

// Global Variables - Queue untuk Checkout
QueueItem checkoutQueue[MAX_QUEUE];
int queueFront = 0;
int queueRear = -1;
int queueSize = 0;

// Variabel Global - Stack untuk Transaksi
Transaction transactionStack[MAX_STACK];
int stackTop = -1;

// Variabel Global - BST untuk Review
Review *reviewRoot = nullptr;

// Variabel Global - Graph untuk Rute
GraphNode graphNodes[MAX_GRAPH_NODES];
GraphEdge graphEdges[MAX_GRAPH_NODES * MAX_GRAPH_NODES];
int nodeCount = 0;
int edgeCount = 0;

// Variabel Global - ID Counter
int userIdCounter = 1;
int sellerIdCounter = 1;
int productIdCounter = 1;
int transactionIdCounter = 1;

// Hash Function
int hashFunction(const char *key)
{
    int hash = 0;
    int len = 0;
    while (key[len] != '\0')
        len++; // Calculate length

    for (int i = 0; i < len; i++)
    {
        hash = (hash * 31 + key[i]) % HASH_SIZE;
    }
    return hash < 0 ? -hash : hash;
}

// String Functions
int stringLength(const char *str)
{
    int len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

void stringCopy(char *dest, const char *src)
{
    int i = 0;
    while (src[i] != '\0')
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

int stringCompare(const char *str1, const char *str2)
{
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0')
    {
        if (str1[i] < str2[i])
            return -1;
        if (str1[i] > str2[i])
            return 1;
        i++;
    }
    if (str1[i] == '\0' && str2[i] == '\0')
        return 0;
    return (str1[i] == '\0') ? -1 : 1;
}

bool stringContains(const char *haystack, const char *needle)
{
    int hayLen = stringLength(haystack);
    int needleLen = stringLength(needle);

    for (int i = 0; i <= hayLen - needleLen; i++)
    {
        bool match = true;
        for (int j = 0; j < needleLen; j++)
        {
            if (haystack[i + j] != needle[j])
            {
                match = false;
                break;
            }
        }
        if (match)
            return true;
    }
    return false;
}

// Inisialisasi Hash Tables
void initializeHashTables()
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        userHashTable[i].isActive = false;
        sellerHashTable[i].isActive = false;
    }
}

// User Management Functions
bool registerUser()
{
    char username[50], password[50], alamat[100];

    cout << "\n=== REGISTRASI USER ===" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    cout << "Alamat: ";
    cin.ignore();
    cin.getline(alamat, 100);

    int hashIndex = hashFunction(username);
    int originalIndex = hashIndex;

    // Linear probing untuk menangani collision
    while (userHashTable[hashIndex].isActive)
    {
        if (stringCompare(userHashTable[hashIndex].username, username) == 0)
        {
            cout << "Username sudah terdaftar!" << endl;
            return false;
        }
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
        {
            cout << "Hash table penuh!" << endl;
            return false;
        }
    }

    // Insert user data
    userHashTable[hashIndex].id = userIdCounter++;
    stringCopy(userHashTable[hashIndex].username, username);
    stringCopy(userHashTable[hashIndex].password, password);
    stringCopy(userHashTable[hashIndex].alamat, alamat);
    userHashTable[hashIndex].isActive = true;

    cout << "Registrasi berhasil!" << endl;
    return true;
}

User *loginUser()
{
    char username[50], password[50];

    cout << "\n=== LOGIN USER ===" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    int hashIndex = hashFunction(username);
    int originalIndex = hashIndex;

    while (userHashTable[hashIndex].isActive)
    {
        if (stringCompare(userHashTable[hashIndex].username, username) == 0 &&
            stringCompare(userHashTable[hashIndex].password, password) == 0)
        {
            cout << "Login berhasil!" << endl;
            return &userHashTable[hashIndex];
        }
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
            break;
    }

    cout << "Username atau password salah!" << endl;
    return nullptr;
}

// Seller Management Functions
bool registerSeller()
{
    char username[50], password[50], nama_toko[100];

    cout << "\n=== REGISTRASI SELLER ===" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    cout << "Nama Toko: ";
    cin.ignore();
    cin.getline(nama_toko, 100);

    int hashIndex = hashFunction(username);
    int originalIndex = hashIndex;

    while (sellerHashTable[hashIndex].isActive)
    {
        if (stringCompare(sellerHashTable[hashIndex].username, username) == 0)
        {
            cout << "Username sudah terdaftar!" << endl;
            return false;
        }
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
        {
            cout << "Hash table penuh!" << endl;
            return false;
        }
    }

    sellerHashTable[hashIndex].id = sellerIdCounter++;
    stringCopy(sellerHashTable[hashIndex].username, username);
    stringCopy(sellerHashTable[hashIndex].password, password);
    stringCopy(sellerHashTable[hashIndex].nama_toko, nama_toko);
    sellerHashTable[hashIndex].isActive = true;

    cout << "Registrasi seller berhasil!" << endl;
    return true;
}

Seller *loginSeller()
{
    char username[50], password[50];

    cout << "\n=== LOGIN SELLER ===" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;

    int hashIndex = hashFunction(username);
    int originalIndex = hashIndex;

    while (sellerHashTable[hashIndex].isActive)
    {
        if (stringCompare(sellerHashTable[hashIndex].username, username) == 0 &&
            stringCompare(sellerHashTable[hashIndex].password, password) == 0)
        {
            cout << "Login seller berhasil!" << endl;
            return &sellerHashTable[hashIndex];
        }
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
            break;
    }

    cout << "Username atau password salah!" << endl;
    return nullptr;
}

// Product Management Functions (Linked List)
void addProduct(int seller_id)
{
    char nama[100], kategori[50], subkategori[50];
    double harga;
    int stok;

    cout << "\n=== TAMBAH PRODUK ===" << endl;
    cout << "Nama Produk: ";
    cin.ignore();
    cin.getline(nama, 100);
    cout << "Kategori (smartphone/tablet/laptop/smartwatch): ";
    cin >> kategori;
    cout << "Subkategori: ";
    cin >> subkategori;
    cout << "Harga: ";
    cin >> harga;
    cout << "Stok: ";
    cin >> stok;

    Product *newProduct = new Product;
    newProduct->id = productIdCounter++;
    newProduct->seller_id = seller_id;
    stringCopy(newProduct->nama, nama);
    stringCopy(newProduct->kategori, kategori);
    stringCopy(newProduct->subkategori, subkategori);
    newProduct->harga = harga;
    newProduct->stok = stok;
    newProduct->rating = 0.0;
    newProduct->jumlah_review = 0;
    newProduct->next = productHead;
    productHead = newProduct;

    cout << "Produk berhasil ditambahkan!" << endl;
}

void displayProducts()
{
    cout << "\n=== DAFTAR PRODUK ===" << endl;
    Product *current = productHead;
    int count = 1;

    while (current != nullptr)
    {
        cout << count++ << ". " << current->nama << endl;
        cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
        cout << "   Harga: Rp " << current->harga << endl;
        cout << "   Stok: " << current->stok << endl;
        cout << "   Rating: " << current->rating << " (" << current->jumlah_review << " reviews)" << endl;
        cout << "   ID: " << current->id << endl;
        cout << endl;
        current = current->next;
    }
}

// Recursive function untuk navigasi kategori
void displayCategoryMenu(int level)
{
    if (level == 0)
    {
        cout << "\n=== KATEGORI GADGET ===" << endl;
        cout << "1. Smartphone" << endl;
        cout << "2. Tablet" << endl;
        cout << "3. Laptop" << endl;
        cout << "4. Smartwatch" << endl;
        cout << "5. Kembali ke Menu Utama" << endl;

        int choice;
        cout << "Pilih kategori: ";
        cin >> choice;

        if (choice >= 1 && choice <= 4)
        {
            displayCategoryMenu(choice);
        }
    }
    else
    {
        const char *categories[] = {"", "smartphone", "tablet", "laptop", "smartwatch"};
        cout << "\n=== PRODUK " << categories[level] << " ===" << endl;

        Product *current = productHead;
        int count = 1;
        bool found = false;

        while (current != nullptr)
        {
            if (stringCompare(current->kategori, categories[level]) == 0)
            {
                cout << count++ << ". " << current->nama << endl;
                cout << "   Harga: Rp " << current->harga << endl;
                cout << "   Stok: " << current->stok << endl;
                cout << "   Rating: " << current->rating << endl;
                cout << "   ID: " << current->id << endl;
                cout << endl;
                found = true;
            }
            current = current->next;
        }

        if (!found)
        {
            cout << "Tidak ada produk dalam kategori ini." << endl;
        }
    }
}

// Sorting Functions
void sortProductsByPrice(bool ascending)
{
    if (productHead == nullptr || productHead->next == nullptr)
        return;

    // Bubble sort untuk linked list
    bool swapped;
    do
    {
        swapped = false;
        Product *current = productHead;
        Product *prev = nullptr;

        while (current->next != nullptr)
        {
            bool shouldSwap = ascending ? (current->harga > current->next->harga) : (current->harga < current->next->harga);

            if (shouldSwap)
            {
                // Swap nodes
                Product *next = current->next;
                current->next = next->next;
                next->next = current;

                if (prev == nullptr)
                {
                    productHead = next;
                }
                else
                {
                    prev->next = next;
                }

                prev = next;
                swapped = true;
            }
            else
            {
                prev = current;
                current = current->next;
            }
        }
    } while (swapped);

    cout << "Produk berhasil diurutkan!" << endl;
}

// Search Function
void searchProducts()
{
    char keyword[100];
    cout << "\nMasukkan kata kunci pencarian: ";
    cin.ignore();
    cin.getline(keyword, 100);

    cout << "\n=== HASIL PENCARIAN ===" << endl;
    Product *current = productHead;
    int count = 1;
    bool found = false;

    while (current != nullptr)
    {
        if (stringContains(current->nama, keyword))
        {
            cout << count++ << ". " << current->nama << endl;
            cout << "   Kategori: " << current->kategori << endl;
            cout << "   Harga: Rp " << current->harga << endl;
            cout << "   Stok: " << current->stok << endl;
            cout << "   Rating: " << current->rating << endl;
            cout << "   ID: " << current->id << endl;
            cout << endl;
            found = true;
        }
        current = current->next;
    }

    if (!found)
    {
        cout << "Produk tidak ditemukan." << endl;
    }
}

// Queue Functions untuk Checkout
bool enqueue(int user_id, int product_id, int quantity, double total_harga)
{
    if (queueSize >= MAX_QUEUE)
    {
        cout << "Antrian checkout penuh!" << endl;
        return false;
    }

    queueRear = (queueRear + 1) % MAX_QUEUE;
    checkoutQueue[queueRear].user_id = user_id;
    checkoutQueue[queueRear].product_id = product_id;
    checkoutQueue[queueRear].quantity = quantity;
    checkoutQueue[queueRear].total_harga = total_harga;
    queueSize++;

    return true;
}

bool dequeue(QueueItem *item)
{
    if (queueSize == 0)
    {
        return false;
    }

    *item = checkoutQueue[queueFront];
    queueFront = (queueFront + 1) % MAX_QUEUE;
    queueSize--;

    return true;
}

void addToCheckout(int user_id)
{
    int product_id, quantity;
    cout << "\nMasukkan ID produk: ";
    cin >> product_id;
    cout << "Masukkan quantity: ";
    cin >> quantity;

    // Cari produk
    Product *current = productHead;
    while (current != nullptr)
    {
        if (current->id == product_id)
        {
            if (current->stok >= quantity)
            {
                double total = current->harga * quantity;
                if (enqueue(user_id, product_id, quantity, total))
                {
                    cout << "Produk berhasil ditambahkan ke checkout!" << endl;
                    cout << "Total: Rp " << total << endl;
                }
                else
                {
                    cout << "Gagal menambahkan ke checkout!" << endl;
                }
            }
            else
            {
                cout << "Stok tidak mencukupi!" << endl;
            }
            return;
        }
        current = current->next;
    }

    cout << "Produk tidak ditemukan!" << endl;
}

// Stack Functions untuk Transaksi
bool push(Transaction trans)
{
    if (stackTop >= MAX_STACK - 1)
    {
        cout << "Stack transaksi penuh!" << endl;
        return false;
    }

    transactionStack[++stackTop] = trans;
    return true;
}

void processCheckout()
{
    if (queueSize == 0)
    {
        cout << "Tidak ada item dalam antrian checkout!" << endl;
        return;
    }

    QueueItem item;
    if (dequeue(&item))
    {
        // Cari produk dan update stok
        Product *current = productHead;
        while (current != nullptr)
        {
            if (current->id == item.product_id)
            {
                if (current->stok >= item.quantity)
                {
                    current->stok -= item.quantity;

                    // Buat transaksi baru
                    Transaction trans;
                    trans.id = transactionIdCounter++;
                    trans.user_id = item.user_id;
                    trans.product_id = item.product_id;
                    stringCopy(trans.nama_produk, current->nama);
                    trans.harga = current->harga;
                    trans.quantity = item.quantity;
                    stringCopy(trans.tanggal, "2024-01-01"); // Simplified date

                    if (push(trans))
                    {
                        cout << "\nCheckout berhasil!" << endl;
                        cout << "Total pembayaran: Rp " << item.total_harga << endl;
                        cout << "Transaksi ID: " << trans.id << endl;
                    }
                }
                else
                {
                    cout << "Stok tidak mencukupi untuk checkout!" << endl;
                }
                return;
            }
            current = current->next;
        }
    }
}

void displayTransactionHistory(int user_id)
{
    cout << "\n=== RIWAYAT TRANSAKSI ===" << endl;
    bool found = false;

    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].user_id == user_id)
        {
            cout << "ID Transaksi: " << transactionStack[i].id << endl;
            cout << "Produk: " << transactionStack[i].nama_produk << endl;
            cout << "Harga: Rp " << transactionStack[i].harga << endl;
            cout << "Quantity: " << transactionStack[i].quantity << endl;
            cout << "Total: Rp " << (transactionStack[i].harga * transactionStack[i].quantity) << endl;
            cout << "Tanggal: " << transactionStack[i].tanggal << endl;
            cout << "------------------------" << endl;
            found = true;
        }
    }

    if (!found)
    {
        cout << "Belum ada transaksi." << endl;
    }
}

// BST Functions untuk Review
Review *insertReview(Review *root, int product_id, int user_id, const char *username, int rating, const char *komentar)
{
    if (root == nullptr)
    {
        Review *newReview = new Review;
        newReview->product_id = product_id;
        newReview->user_id = user_id;
        stringCopy(newReview->username, username);
        newReview->rating = rating;
        stringCopy(newReview->komentar, komentar);
        newReview->left = nullptr;
        newReview->right = nullptr;
        return newReview;
    }

    if (rating < root->rating)
    {
        root->left = insertReview(root->left, product_id, user_id, username, rating, komentar);
    }
    else
    {
        root->right = insertReview(root->right, product_id, user_id, username, rating, komentar);
    }

    return root;
}

void displayReviews(Review *root, int product_id)
{
    if (root != nullptr)
    {
        displayReviews(root->right, product_id); // Tampilkan rating tertinggi dulu

        if (root->product_id == product_id)
        {
            cout << "User: " << root->username << endl;
            cout << "Rating: " << root->rating << "/5" << endl;
            cout << "Komentar: " << root->komentar << endl;
            cout << "------------------------" << endl;
        }

        displayReviews(root->left, product_id);
    }
}

void addReview(int user_id, const char *username)
{
    int product_id, rating;
    char komentar[200];

    cout << "\n=== TAMBAH REVIEW ===" << endl;
    cout << "ID Produk: ";
    cin >> product_id;
    cout << "Rating (1-5): ";
    cin >> rating;
    cout << "Komentar: ";
    cin.ignore();
    cin.getline(komentar, 200);

    if (rating < 1 || rating > 5)
    {
        cout << "Rating harus antara 1-5!" << endl;
        return;
    }

    reviewRoot = insertReview(reviewRoot, product_id, user_id, username, rating, komentar);

    // Update rating produk
    Product *current = productHead;
    while (current != nullptr)
    {
        if (current->id == product_id)
        {
            current->jumlah_review++;
            current->rating = ((current->rating * (current->jumlah_review - 1)) + rating) / current->jumlah_review;
            break;
        }
        current = current->next;
    }

    cout << "Review berhasil ditambahkan!" << endl;
}

void viewProductReviews()
{
    int product_id;
    cout << "\nMasukkan ID produk: ";
    cin >> product_id;

    cout << "\n=== REVIEW PRODUK ===" << endl;
    displayReviews(reviewRoot, product_id);
}

// Graph Functions untuk Simulasi Pengiriman
void initializeGraph()
{
    // Inisialisasi beberapa lokasi gudang dan kota
    stringCopy(graphNodes[0].nama_lokasi, "Gudang Jakarta");
    stringCopy(graphNodes[1].nama_lokasi, "Gudang Surabaya");
    stringCopy(graphNodes[2].nama_lokasi, "Gudang Medan");
    stringCopy(graphNodes[3].nama_lokasi, "Kota Bandung");
    stringCopy(graphNodes[4].nama_lokasi, "Kota Yogyakarta");
    stringCopy(graphNodes[5].nama_lokasi, "Kota Semarang");

    for (int i = 0; i < 6; i++)
    {
        graphNodes[i].id = i;
        graphNodes[i].visited = false;
    }
    nodeCount = 6;

    // Tambahkan beberapa rute (edges)
    graphEdges[0] = {0, 3, 150};  // Jakarta - Bandung
    graphEdges[1] = {0, 4, 500};  // Jakarta - Yogyakarta
    graphEdges[2] = {1, 4, 300};  // Surabaya - Yogyakarta
    graphEdges[3] = {1, 5, 250};  // Surabaya - Semarang
    graphEdges[4] = {2, 0, 1000}; // Medan - Jakarta
    graphEdges[5] = {3, 4, 350};  // Bandung - Yogyakarta
    edgeCount = 6;
}

void resetGraphVisited()
{
    for (int i = 0; i < nodeCount; i++)
    {
        graphNodes[i].visited = false;
    }
}

void dfsRoute(int start, int destination, int currentDistance, int *shortestDistance, int path[], int pathLength)
{
    if (start == destination)
    {
        if (currentDistance < *shortestDistance)
        {
            *shortestDistance = currentDistance;
        }
        cout << "Rute ditemukan dengan jarak: " << currentDistance << " km" << endl;
        cout << "Path: ";
        for (int i = 0; i < pathLength; i++)
        {
            cout << graphNodes[path[i]].nama_lokasi;
            if (i < pathLength - 1)
                cout << " -> ";
        }
        cout << endl;
        return;
    }

    graphNodes[start].visited = true;

    for (int i = 0; i < edgeCount; i++)
    {
        if (graphEdges[i].from == start && !graphNodes[graphEdges[i].to].visited)
        {
            path[pathLength] = graphEdges[i].to;
            dfsRoute(graphEdges[i].to, destination, currentDistance + graphEdges[i].distance,
                     shortestDistance, path, pathLength + 1);
        }
    }

    graphNodes[start].visited = false; // Backtrack
}

void simulateShipping()
{
    cout << "\n=== SIMULASI PENGIRIMAN ===" << endl;
    cout << "Lokasi yang tersedia:" << endl;
    for (int i = 0; i < nodeCount; i++)
    {
        cout << i << ". " << graphNodes[i].nama_lokasi << endl;
    }

    int gudang, tujuan;
    cout << "\nPilih gudang asal (0-" << (nodeCount - 1) << "): ";
    cin >> gudang;
    cout << "Pilih tujuan pengiriman (0-" << (nodeCount - 1) << "): ";
    cin >> tujuan;

    if (gudang < 0 || gudang >= nodeCount || tujuan < 0 || tujuan >= nodeCount)
    {
        cout << "Lokasi tidak valid!" << endl;
        return;
    }

    cout << "\nMencari rute pengiriman dari " << graphNodes[gudang].nama_lokasi
         << " ke " << graphNodes[tujuan].nama_lokasi << endl;

    int shortestDistance = 999999;
    int path[MAX_GRAPH_NODES];
    path[0] = gudang;

    resetGraphVisited();
    dfsRoute(gudang, tujuan, 0, &shortestDistance, path, 1);

    if (shortestDistance == 999999)
    {
        cout << "Tidak ada rute yang tersedia!" << endl;
    }
    else
    {
        cout << "\nRute terpendek: " << shortestDistance << " km" << endl;
    }
}

// Menu Functions
void userMenu(User *user)
{
    int choice;
    do
    {
        cout << "\n=== MENU USER ===" << endl;
        cout << "Selamat datang, " << user->username << "!" << endl;
        cout << "1. Lihat Produk" << endl;
        cout << "2. Browse by Kategori" << endl;
        cout << "3. Cari Produk" << endl;
        cout << "4. Urutkan Produk by Harga" << endl;
        cout << "5. Tambah ke Checkout" << endl;
        cout << "6. Proses Checkout" << endl;
        cout << "7. Lihat Riwayat Transaksi" << endl;
        cout << "8. Tambah Review" << endl;
        cout << "9. Lihat Review Produk" << endl;
        cout << "10. Simulasi Pengiriman" << endl;
        cout << "0. Logout" << endl;
        cout << "Pilihan: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            displayProducts();
            break;
        case 2:
            displayCategoryMenu(0);
            break;
        case 3:
            searchProducts();
            break;
        case 4:
        {
            int sortChoice;
            cout << "1. Ascending (murah ke mahal)" << endl;
            cout << "2. Descending (mahal ke murah)" << endl;
            cout << "Pilih: ";
            cin >> sortChoice;
            sortProductsByPrice(sortChoice == 1);
            break;
        }
        case 5:
            addToCheckout(user->id);
            break;
        case 6:
            processCheckout();
            break;
        case 7:
            displayTransactionHistory(user->id);
            break;
        case 8:
            addReview(user->id, user->username);
            break;
        case 9:
            viewProductReviews();
            break;
        case 10:
            simulateShipping();
            break;
        case 0:
            cout << "Logout berhasil!" << endl;
            break;
        default:
            cout << "Pilihan tidak valid!" << endl;
        }
    } while (choice != 0);
}

void sellerMenu(Seller *seller)
{
    int choice;
    do
    {
        cout << "\n=== MENU SELLER ===" << endl;
        cout << "Selamat datang, " << seller->nama_toko << "!" << endl;
        cout << "1. Tambah Produk" << endl;
        cout << "2. Lihat Semua Produk" << endl;
        cout << "3. Lihat Produk Saya" << endl;
        cout << "0. Logout" << endl;
        cout << "Pilihan: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            addProduct(seller->id);
            break;
        case 2:
            displayProducts();
            break;
        case 3:
        {
            cout << "\n=== PRODUK SAYA ===" << endl;
            Product *current = productHead;
            int count = 1;
            bool found = false;

            while (current != nullptr)
            {
                if (current->seller_id == seller->id)
                {
                    cout << count++ << ". " << current->nama << endl;
                    cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
                    cout << "   Harga: Rp " << current->harga << endl;
                    cout << "   Stok: " << current->stok << endl;
                    cout << "   Rating: " << current->rating << " (" << current->jumlah_review << " reviews)" << endl;
                    cout << endl;
                    found = true;
                }
                current = current->next;
            }

            if (!found)
            {
                cout << "Anda belum memiliki produk." << endl;
            }
            break;
        }
        case 0:
            cout << "Logout berhasil!" << endl;
            break;
        default:
            cout << "Pilihan tidak valid!" << endl;
        }
    } while (choice != 0);
}

void mainMenu()
{
    int choice;
    do
    {
        cout << "\n========================================" << endl;
        cout << "    MARKETPLACE GADGET SIMULATOR" << endl;
        cout << "========================================" << endl;
        cout << "1. Register User" << endl;
        cout << "2. Login User" << endl;
        cout << "3. Register Seller" << endl;
        cout << "4. Login Seller" << endl;
        cout << "5. Lihat Semua Produk (Guest)" << endl;
        cout << "0. Exit" << endl;
        cout << "Pilihan: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            registerUser();
            break;
        case 2:
        {
            User *user = loginUser();
            if (user != nullptr)
            {
                userMenu(user);
            }
            break;
        }
        case 3:
            registerSeller();
            break;
        case 4:
        {
            Seller *seller = loginSeller();
            if (seller != nullptr)
            {
                sellerMenu(seller);
            }
            break;
        }
        case 5:
            displayProducts();
            break;
        case 0:
            cout << "Terima kasih telah menggunakan Marketplace Gadget!" << endl;
            break;
        default:
            cout << "Pilihan tidak valid!" << endl;
        }
    } while (choice != 0);
}

// Function untuk menambahkan data sample
void initializeSampleData()
{
    // Sample Sellers
    int hashIndex = hashFunction("seller1");
    sellerHashTable[hashIndex].id = sellerIdCounter++;
    stringCopy(sellerHashTable[hashIndex].username, "seller1");
    stringCopy(sellerHashTable[hashIndex].password, "pass123");
    stringCopy(sellerHashTable[hashIndex].nama_toko, "Toko Gadget Jakarta");
    sellerHashTable[hashIndex].isActive = true;

    hashIndex = hashFunction("seller2");
    sellerHashTable[hashIndex].id = sellerIdCounter++;
    stringCopy(sellerHashTable[hashIndex].username, "seller2");
    stringCopy(sellerHashTable[hashIndex].password, "pass123");
    stringCopy(sellerHashTable[hashIndex].nama_toko, "Electronic Store Surabaya");
    sellerHashTable[hashIndex].isActive = true;

    // Sample Products
    Product *product1 = new Product;
    product1->id = productIdCounter++;
    product1->seller_id = 1;
    stringCopy(product1->nama, "iPhone 14 Pro Max");
    stringCopy(product1->kategori, "smartphone");
    stringCopy(product1->subkategori, "flagship");
    product1->harga = 18000000;
    product1->stok = 10;
    product1->rating = 4.8;
    product1->jumlah_review = 25;
    product1->next = productHead;
    productHead = product1;

    Product *product2 = new Product;
    product2->id = productIdCounter++;
    product2->seller_id = 1;
    stringCopy(product2->nama, "Samsung Galaxy S23 Ultra");
    stringCopy(product2->kategori, "smartphone");
    stringCopy(product2->subkategori, "flagship");
    product2->harga = 16500000;
    product2->stok = 8;
    product2->rating = 4.7;
    product2->jumlah_review = 18;
    product2->next = productHead;
    productHead = product2;

    Product *product3 = new Product;
    product3->id = productIdCounter++;
    product3->seller_id = 2;
    stringCopy(product3->nama, "iPad Air 5th Gen");
    stringCopy(product3->kategori, "tablet");
    stringCopy(product3->subkategori, "premium");
    product3->harga = 9500000;
    product3->stok = 15;
    product3->rating = 4.6;
    product3->jumlah_review = 12;
    product3->next = productHead;
    productHead = product3;

    Product *product4 = new Product;
    product4->id = productIdCounter++;
    product4->seller_id = 2;
    stringCopy(product4->nama, "MacBook Air M2");
    stringCopy(product4->kategori, "laptop");
    stringCopy(product4->subkategori, "ultrabook");
    product4->harga = 15900000;
    product4->stok = 5;
    product4->rating = 4.9;
    product4->jumlah_review = 30;
    product4->next = productHead;
    productHead = product4;

    Product *product5 = new Product;
    product5->id = productIdCounter++;
    product5->seller_id = 1;
    stringCopy(product5->nama, "Apple Watch Series 8");
    stringCopy(product5->kategori, "smartwatch");
    stringCopy(product5->subkategori, "premium");
    product5->harga = 6500000;
    product5->stok = 20;
    product5->rating = 4.5;
    product5->jumlah_review = 22;
    product5->next = productHead;
    productHead = product5;

    // Sample User
    hashIndex = hashFunction("user1");
    userHashTable[hashIndex].id = userIdCounter++;
    stringCopy(userHashTable[hashIndex].username, "user1");
    stringCopy(userHashTable[hashIndex].password, "pass123");
    stringCopy(userHashTable[hashIndex].alamat, "Jl. Merdeka No. 123, Jakarta");
    userHashTable[hashIndex].isActive = true;

    // Sample Reviews
    reviewRoot = insertReview(reviewRoot, 1, 1, "user1", 5, "iPhone terbaik yang pernah saya beli!");
    reviewRoot = insertReview(reviewRoot, 1, 1, "user1", 4, "Kualitas bagus tapi mahal");
    reviewRoot = insertReview(reviewRoot, 2, 1, "user1", 5, "Samsung Galaxy luar biasa!");

    cout << "Sample data berhasil dimuat!" << endl;
    cout << "Sample login:" << endl;
    cout << "User - username: user1, password: pass123" << endl;
    cout << "Seller - username: seller1, password: pass123" << endl;
    cout << "Seller - username: seller2, password: pass123" << endl;
}

// Memory cleanup functions
void cleanupProducts()
{
    Product *current = productHead;
    while (current != nullptr)
    {
        Product *next = current->next;
        delete current;
        current = next;
    }
}

void cleanupReviews(Review *root)
{
    if (root != nullptr)
    {
        cleanupReviews(root->left);
        cleanupReviews(root->right);
        delete root;
    }
}

// Main Function
int main()
{
    cout << "\nInisialisasi sistem..." << endl;

    // Inisialisasi semua struktur data
    initializeHashTables();
    initializeGraph();

    // Load sample data
    initializeSampleData();

    cout << "\nSistem siap digunakan!" << endl;

    // Jalankan menu utama
    mainMenu();

    // Cleanup memory
    cleanupProducts();
    cleanupReviews(reviewRoot);

    cout << "Program selesai. Terima kasih!" << endl;

    return 0;
}