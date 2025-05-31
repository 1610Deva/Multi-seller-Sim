#include <iostream>
#include <string>
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
    char lokasi_toko[100];
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
    int seller_id;
    int product_id;
    char nama_produk[100];
    double harga;
    int quantity;
    char tanggal[20];
    bool isShipped;
    bool isReceived;
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

// Fungsi untuk menyimpan jalur terbaik
struct RouteInfo
{
    int path[MAX_GRAPH_NODES];
    int pathLength;
    int totalDistance;
};

RouteInfo bestRoute;
bool routeFound = false;

// Hash Function
int hashFunction(const char *key)
{
    int hash = 0;
    int len = 0;
    while (key[len] != '\0')
        len++;

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

// Helper function untuk string repeat - PINDAHKAN KE SINI
string repeatChar(int count, char ch)
{
    string result = "";
    for (int i = 0; i < count; i++)
    {
        result += ch;
    }
    return result;
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
    char username[50], password[50], nama_toko[100], lokasi_toko[100];

    cout << "\n=== REGISTRASI SELLER ===" << endl;
    cout << "Username: ";
    cin >> username;
    cout << "Password: ";
    cin >> password;
    cout << "Nama Toko: ";
    cin.ignore();
    cin.getline(nama_toko, 100);
    cout << "Lokasi Toko: ";
    cin.getline(lokasi_toko, 100);

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
    stringCopy(sellerHashTable[hashIndex].lokasi_toko, lokasi_toko);
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

void cancelTransaction(int user_id)
{
    int transId;
    cout << "Masukkan ID Transaksi yang ingin dibatalkan: ";
    cin >> transId;

    bool found = false;
    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].id == transId && transactionStack[i].user_id == user_id)
        {
            if (transactionStack[i].isShipped)
            {
                cout << "Transaksi sudah dikirim, tidak bisa dibatalkan!" << endl;
                return;
            }
            // Kembalikan stok produk
            Product *current = productHead;
            while (current != nullptr)
            {
                if (current->id == transactionStack[i].product_id)
                {
                    current->stok += transactionStack[i].quantity;
                    break;
                }
                current = current->next;
            }
            // Hapus transaksi (geser stack)
            for (int j = i; j < stackTop; j++)
            {
                transactionStack[j] = transactionStack[j + 1];
            }
            stackTop--;
            cout << "Transaksi berhasil dibatalkan dan dana direfund!" << endl;
            found = true;
            break;
        }
    }
    if (!found)
        cout << "Transaksi tidak ditemukan!" << endl;
}

void showSellerReport(int seller_id)
{
    int totalProduk = 0;
    double totalPendapatan = 0;

    for (int i = 0; i <= stackTop; i++)
    {
        Product *p = productHead;
        while (p != nullptr)
        {
            if (p->id == transactionStack[i].product_id && p->seller_id == seller_id)
            {
                totalProduk += transactionStack[i].quantity;
                totalPendapatan += (transactionStack[i].harga * transactionStack[i].quantity);
            }
            p = p->next;
        }
    }

    cout << "\n=== LAPORAN PENJUALAN ===" << endl;
    cout << "Total Produk Terjual: " << totalProduk << endl;
    cout << "Total Pendapatan: Rp " << totalPendapatan << endl;
}

// Product Management Functions (Linked List)
void addProduct(int seller_id)
{
    char nama[100], kategori[50], subkategori[50];
    double harga;
    int stok;

    cout << "\n"
         << repeatChar(70, '=') << endl;
    cout << "                   TAMBAH PRODUK BARU" << endl;
    cout << repeatChar(70, '=') << endl;

    // Cari nama toko seller
    char nama_toko[100] = "Toko Anda";
    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (sellerHashTable[i].isActive && sellerHashTable[i].id == seller_id)
        {
            stringCopy(nama_toko, sellerHashTable[i].nama_toko);
            break;
        }
    }

    cout << "Toko: " << nama_toko << endl;
    cout << "Seller ID: " << seller_id << endl;
    cout << repeatChar(70, '-') << endl;

    // Input nama produk
    cout << "\n[1/5] INFORMASI PRODUK" << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "Masukkan nama produk: ";
    cin.ignore(); // Bersihkan buffer
    cin.getline(nama, 100);

    // Input kategori dengan pilihan
    cout << "\n[2/5] KATEGORI PRODUK" << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "Pilih kategori produk:" << endl;
    cout << "1. Smartphone" << endl;
    cout << "2. Tablet" << endl;
    cout << "3. Laptop" << endl;
    cout << "4. Smartwatch" << endl;
    cout << "5. Audio (Headphone/Speaker)" << endl;
    cout << "6. Aksesoris" << endl;
    cout << repeatChar(40, '-') << endl;

    int kategoriChoice;
    cout << "Pilih kategori (1-6): ";
    cin >> kategoriChoice;

    switch (kategoriChoice)
    {
    case 1:
        stringCopy(kategori, "smartphone");
        break;
    case 2:
        stringCopy(kategori, "tablet");
        break;
    case 3:
        stringCopy(kategori, "laptop");
        break;
    case 4:
        stringCopy(kategori, "smartwatch");
        break;
    case 5:
        stringCopy(kategori, "audio");
        break;
    case 6:
        stringCopy(kategori, "aksesoris");
        break;
    default:
        cout << "Kategori tidak valid! Menggunakan 'smartphone' sebagai default." << endl;
        stringCopy(kategori, "smartphone");
        break;
    }

    // Input subkategori berdasarkan kategori yang dipilih
    cout << "\n[3/5] SUBKATEGORI PRODUK" << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "Pilih subkategori untuk " << kategori << ":" << endl;

    int subkategoriChoice;

    if (stringCompare(kategori, "smartphone") == 0)
    {
        cout << "1. Flagship" << endl;
        cout << "2. Mid Range" << endl;
        cout << "3. Budget" << endl;
        cout << "4. Gaming Phone" << endl;
        cout << "Pilih subkategori (1-4): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "flagship");
            break;
        case 2:
            stringCopy(subkategori, "mid_range");
            break;
        case 3:
            stringCopy(subkategori, "budget");
            break;
        case 4:
            stringCopy(subkategori, "gaming");
            break;
        default:
            stringCopy(subkategori, "mid_range");
            break;
        }
    }
    else if (stringCompare(kategori, "tablet") == 0)
    {
        cout << "1. Premium" << endl;
        cout << "2. Standard" << endl;
        cout << "3. Budget" << endl;
        cout << "Pilih subkategori (1-3): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "premium");
            break;
        case 2:
            stringCopy(subkategori, "standard");
            break;
        case 3:
            stringCopy(subkategori, "budget");
            break;
        default:
            stringCopy(subkategori, "standard");
            break;
        }
    }
    else if (stringCompare(kategori, "laptop") == 0)
    {
        cout << "1. Gaming" << endl;
        cout << "2. Ultrabook" << endl;
        cout << "3. Workstation" << endl;
        cout << "4. Budget" << endl;
        cout << "Pilih subkategori (1-4): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "gaming");
            break;
        case 2:
            stringCopy(subkategori, "ultrabook");
            break;
        case 3:
            stringCopy(subkategori, "workstation");
            break;
        case 4:
            stringCopy(subkategori, "budget");
            break;
        default:
            stringCopy(subkategori, "ultrabook");
            break;
        }
    }
    else if (stringCompare(kategori, "smartwatch") == 0)
    {
        cout << "1. Premium" << endl;
        cout << "2. Fitness" << endl;
        cout << "3. Basic" << endl;
        cout << "Pilih subkategori (1-3): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "premium");
            break;
        case 2:
            stringCopy(subkategori, "fitness");
            break;
        case 3:
            stringCopy(subkategori, "basic");
            break;
        default:
            stringCopy(subkategori, "fitness");
            break;
        }
    }
    else if (stringCompare(kategori, "audio") == 0)
    {
        cout << "1. Headphone" << endl;
        cout << "2. Earphone" << endl;
        cout << "3. Speaker" << endl;
        cout << "4. TWS" << endl;
        cout << "Pilih subkategori (1-4): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "headphone");
            break;
        case 2:
            stringCopy(subkategori, "earphone");
            break;
        case 3:
            stringCopy(subkategori, "speaker");
            break;
        case 4:
            stringCopy(subkategori, "tws");
            break;
        default:
            stringCopy(subkategori, "earphone");
            break;
        }
    }
    else // aksesoris
    {
        cout << "1. Case" << endl;
        cout << "2. Charger" << endl;
        cout << "3. Power Bank" << endl;
        cout << "4. Cable" << endl;
        cout << "Pilih subkategori (1-4): ";
        cin >> subkategoriChoice;

        switch (subkategoriChoice)
        {
        case 1:
            stringCopy(subkategori, "case");
            break;
        case 2:
            stringCopy(subkategori, "charger");
            break;
        case 3:
            stringCopy(subkategori, "powerbank");
            break;
        case 4:
            stringCopy(subkategori, "cable");
            break;
        default:
            stringCopy(subkategori, "case");
            break;
        }
    }

    // Input harga
    cout << "\n[4/5] INFORMASI HARGA & STOK" << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "Masukkan harga produk (Rp): ";
    cin >> harga;

    // Validasi harga
    while (harga <= 0)
    {
        cout << "[ERROR] Harga harus lebih dari 0!" << endl;
        cout << "Masukkan harga yang valid (Rp): ";
        cin >> harga;
    }

    // Input stok
    cout << "Masukkan jumlah stok: ";
    cin >> stok;

    // Validasi stok
    while (stok < 0)
    {
        cout << "[ERROR] Stok tidak boleh negatif!" << endl;
        cout << "Masukkan stok yang valid: ";
        cin >> stok;
    }

    // Konfirmasi sebelum menyimpan
    cout << "\n[5/5] KONFIRMASI PRODUK" << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "DETAIL PRODUK YANG AKAN DITAMBAHKAN:" << endl;
    cout << repeatChar(70, '-') << endl;
    cout << "Nama Produk  : " << nama << endl;
    cout << "Kategori     : " << kategori << endl;
    cout << "Subkategori  : " << subkategori << endl;
    cout << "Harga        : Rp " << harga << endl;
    cout << "Stok         : " << stok << " unit" << endl;
    cout << "Toko         : " << nama_toko << endl;
    cout << repeatChar(70, '-') << endl;

    char konfirmasi;
    cout << "Apakah informasi sudah benar? (y/n): ";
    cin >> konfirmasi;

    if (konfirmasi != 'y' && konfirmasi != 'Y')
    {
        cout << "\n"
             << repeatChar(50, '=') << endl;
        cout << "            PRODUK DIBATALKAN" << endl;
        cout << repeatChar(50, '=') << endl;
        cout << "Produk tidak jadi ditambahkan." << endl;
        cout << "Anda dapat mencoba lagi kapan saja." << endl;
        cout << repeatChar(50, '=') << endl;
        return;
    }

    // Buat produk baru
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

    // Tampilkan pesan sukses
    cout << "\n"
         << repeatChar(70, '=') << endl;
    cout << "                PRODUK BERHASIL DITAMBAHKAN!" << endl;
    cout << repeatChar(70, '=') << endl;
    cout << "Produk ID    : " << newProduct->id << endl;
    cout << "Nama         : " << nama << endl;
    cout << "Kategori     : " << kategori << " - " << subkategori << endl;
    cout << "Harga        : Rp " << harga << endl;
    cout << "Stok         : " << stok << " unit" << endl;
    cout << "Status       : Aktif dan siap dijual" << endl;
    cout << repeatChar(70, '=') << endl;
    cout << "\nTips:" << endl;
    cout << "• Produk Anda sekarang dapat dilihat oleh pembeli" << endl;
    cout << "• Pantau order masuk melalui menu 'Lihat Order Masuk'" << endl;
    cout << "• Update stok secara berkala agar tidak kehabisan" << endl;
    cout << "• Berikan layanan terbaik untuk mendapat review positif" << endl;
    cout << repeatChar(70, '=') << endl;
}

// void displayProducts()
// {
//     cout << "\n=== DAFTAR PRODUK ===" << endl;
//     Product *current = productHead;
//     int count = 1;

//     while (current != nullptr)
//     {
//         cout << count++ << ". " << current->nama << endl;
//         cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
//         cout << "   Harga: Rp " << current->harga << endl;
//         cout << "   Stok: " << current->stok << endl;
//         cout << "   Rating: " << current->rating << " (" << current->jumlah_review << " reviews)" << endl;
//         cout << "   ID: " << current->id << endl;
//         cout << endl;
//         current = current->next;
//     }
// }

void displayProducts()
{
    cout << "\n=== DAFTAR PRODUK ===" << endl;
    Product *current = productHead;
    int count = 1;

    while (current != nullptr)
    {
        // Mencari informasi seller berdasarkan seller_id
        char nama_toko[100] = "Tidak diketahui";
        char lokasi[100] = "Tidak diketahui";

        // Mencari seller di hash table untuk mendapatkan nama toko
        for (int i = 0; i < HASH_SIZE; i++)
        {
            if (sellerHashTable[i].isActive && sellerHashTable[i].id == current->seller_id)
            {
                stringCopy(nama_toko, sellerHashTable[i].nama_toko);

                // Lokasi toko dapat diambil dari alamat seller atau di-hardcode
                // Di sini kita mengasumsikan lokasinya berdasarkan nama toko
                if (stringContains(sellerHashTable[i].nama_toko, "Jakarta"))
                {
                    stringCopy(lokasi, "Jakarta");
                }
                else if (stringContains(sellerHashTable[i].nama_toko, "Surabaya"))
                {
                    stringCopy(lokasi, "Surabaya");
                }
                // Tambahkan kondisi lain sesuai kebutuhan
                break;
            }
        }

        cout << count++ << ". " << current->nama << endl;
        cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
        cout << "   Harga: Rp " << current->harga << endl;
        cout << "   Stok: " << current->stok << endl;
        cout << "   Rating: " << current->rating << " (" << current->jumlah_review << " reviews)" << endl;
        cout << "   ID: " << current->id << endl;
        cout << "   Toko: " << nama_toko << endl;
        cout << "   Lokasi: " << lokasi << endl;
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
        cout << "\n"
             << repeatChar(50, '=') << endl;
        cout << "           CHECKOUT KOSONG" << endl;
        cout << repeatChar(50, '=') << endl;
        cout << "Tidak ada item dalam antrian checkout!" << endl;
        cout << "Silakan tambahkan produk ke checkout terlebih dahulu." << endl;
        cout << "Gunakan menu 'Tambah ke Checkout' untuk menambah produk." << endl;
        return;
    }

    // Tampilkan semua item dalam antrian checkout
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "                  DETAIL CHECKOUT" << endl;
    cout << repeatChar(60, '=') << endl;

    // Hitung total keseluruhan dan tampilkan detail setiap item
    double totalKeseluruhan = 0;
    int tempFront = queueFront;
    int tempSize = queueSize;
    int itemNumber = 1;

    cout << "ITEM YANG AKAN DIBELI:" << endl;
    cout << repeatChar(60, '-') << endl;

    for (int i = 0; i < tempSize; i++)
    {
        int currentIndex = (tempFront + i) % MAX_QUEUE;
        QueueItem currentItem = checkoutQueue[currentIndex];

        // Cari detail produk
        Product *current = productHead;
        while (current != nullptr)
        {
            if (current->id == currentItem.product_id)
            {
                // Cari informasi seller
                char nama_toko[100] = "Tidak diketahui";
                for (int j = 0; j < HASH_SIZE; j++)
                {
                    if (sellerHashTable[j].isActive && sellerHashTable[j].id == current->seller_id)
                    {
                        stringCopy(nama_toko, sellerHashTable[j].nama_toko);
                        break;
                    }
                }

                cout << itemNumber++ << ". " << current->nama << endl;
                cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
                cout << "   Harga Satuan: Rp " << current->harga << endl;
                cout << "   Jumlah: " << currentItem.quantity << " unit" << endl;
                cout << "   Subtotal: Rp " << currentItem.total_harga << endl;
                cout << "   Dijual oleh: " << nama_toko << endl;
                cout << "   Rating: " << current->rating << "/5 (" << current->jumlah_review << " reviews)" << endl;

                totalKeseluruhan += currentItem.total_harga;
                break;
            }
            current = current->next;
        }
        cout << repeatChar(60, '-') << endl;
    }

    // Hitung biaya tambahan
    double biayaAdmin = 2500;
    double biayaPengiriman = 15000;
    double totalAkhir = totalKeseluruhan + biayaAdmin + biayaPengiriman;

    cout << "\nRINGKASAN PEMBAYARAN:" << endl;
    cout << "Subtotal Produk    : Rp " << totalKeseluruhan << endl;
    cout << "Biaya Admin        : Rp " << biayaAdmin << endl;
    cout << "Biaya Pengiriman   : Rp " << biayaPengiriman << endl;
    cout << repeatChar(40, '-') << endl;
    cout << "TOTAL PEMBAYARAN   : Rp " << totalAkhir << endl;
    cout << repeatChar(60, '=') << endl;

    // Pilihan metode pembayaran
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               PILIH METODE PEMBAYARAN" << endl;
    cout << repeatChar(60, '=') << endl;
    cout << "1. Transfer Bank (BCA, Mandiri, BNI)" << endl;
    cout << "2. E-Wallet (OVO, GoPay, DANA)" << endl;
    cout << "3. Virtual Account" << endl;
    cout << "4. Credit/Debit Card" << endl;
    cout << "5. COD (Cash on Delivery)" << endl;
    cout << "0. Batal Checkout" << endl;
    cout << repeatChar(60, '-') << endl;
    cout << "Pilih metode pembayaran (1-5): ";

    int metodePembayaran;
    cin >> metodePembayaran;

    if (metodePembayaran == 0)
    {
        cout << "\nCheckout dibatalkan." << endl;
        return;
    }

    if (metodePembayaran < 1 || metodePembayaran > 5)
    {
        cout << "\nMetode pembayaran tidak valid!" << endl;
        return;
    }

    // Konfirmasi pembayaran
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               KONFIRMASI PEMBAYARAN" << endl;
    cout << repeatChar(60, '=') << endl;

    const char *namaMetode[] = {"", "Transfer Bank", "E-Wallet", "Virtual Account", "Credit/Debit Card", "COD"};
    cout << "Metode Pembayaran: " << namaMetode[metodePembayaran] << endl;
    cout << "Total Pembayaran: Rp " << totalAkhir << endl;

    // Detail pembayaran berdasarkan metode
    switch (metodePembayaran)
    {
    case 1:
        cout << "\nDetail Transfer Bank:" << endl;
        cout << "- BCA: 1234567890 a.n. Marketplace Gadget" << endl;
        cout << "- Mandiri: 9876543210 a.n. Marketplace Gadget" << endl;
        cout << "- BNI: 5555666677 a.n. Marketplace Gadget" << endl;
        cout << "- Kirim bukti transfer ke admin" << endl;
        break;
    case 2:
        cout << "\nDetail E-Wallet:" << endl;
        cout << "- Scan QR Code yang akan dikirim ke WhatsApp/Email Anda" << endl;
        cout << "- Atau bayar melalui aplikasi dengan kode: MGS" << totalAkhir << endl;
        break;
    case 3:
        cout << "\nDetail Virtual Account:" << endl;
        cout << "- Nomor VA: 80888" << (1000000 + rand() % 9000000) << endl;
        cout << "- Bayar melalui ATM, mobile banking, atau internet banking" << endl;
        cout << "- VA berlaku 24 jam" << endl;
        break;
    case 4:
        cout << "\nDetail Card Payment:" << endl;
        cout << "- Link pembayaran akan dikirim ke email Anda" << endl;
        cout << "- Atau bayar melalui gateway yang tersedia" << endl;
        cout << "- Pembayaran aman dengan enkripsi SSL" << endl;
        break;
    case 5:
        cout << "\nDetail COD:" << endl;
        cout << "- Bayar tunai saat barang sampai" << endl;
        cout << "- Biaya COD: Rp 5000 (sudah termasuk dalam total)" << endl;
        cout << "- Pastikan menyiapkan uang pas" << endl;
        totalAkhir += 5000; // Tambah biaya COD
        break;
    }

    cout << "\nApakah Anda yakin ingin melanjutkan pembayaran? (y/n): ";
    char konfirmasi;
    cin >> konfirmasi;

    if (konfirmasi != 'y' && konfirmasi != 'Y')
    {
        cout << "\nPembayaran dibatalkan." << endl;
        return;
    }

    // Proses semua item dalam queue
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               MEMPROSES PESANAN" << endl;
    cout << repeatChar(60, '=') << endl;

    bool semuaBerhasil = true;
    int jumlahItemDiproses = 0;

    while (queueSize > 0)
    {
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
                        trans.seller_id = current->seller_id;
                        trans.product_id = item.product_id;
                        stringCopy(trans.nama_produk, current->nama);
                        trans.harga = current->harga;
                        trans.quantity = item.quantity;
                        stringCopy(trans.tanggal, "2024-12-20"); // Tanggal hari ini
                        trans.isShipped = false;                 // Belum dikirim
                        trans.isReceived = false; // Belum diterima

                        if (push(trans))
                        {
                            cout << " [OK] " << current->nama << " - " << item.quantity << " unit berhasil diproses" << endl;
                            jumlahItemDiproses++;
                        }
                        else
                        {
                            cout << "[GAGAL] Gagal memproses " << current->nama << endl;
                            semuaBerhasil = false;
                            // Kembalikan stok jika gagal
                            current->stok += item.quantity;
                        }
                    }
                    else
                    {
                        cout << "[ERROR] Stok " << current->nama << " tidak mencukupi!" << endl;
                        semuaBerhasil = false;
                    }
                    break;
                }
                current = current->next;
            }
        }
    }

    // Tampilkan hasil akhir
    if (semuaBerhasil && jumlahItemDiproses > 0)
    {
        cout << "\n"
             << repeatChar(60, '=') << endl;
        cout << "               CHECKOUT BERHASIL!" << endl;
        cout << repeatChar(60, '=') << endl;
        cout << "Jumlah item diproses: " << jumlahItemDiproses << " item" << endl;
        cout << "Total pembayaran: Rp " << totalAkhir << endl;
        cout << "Metode pembayaran: " << namaMetode[metodePembayaran] << endl;
        cout << "\nStatus Pesanan:" << endl;
        cout << "- Pesanan sedang diproses oleh seller" << endl;
        cout << "- Anda akan mendapat notifikasi saat barang dikirim" << endl;
        cout << "- Cek riwayat transaksi untuk detail lebih lanjut" << endl;
        cout << "\nTerima kasih telah berbelanja di Marketplace Gadget!" << endl;
    }
    else
    {
        cout << "\n"
             << repeatChar(60, '=') << endl;
        cout << "               CHECKOUT GAGAL!" << endl;
        cout << repeatChar(60, '=') << endl;
        cout << "Beberapa item tidak dapat diproses." << endl;
        cout << "Silakan cek kembali stok produk yang tersedia." << endl;
    }

    cout << repeatChar(60, '=') << endl;
}

void confirmPackageReceived(int user_id)
{
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               KONFIRMASI PENERIMAAN PAKET" << endl;
    cout << repeatChar(60, '=') << endl;

    // Tampilkan daftar paket yang sudah dikirim tapi belum dikonfirmasi
    cout << "DAFTAR PAKET YANG SUDAH DIKIRIM:" << endl;
    cout << repeatChar(60, '-') << endl;

    bool foundShippedPackages = false;
    int displayNumber = 1;

    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].user_id == user_id &&
            transactionStack[i].isShipped &&
            !transactionStack[i].isReceived)
        {
            foundShippedPackages = true;
            cout << displayNumber++ << ". ID Transaksi: " << transactionStack[i].id << endl;
            cout << "   Produk: " << transactionStack[i].nama_produk << endl;
            cout << "   Quantity: " << transactionStack[i].quantity << " unit" << endl;
            cout << "   Total: Rp " << (transactionStack[i].harga * transactionStack[i].quantity) << endl;
            cout << "   Tanggal: " << transactionStack[i].tanggal << endl;
            cout << "   Status: Sedang dalam pengiriman" << endl;
            cout << repeatChar(60, '-') << endl;
        }
    }

    if (!foundShippedPackages)
    {
        cout << "Tidak ada paket yang sedang dalam pengiriman." << endl;
        cout << "Semua paket sudah dikonfirmasi atau belum dikirim." << endl;
        cout << repeatChar(60, '=') << endl;
        return;
    }

    // Input ID transaksi untuk konfirmasi
    int transId;
    cout << "\nMasukkan ID Transaksi yang paketnya sudah diterima: ";
    cin >> transId;

    // Cari transaksi
    bool found = false;
    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].id == transId &&
            transactionStack[i].user_id == user_id)
        {
            if (!transactionStack[i].isShipped)
            {
                cout << "\n[GAGAL] KONFIRMASI GAGAL!" << endl;
                cout << "Paket belum dikirim oleh seller." << endl;
                cout << "Status saat ini: Menunggu pengiriman" << endl;
                return;
            }

            if (transactionStack[i].isReceived)
            {
                cout << "\n[INFO] PAKET SUDAH DIKONFIRMASI!" << endl;
                cout << "Paket untuk transaksi ID " << transId << " sudah dikonfirmasi sebelumnya." << endl;
                return;
            }

            // Konfirmasi penerimaan
            cout << "\n"
                 << repeatChar(60, '=') << endl;
            cout << "               DETAIL KONFIRMASI" << endl;
            cout << repeatChar(60, '=') << endl;
            cout << "ID Transaksi: " << transactionStack[i].id << endl;
            cout << "Produk: " << transactionStack[i].nama_produk << endl;
            cout << "Quantity: " << transactionStack[i].quantity << " unit" << endl;
            cout << "Total Pembayaran: Rp " << (transactionStack[i].harga * transactionStack[i].quantity) << endl;
            cout << repeatChar(60, '-') << endl;

            char konfirmasi;
            cout << "Apakah paket sudah diterima dengan baik? (y/n): ";
            cin >> konfirmasi;

            if (konfirmasi == 'y' || konfirmasi == 'Y')
            {
                transactionStack[i].isReceived = true;

                cout << "\n"
                     << repeatChar(60, '=') << endl;
                cout << "               KONFIRMASI BERHASIL!" << endl;
                cout << repeatChar(60, '=') << endl;
                cout << "[OK] Paket telah dikonfirmasi diterima!" << endl;
                cout << "[OK] Transaksi ID " << transId << " selesai!" << endl;
                cout << "\nTerima kasih telah berbelanja di Marketplace Gadget!" << endl;
                cout << "\nSaran:" << endl;
                cout << "• Berikan review untuk produk yang sudah diterima" << endl;
                cout << "• Simpan bukti transaksi untuk keperluan garansi" << endl;
                cout << "• Hubungi seller jika ada masalah dengan produk" << endl;

                found = true;
            }
            else
            {
                cout << "\n[BATAL] Konfirmasi dibatalkan." << endl;
                cout << "Jika ada masalah dengan paket, silakan hubungi seller atau customer service." << endl;
            }
            break;
        }
    }

    if (!found)
    {
        cout << "\n[ERROR] TRANSAKSI TIDAK DITEMUKAN!" << endl;
        cout << "ID Transaksi " << transId << " tidak ditemukan atau bukan milik Anda." << endl;
        cout << "Pastikan ID transaksi yang dimasukkan benar." << endl;
    }

    cout << repeatChar(60, '=') << endl;
}

void displayTransactionHistory(int user_id)
{
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               RIWAYAT TRANSAKSI" << endl;
    cout << repeatChar(60, '=') << endl;

    bool found = false;
    int displayNumber = 1;

    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].user_id == user_id)
        {
            cout << displayNumber++ << ". ID Transaksi: " << transactionStack[i].id << endl;
            cout << "   Produk: " << transactionStack[i].nama_produk << endl;
            cout << "   Harga Satuan: Rp " << transactionStack[i].harga << endl;
            cout << "   Quantity: " << transactionStack[i].quantity << " unit" << endl;
            cout << "   Total: Rp " << (transactionStack[i].harga * transactionStack[i].quantity) << endl;
            cout << "   Tanggal: " << transactionStack[i].tanggal << endl;

            // Status yang lebih detail
            if (!transactionStack[i].isShipped)
            {
                cout << "   Status: [PENDING] Menunggu pengiriman dari seller" << endl;
            }
            else if (transactionStack[i].isShipped && !transactionStack[i].isReceived)
            {
                cout << "   Status: [SHIPPING] Sedang dalam pengiriman" << endl;
                cout << "   [ACTION REQUIRED] Siap untuk dikonfirmasi penerimaan" << endl;
            }
            else if (transactionStack[i].isReceived)
            {
                cout << "   Status: [COMPLETED] Paket diterima & transaksi selesai" << endl;
            }

            cout << repeatChar(60, '-') << endl;
            found = true;
        }
    }

    if (!found)
    {
        cout << "Belum ada transaksi." << endl;
        cout << "Mulai berbelanja untuk melihat riwayat transaksi!" << endl;
    }
    else
    {
        cout << "\nLegenda Status:" << endl;
        cout << "[PENDING] = Menunggu seller mengirim paket" << endl;
        cout << "[SHIPPING] = Paket dalam perjalanan (bisa dikonfirmasi)" << endl;
        cout << "[COMPLETED] = Transaksi selesai" << endl;
    }

    cout << repeatChar(60, '=') << endl;
}

void showSellerOrders(int seller_id)
{
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "               DAFTAR ORDER MASUK" << endl;
    cout << repeatChar(60, '=') << endl;

    bool found = false;
    int displayNumber = 1;

    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].seller_id == seller_id)
        {
            cout << displayNumber++ << ". ID Transaksi: " << transactionStack[i].id << endl;
            cout << "   Produk: " << transactionStack[i].nama_produk << endl;
            cout << "   Quantity: " << transactionStack[i].quantity << " unit" << endl;
            cout << "   Total: Rp " << (transactionStack[i].harga * transactionStack[i].quantity) << endl;
            cout << "   Tanggal: " << transactionStack[i].tanggal << endl;

            // Status lebih detail untuk seller
            if (!transactionStack[i].isShipped)
            {
                cout << "   Status: [PENDING] Perlu dikirim" << endl;
            }
            else if (transactionStack[i].isShipped && !transactionStack[i].isReceived)
            {
                cout << "   Status: [SHIPPING] Sedang dikirim (menunggu konfirmasi user)" << endl;
            }
            else if (transactionStack[i].isReceived)
            {
                cout << "   Status: [COMPLETED] Diterima user & transaksi selesai" << endl;
            }

            cout << repeatChar(60, '-') << endl;
            found = true;
        }
    }

    if (!found)
    {
        cout << "Belum ada order masuk." << endl;
        cout << "Promosikan toko Anda untuk mendapat lebih banyak order!" << endl;
    }

    cout << repeatChar(60, '=') << endl;
}

void updateShippingStatus(int seller_id)
{
    int transId;
    cout << "\nMasukkan ID Transaksi yang ingin diupdate statusnya: ";
    cin >> transId;

    for (int i = stackTop; i >= 0; i--)
    {
        if (transactionStack[i].id == transId && transactionStack[i].seller_id == seller_id)
        {
            if (transactionStack[i].isShipped)
            {
                cout << "Order sudah dikirim!" << endl;
                return;
            }
            transactionStack[i].isShipped = true;
            cout << "Status pengiriman berhasil diupdate menjadi 'Sedang dikirim'!" << endl;
            return;
        }
    }
    cout << "Transaksi tidak ditemukan!" << endl;
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


// Juga, mari kita perbaiki fungsi displayReviews untuk tampilan yang lebih baik
void displayReviews(Review *root, int product_id)
{
    if (root != nullptr)
    {
        displayReviews(root->right, product_id); // Tampilkan rating tertinggi dulu

        if (root->product_id == product_id)
        {
            // Tampilan review yang lebih sederhana tanpa karakter Unicode
            cout << "+" << repeatChar(58, '-') << "+" << endl;
            cout << "| User: " << root->username;

            // Padding untuk alignment
            int userLen = stringLength(root->username);
            for (int i = userLen; i < 51; i++)
            {
                cout << " ";
            }
            cout << "|" << endl;

            cout << "| Rating: " << root->rating << "/5";

            // Tampilkan bintang visual
            cout << " (";
            for (int i = 0; i < root->rating; i++)
            {
                cout << "*";
            }
            for (int i = root->rating; i < 5; i++)
            {
                cout << "o";
            }
            cout << ")";

            // Padding
            int ratingLen = 12 + 7; // "Rating: X/5 (***oo)"
            for (int i = ratingLen; i < 58; i++)
            {
                cout << " ";
            }
            cout << "|" << endl;

            cout << "| Komentar:";
            for (int i = 0; i < 49; i++)
                cout << " ";
            cout << "|" << endl;

            // Split komentar jika terlalu panjang
            char comment[200];
            stringCopy(comment, root->komentar);
            int commentLen = stringLength(comment);

            if (commentLen <= 56)
            {
                cout << "| " << comment;
                for (int i = commentLen; i < 56; i++)
                {
                    cout << " ";
                }
                cout << "|" << endl;
            }
            else
            {
                // Split komentar ke beberapa baris
                int pos = 0;
                while (pos < commentLen)
                {
                    cout << "| ";
                    int lineLen = 0;
                    while (pos < commentLen && lineLen < 56)
                    {
                        cout << comment[pos++];
                        lineLen++;
                    }
                    for (int i = lineLen; i < 56; i++)
                    {
                        cout << " ";
                    }
                    cout << "|" << endl;
                }
            }

            cout << "+" << repeatChar(58, '-') << "+" << endl;
            cout << endl;
        }

        displayReviews(root->left, product_id);
    }
}

void viewProductReviews()
{
    int product_id;
    cout << "\nMasukkan ID produk: ";
    cin >> product_id;

    // Cari dan tampilkan informasi produk terlebih dahulu
    Product *current = productHead;
    bool productFound = false;

    while (current != nullptr)
    {
        if (current->id == product_id)
        {
            productFound = true;

            // Mencari informasi seller berdasarkan seller_id
            char nama_toko[100] = "Tidak diketahui";
            char lokasi[100] = "Tidak diketahui";

            for (int i = 0; i < HASH_SIZE; i++)
            {
                if (sellerHashTable[i].isActive && sellerHashTable[i].id == current->seller_id)
                {
                    stringCopy(nama_toko, sellerHashTable[i].nama_toko);

                    if (stringContains(sellerHashTable[i].nama_toko, "Jakarta"))
                    {
                        stringCopy(lokasi, "Jakarta");
                    }
                    else if (stringContains(sellerHashTable[i].nama_toko, "Surabaya"))
                    {
                        stringCopy(lokasi, "Surabaya");
                    }
                    else if (stringLength(sellerHashTable[i].lokasi_toko) > 0)
                    {
                        stringCopy(lokasi, sellerHashTable[i].lokasi_toko);
                    }
                    break;
                }
            }

            // Tampilkan informasi produk
            cout << "\n"
                 << repeatChar(60, '=') << endl;
            cout << "           INFORMASI PRODUK" << endl;
            cout << repeatChar(60, '=') << endl;
            cout << "Nama Produk    : " << current->nama << endl;
            cout << "Kategori      : " << current->kategori << " - " << current->subkategori << endl;
            cout << "Harga         : Rp " << current->harga << endl;
            cout << "Stok          : " << current->stok << " unit" << endl;
            cout << "Rating        : " << current->rating << "/5.0" << endl;
            cout << "Jumlah Review : " << current->jumlah_review << " review(s)" << endl;
            cout << "Toko          : " << nama_toko << endl;
            cout << "Lokasi        : " << lokasi << endl;
            cout << "ID Produk     : " << current->id << endl;
            cout << repeatChar(60, '=') << endl;
            break;
        }
        current = current->next;
    }

    if (!productFound)
    {
        cout << "\nProduk dengan ID " << product_id << " tidak ditemukan!" << endl;
        cout << "Tip: Gunakan menu 'Lihat Produk' untuk melihat daftar ID produk yang tersedia." << endl;
        return;
    }

    // Tampilkan review produk
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "           REVIEW & RATING PRODUK" << endl;
    cout << repeatChar(60, '=') << endl;

    // Cek apakah ada review untuk produk ini
    bool reviewExists = false;
    Review *stack[MAX_REVIEWS];
    int stackTopLocal = -1; // Gunakan nama yang berbeda dari stackTop global

    if (reviewRoot != nullptr)
    {
        stack[++stackTopLocal] = reviewRoot;

        while (stackTopLocal >= 0)
        {
            Review *currentReview = stack[stackTopLocal--];

            if (currentReview->product_id == product_id)
            {
                reviewExists = true;
                break;
            }

            if (currentReview->right != nullptr)
            {
                stack[++stackTopLocal] = currentReview->right;
            }
            if (currentReview->left != nullptr)
            {
                stack[++stackTopLocal] = currentReview->left;
            }
        }
    }

    if (!reviewExists)
    {
        cout << "Belum ada review untuk produk ini." << endl;
        cout << "Jadilah yang pertama memberikan review!" << endl;
        cout << "Gunakan menu 'Tambah Review' untuk memberikan penilaian." << endl;
        cout << repeatChar(60, '=') << endl;
        return;
    }

    cout << "Menampilkan semua review (diurutkan dari rating tertinggi):" << endl;
    cout << repeatChar(60, '-') << endl;

    displayReviews(reviewRoot, product_id); // PASTIKAN NAMA FUNGSI BENAR

    cout << repeatChar(60, '=') << endl;
    cout << "Tips:" << endl;
    cout << "   * Baca review dari berbagai pengguna untuk mendapat gambaran lengkap" << endl;
    cout << "   * Perhatikan rating dan komentar detail sebelum membeli" << endl;
    cout << "   * Jangan ragu bertanya ke seller jika ada yang kurang jelas" << endl;
}

// Fungsi khusus untuk menampilkan produk untuk guest
void displayProductsForGuest()
{
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "                DAFTAR PRODUK (MODE GUEST)" << endl;
    cout << "     Untuk melakukan pembelian, silakan login terlebih dahulu." << endl;
    cout << repeatChar(60, '=') << endl;

    Product *current = productHead;
    int count = 1;

    while (current != nullptr)
    {
        // Mencari informasi seller berdasarkan seller_id
        char nama_toko[100] = "Tidak diketahui";
        char lokasi[100] = "Tidak diketahui";

        for (int i = 0; i < HASH_SIZE; i++)
        {
            if (sellerHashTable[i].isActive && sellerHashTable[i].id == current->seller_id)
            {
                stringCopy(nama_toko, sellerHashTable[i].nama_toko);

                if (stringContains(sellerHashTable[i].nama_toko, "Jakarta"))
                {
                    stringCopy(lokasi, "Jakarta");
                }
                else if (stringContains(sellerHashTable[i].nama_toko, "Surabaya"))
                {
                    stringCopy(lokasi, "Surabaya");
                }
                else if (stringLength(sellerHashTable[i].lokasi_toko) > 0)
                {
                    stringCopy(lokasi, sellerHashTable[i].lokasi_toko);
                }
                break;
            }
        }

        cout << count++ << ". " << current->nama << endl;
        cout << "   Kategori: " << current->kategori << " - " << current->subkategori << endl;
        cout << "   Harga: Rp " << current->harga << endl;
        cout << "   Stok: " << current->stok << " unit tersedia" << endl;
        cout << "   Rating: " << current->rating << "/5 (" << current->jumlah_review << " reviews)" << endl;
        cout << "   ID Produk: " << current->id << endl;
        cout << "   Dijual oleh: " << nama_toko << endl;
        cout << "   Lokasi Toko: " << lokasi << endl;
        cout << "   [LOGIN REQUIRED untuk membeli produk ini]" << endl;
        cout << repeatChar(60, '-') << endl;
        current = current->next;
    }

    cout << "\nTip: Gunakan opsi 'Login untuk Berbelanja' untuk melakukan pemesanan!" << endl;
}

// Fungsi untuk menampilkan semua review
void displayAllReviews()
{
    cout << "\n"
         << repeatChar(60, '=') << endl;
    cout << "                SEMUA REVIEW PRODUK" << endl;
    cout << repeatChar(60, '=') << endl;

    // Traverse semua produk
    Product *current = productHead;

    while (current != nullptr)
    {
        cout << "\nPRODUK: " << current->nama << " (ID: " << current->id << ")" << endl;
        cout << "Rating Rata-rata: " << current->rating << "/5 (" << current->jumlah_review << " reviews)" << endl;
        cout << repeatChar(60, '-') << endl;

        // Tampilkan review untuk produk ini
        displayReviews(reviewRoot, current->id);

        current = current->next;
    }
}

// Modified Graph Functions untuk Simulasi Pengiriman dengan jalur detail

void initializeGraph()
{
    // Inisialisasi lokasi gudang dan kota yang diperluas
    stringCopy(graphNodes[0].nama_lokasi, "Gudang Jakarta");
    stringCopy(graphNodes[1].nama_lokasi, "Gudang Surabaya");
    stringCopy(graphNodes[2].nama_lokasi, "Gudang Medan");
    stringCopy(graphNodes[3].nama_lokasi, "Kota Bandung");
    stringCopy(graphNodes[4].nama_lokasi, "Kota Yogyakarta");
    stringCopy(graphNodes[5].nama_lokasi, "Kota Semarang");
    stringCopy(graphNodes[6].nama_lokasi, "Kota Denpasar");
    stringCopy(graphNodes[7].nama_lokasi, "Kota Padang");
    stringCopy(graphNodes[8].nama_lokasi, "Kota Lampung");
    stringCopy(graphNodes[9].nama_lokasi, "Kota Solo");
    stringCopy(graphNodes[10].nama_lokasi, "Kota Malang");

    for (int i = 0; i < 11; i++)
    {
        graphNodes[i].id = i;
        graphNodes[i].visited = false;
    }
    nodeCount = 11;

    // Tambahkan rute yang lebih kompleks dengan kota perantara
    // Format: {from, to, distance}

    // Dari Jakarta
    graphEdges[0] = {0, 3, 150}; // Jakarta - Bandung
    graphEdges[1] = {0, 8, 220}; // Jakarta - Lampung
    graphEdges[2] = {0, 4, 500}; // Jakarta - Yogyakarta
    graphEdges[3] = {0, 5, 450}; // Jakarta - Semarang

    // Dari Surabaya
    graphEdges[4] = {1, 10, 90}; // Surabaya - Malang
    graphEdges[5] = {1, 4, 300}; // Surabaya - Yogyakarta
    graphEdges[6] = {1, 5, 250}; // Surabaya - Semarang
    graphEdges[7] = {1, 9, 280}; // Surabaya - Solo

    // Dari Medan
    graphEdges[8] = {2, 7, 350};  // Medan - Padang
    graphEdges[9] = {2, 0, 1000}; // Medan - Jakarta

    // Koneksi antar kota
    graphEdges[10] = {3, 4, 350};  // Bandung - Yogyakarta
    graphEdges[11] = {4, 5, 120};  // Yogyakarta - Semarang
    graphEdges[12] = {4, 9, 65};   // Yogyakarta - Solo
    graphEdges[13] = {5, 9, 110};  // Semarang - Solo
    graphEdges[14] = {9, 1, 280};  // Solo - Surabaya
    graphEdges[15] = {4, 6, 650};  // Yogyakarta - Denpasar
    graphEdges[16] = {1, 6, 700};  // Surabaya - Denpasar
    graphEdges[17] = {8, 3, 180};  // Lampung - Bandung
    graphEdges[18] = {7, 0, 900};  // Padang - Jakarta
    graphEdges[19] = {10, 6, 550}; // Malang - Denpasar

    // Rute balik (bidirectional)
    graphEdges[20] = {3, 0, 150};  // Bandung - Jakarta
    graphEdges[21] = {8, 0, 220};  // Lampung - Jakarta
    graphEdges[22] = {4, 0, 500};  // Yogyakarta - Jakarta
    graphEdges[23] = {5, 0, 450};  // Semarang - Jakarta
    graphEdges[24] = {10, 1, 90};  // Malang - Surabaya
    graphEdges[25] = {4, 1, 300};  // Yogyakarta - Surabaya
    graphEdges[26] = {5, 1, 250};  // Semarang - Surabaya
    graphEdges[27] = {9, 1, 280};  // Solo - Surabaya
    graphEdges[28] = {7, 2, 350};  // Padang - Medan
    graphEdges[29] = {0, 2, 1000}; // Jakarta - Medan
    graphEdges[30] = {4, 3, 350};  // Yogyakarta - Bandung
    graphEdges[31] = {5, 4, 120};  // Semarang - Yogyakarta
    graphEdges[32] = {9, 4, 65};   // Solo - Yogyakarta
    graphEdges[33] = {9, 5, 110};  // Solo - Semarang
    graphEdges[34] = {1, 9, 280};  // Surabaya - Solo
    graphEdges[35] = {6, 4, 650};  // Denpasar - Yogyakarta
    graphEdges[36] = {6, 1, 700};  // Denpasar - Surabaya
    graphEdges[37] = {3, 8, 180};  // Bandung - Lampung
    graphEdges[38] = {0, 7, 900};  // Jakarta - Padang
    graphEdges[39] = {6, 10, 550}; // Denpasar - Malang

    edgeCount = 40;
}

void resetGraphVisited()
{
    for (int i = 0; i < nodeCount; i++)
    {
        graphNodes[i].visited = false;
    }
}

// // Fungsi untuk menyimpan jalur terbaik
// struct RouteInfo
// {
//     int path[MAX_GRAPH_NODES];
//     int pathLength;
//     int totalDistance;
// };

// RouteInfo bestRoute;
// bool routeFound = false;

void findBestRoute(int start, int destination, int currentDistance, int path[], int pathLength)
{
    if (start == destination)
    {
        if (!routeFound || currentDistance < bestRoute.totalDistance)
        {
            // Simpan rute terbaik
            for (int i = 0; i < pathLength; i++)
            {
                bestRoute.path[i] = path[i];
            }
            bestRoute.pathLength = pathLength;
            bestRoute.totalDistance = currentDistance;
            routeFound = true;
        }
        return;
    }

    graphNodes[start].visited = true;

    for (int i = 0; i < edgeCount; i++)
    {
        if (graphEdges[i].from == start && !graphNodes[graphEdges[i].to].visited)
        {
            path[pathLength] = graphEdges[i].to;
            findBestRoute(graphEdges[i].to, destination, currentDistance + graphEdges[i].distance,
                          path, pathLength + 1);
        }
    }

    graphNodes[start].visited = false; // Backtrack
}

void displayDetailedRoute(int path[], int pathLength, int totalDistance)
{
    cout << "\n=== DETAIL RUTE PENGIRIMAN ===" << endl;
    cout << "Jalur yang akan dilalui:" << endl;

    for (int i = 0; i < pathLength; i++)
    {
        cout << (i + 1) << ". " << graphNodes[path[i]].nama_lokasi;

        if (i < pathLength - 1)
        {
            // Cari jarak ke kota berikutnya
            int distance = 0;
            for (int j = 0; j < edgeCount; j++)
            {
                if (graphEdges[j].from == path[i] && graphEdges[j].to == path[i + 1])
                {
                    distance = graphEdges[j].distance;
                    break;
                }
            }
            cout << " -> " << graphNodes[path[i + 1]].nama_lokasi;
            cout << " (" << distance << " km)" << endl;
        }
        else
        {
            cout << " [TUJUAN AKHIR]" << endl;
        }
    }

    cout << "\nRingkasan Pengiriman:" << endl;
    cout << "- Titik Awal: " << graphNodes[path[0]].nama_lokasi << endl;
    cout << "- Titik Tujuan: " << graphNodes[path[pathLength - 1]].nama_lokasi << endl;
    cout << "- Jumlah Kota yang Dilalui: " << pathLength << " kota" << endl;
    cout << "- Total Jarak: " << totalDistance << " km" << endl;

    // Estimasi waktu pengiriman berdasarkan jarak
    int estimasiHari;
    if (totalDistance <= 200)
    {
        estimasiHari = 1;
    }
    else if (totalDistance <= 500)
    {
        estimasiHari = 2;
    }
    else if (totalDistance <= 800)
    {
        estimasiHari = 3;
    }
    else
    {
        estimasiHari = 4;
    }

    cout << "- Estimasi Waktu Pengiriman: " << estimasiHari << " hari kerja" << endl;

    // // Estimasi biaya pengiriman
    // int biayaPengiriman = (totalDistance / 100) * 5000 + 10000; // Base 10rb + 5rb per 100km
    // cout << "- Estimasi Biaya Pengiriman: Rp " << biayaPengiriman << endl;
}

void simulateShipping()
{
    cout << "\n=== SIMULASI PENGIRIMAN TERPADU ===" << endl;
    cout << "Lokasi Gudang dan Kota yang Tersedia:" << endl;

    cout << "\nGUDANG:" << endl;
    for (int i = 0; i < 3; i++)
    {
        cout << i << ". " << graphNodes[i].nama_lokasi << endl;
    }

    cout << "\nKOTA TUJUAN:" << endl;
    for (int i = 3; i < nodeCount; i++)
    {
        cout << i << ". " << graphNodes[i].nama_lokasi << endl;
    }

    int gudang, tujuan;
    cout << "\nPilih gudang asal (0-2): ";
    cin >> gudang;
    cout << "Pilih kota tujuan (3-" << (nodeCount - 1) << "): ";
    cin >> tujuan;

    if (gudang < 0 || gudang > 2 || tujuan < 3 || tujuan >= nodeCount)
    {
        cout << "Pilihan tidak valid!" << endl;
        cout << "Gudang harus 0-2, Tujuan harus 3-" << (nodeCount - 1) << endl;
        return;
    }

    cout << "\nMencari rute optimal dari " << graphNodes[gudang].nama_lokasi
         << " ke " << graphNodes[tujuan].nama_lokasi << "..." << endl;

    // Reset variabel pencarian rute
    routeFound = false;
    bestRoute.totalDistance = 999999;

    int path[MAX_GRAPH_NODES];
    path[0] = gudang;

    resetGraphVisited();
    findBestRoute(gudang, tujuan, 0, path, 1);

    if (!routeFound)
    {
        cout << "\nMaaf, tidak ada rute yang tersedia dari " << graphNodes[gudang].nama_lokasi
             << " ke " << graphNodes[tujuan].nama_lokasi << endl;
        cout << "Silakan pilih kombinasi gudang dan tujuan yang lain." << endl;
    }
    else
    {
        displayDetailedRoute(bestRoute.path, bestRoute.pathLength, bestRoute.totalDistance);

        // Tambahan informasi logistik
        cout << "\n=== INFORMASI TAMBAHAN ===" << endl;
        cout << "Status: Siap untuk pengiriman" << endl;
        cout << "Metode Pengiriman: Jalur Darat" << endl;
    }
}

// Menu Functions
void userMenu(User *user)
{
    int choice;
    do
    {
        cout << "\n"
             << repeatChar(70, '=') << endl;
        cout << "                       DASHBOARD USER" << endl;
        cout << "                  Selamat datang, " << user->username << "!" << endl;
        cout << "                     Status: User Aktif" << endl;
        cout << repeatChar(70, '=') << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                     MENU BELANJA                                    |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  1. Lihat Semua Produk                                              |" << endl;
        cout << "|  2. Browse by Kategori                                              |" << endl;
        cout << "|  3. Cari Produk                                                     |" << endl;
        cout << "|  4. Urutkan Produk by Harga                                         |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                   KERANJANG & CHECKOUT                              |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  5. Tambah ke Keranjang                                             |" << endl;
        cout << "|  6. Proses Checkout & Pembayaran                                    |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                  RIWAYAT & INTERAKSI                                |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  7. Lihat Riwayat Transaksi                                         |" << endl;
        cout << "|  8. Konfirmasi Penerimaan                                           |" << endl;
        cout << "|  9. Tambah Review Produk                                            |" << endl;
        cout << "| 10. Lihat Review Produk                                             |" << endl;
        cout << "| 11. Simulasi Pengiriman                                             |" << endl;
        cout << "| 12. Cancel/Refund Order                                             |" << endl;
        cout << "|                                                                     |" << endl;
        cout << "|  0. Logout                                                          |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "Tips: Gunakan 'Tambah ke Keranjang' sebelum checkout!" << endl;
        cout << "Tips: Konfirmasi penerimaan paket setelah barang sampai!" << endl;
        cout << repeatChar(70, '-') << endl;
        cout << "Pilihan Anda (0-11): ";
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
            cout << "\n"
                 << repeatChar(50, '=') << endl;
            cout << "           PENGURUTAN HARGA PRODUK" << endl;
            cout << repeatChar(50, '=') << endl;
            cout << "1. Ascending (Murah ke Mahal)" << endl;
            cout << "2. Descending (Mahal ke Murah)" << endl;
            cout << repeatChar(50, '-') << endl;
            cout << "Pilih urutan (1-2): ";
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
            confirmPackageReceived(user->id);
            break;
        case 9:
            addReview(user->id, user->username);
            break;
        case 10:
            viewProductReviews();
            break;
        case 11:
            simulateShipping();
            break;
        case 12:
            cancelTransaction(user->id);
            break;
        case 0:
            cout << "\n"
                 << repeatChar(50, '=') << endl;
            cout << "                LOGOUT BERHASIL" << endl;
            cout << "              Terima kasih, " << user->username << "!" << endl;
            cout << "                 Sampai jumpa lagi!" << endl;
            cout << repeatChar(50, '=') << endl;
            break;
        default:
            cout << "\nPilihan tidak valid! Silakan pilih angka 0-11." << endl;
        }
    } while (choice != 0);
}

void sellerMenu(Seller *seller)
{
    int choice;
    do
    {
        cout << "\n"
             << repeatChar(70, '=') << endl;
        cout << "                        DASHBOARD SELLER" << endl;
        cout << "                Selamat datang, " << seller->nama_toko << "!" << endl;
        cout << "                     Status: Seller Aktif" << endl;
        cout << repeatChar(70, '=') << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                   MANAJEMEN PRODUK                                  |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  1. Tambah Produk Baru                                              |" << endl;
        cout << "|  2. Lihat Semua Produk di Marketplace                               |" << endl;
        cout << "|  3. Lihat Produk Toko Saya                                          |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                   MANAJEMEN PESANAN                                 |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  4. Lihat Order Masuk                                               |" << endl;
        cout << "|  5. Update Status Pengiriman                                        |" << endl;
        cout << "|  6. Laporan Penjualan                                               |" << endl;
        cout << "|                                                                     |" << endl;
        cout << "|  0. Logout                                                          |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "Panel kontrol lengkap untuk mengelola toko Anda" << endl;
        cout << repeatChar(70, '-') << endl;
        cout << "Pilihan Anda (0-6): ";
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
            cout << "\n"
                 << repeatChar(60, '=') << endl;
            cout << "                PRODUK TOKO SAYA" << endl;
            cout << "                " << seller->nama_toko << endl;
            cout << repeatChar(60, '=') << endl;
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
                    cout << "   Stok: " << current->stok << " unit" << endl;
                    cout << "   Rating: " << current->rating << " (" << current->jumlah_review << " reviews)" << endl;
                    cout << "   ID: " << current->id << endl;
                    cout << repeatChar(60, '-') << endl;
                    found = true;
                }
                current = current->next;
            }

            if (!found)
            {
                cout << "Anda belum memiliki produk." << endl;
                cout << "Gunakan menu 'Tambah Produk Baru' untuk mulai berjualan!" << endl;
            }
            cout << repeatChar(60, '=') << endl;
            break;
        }
        case 4:
            showSellerOrders(seller->id);
            break;
        case 5:
            updateShippingStatus(seller->id);
            break;
        case 6:
            showSellerReport(seller->id);
            break;
        case 0:
            cout << "\n"
                 << repeatChar(50, '=') << endl;
            cout << "                   LOGOUT BERHASIL" << endl;
            cout << "            Terima kasih, " << seller->nama_toko << "!" << endl;
            cout << "                 Sampai jumpa lagi!" << endl;
            cout << repeatChar(50, '=') << endl;
            break;
        default:
            cout << "\nPilihan tidak valid! Silakan pilih angka 0-6." << endl;
        }
    } while (choice != 0);
}

// Tambahkan fungsi guestMenu yang dipanggil di case 1
void guestMenu()
{
    int choice;
    do
    {
        cout << "\n"<< endl;
        cout << "\n"
             << repeatChar(70, '=') << endl;
        cout << "                      MODE GUEST" << endl;
        cout << "              Jelajahi produk tanpa perlu login!" << endl;
        cout << repeatChar(70, '=') << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                    MENU JELAJAH PRODUK                             |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  1. Lihat Semua Produk                                             |" << endl;
        cout << "|  2. Browse by Kategori                                             |" << endl;
        cout << "|  3. Cari Produk Spesifik                                           |" << endl;
        cout << "|  4. Urutkan Produk by Harga                                        |" << endl;
        cout << "|  5. Lihat Review Produk Tertentu                                   |" << endl;
        cout << "|  6. Lihat Semua Review                                             |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                   AKSI SELANJUTNYA                                 |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  7. Login untuk Berbelanja                                         |" << endl;
        cout << "|  8. Daftar Akun Baru                                               |" << endl;
        cout << "|                                                                    |" << endl;
        cout << "|  0. Kembali ke Menu Utama                                          |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "Untuk berbelanja, Anda perlu login terlebih dahulu!" << endl;
        cout << repeatChar(70, '-') << endl;
        cout << "Pilihan Anda (0-8): ";
        cin >> choice;
        cout << "\n" << endl;

        switch (choice)
        {
        case 1:
            displayProductsForGuest();
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
            cout << "\n"
                 << repeatChar(50, '=') << endl;
            cout << "           PENGURUTAN HARGA PRODUK" << endl;
            cout << repeatChar(50, '=') << endl;
            cout << "1. Ascending (Murah ke Mahal)" << endl;
            cout << "2. Descending (Mahal ke Murah)" << endl;
            cout << repeatChar(50, '-') << endl;
            cout << "Pilih urutan (1-2): ";
            cin >> sortChoice;
            sortProductsByPrice(sortChoice == 1);
            displayProductsForGuest();
            break;
        }
        case 5:
            viewProductReviews();
            break;
        case 6:
            displayAllReviews();
            break;
            // Update case 7 dan 8 dalam guestMenu()
        case 7:
        {
            cout << "\n"
                 << repeatChar(70, '=') << endl;
            cout << "              REDIRECT KE LOGIN USER" << endl;
            cout << repeatChar(70, '=') << endl;
            cout << "Anda akan dialihkan ke halaman login untuk dapat berbelanja." << endl;
            cout << "Setelah login berhasil, Anda dapat:" << endl;
            cout << "* Menambahkan produk ke keranjang" << endl;
            cout << "* Melakukan checkout dan pembayaran" << endl;
            cout << "* Melihat riwayat transaksi" << endl;
            cout << "* Memberikan review produk" << endl;
            cout << "* Melacak status pengiriman" << endl;
            cout << repeatChar(70, '-') << endl;

            cout << "\nMemproses redirect..." << endl;
            cout << "Tunggu sebentar";
            for (int i = 0; i < 3; i++)
            {
                cout << ".";
                // Simulasi loading time (opsional - bisa dihapus jika tidak diinginkan)
                for (int j = 0; j < 100000000; j++)
                {
                } // Simple delay
            }
            cout << endl;

            User *user = loginUser();
            if (user != nullptr)
            {
                cout << "\n"
                     << repeatChar(70, '=') << endl;
                cout << "               LOGIN BERHASIL!" << endl;
                cout << repeatChar(70, '=') << endl;
                cout << "Selamat datang kembali, " << user->username << "!" << endl;
                cout << "Anda sekarang dapat mengakses semua fitur marketplace." << endl;
                cout << "Mengalihkan ke dashboard user..." << endl;
                cout << repeatChar(70, '=') << endl;

                // Loading animation sebelum masuk ke user menu
                cout << "\nMemuat dashboard";
                for (int i = 0; i < 3; i++)
                {
                    cout << ".";
                    for (int j = 0; j < 100000000; j++)
                    {
                    } // Simple delay
                }
                cout << endl;

                userMenu(user);
            }
            else
            {
                cout << "\n"
                     << repeatChar(70, '=') << endl;
                cout << "               LOGIN GAGAL!" << endl;
                cout << repeatChar(70, '=') << endl;
                cout << "Username atau password yang Anda masukkan salah." << endl;
                cout << "Kembali ke mode guest untuk mencoba lagi." << endl;
                cout << repeatChar(70, '=') << endl;

                cout << "\nKembali ke mode guest dalam";
                for (int i = 3; i > 0; i--)
                {
                    cout << " " << i;
                    for (int j = 0; j < 200000000; j++)
                    {
                    } // Simple delay
                }
                cout << "..." << endl;
            }
            break;
        }
        case 8:
        {
            cout << "\n"
                 << repeatChar(70, '=') << endl;
            cout << "            REDIRECT KE REGISTRASI USER" << endl;
            cout << repeatChar(70, '=') << endl;
            cout << "Belum punya akun? Tidak masalah!" << endl;
            cout << "Daftar sekarang dan dapatkan akses ke:" << endl;
            cout << "* Ribuan produk gadget terbaru" << endl;
            cout << "* Sistem pembayaran yang aman" << endl;
            cout << "* Tracking pengiriman real-time" << endl;
            cout << "* Customer support 24/7" << endl;
            cout << "* Promo dan diskon eksklusif" << endl;
            cout << repeatChar(70, '-') << endl;

            cout << "\nMemproses redirect..." << endl;
            cout << "Tunggu sebentar";
            for (int i = 0; i < 3; i++)
            {
                cout << ".";
                for (int j = 0; j < 100000000; j++)
                {
                } // Simple delay
            }
            cout << endl;

            bool registrationSuccess = registerUser();

            if (registrationSuccess)
            {
                cout << "\n"
                     << repeatChar(70, '=') << endl;
                cout << "               REGISTRASI BERHASIL!" << endl;
                cout << repeatChar(70, '=') << endl;
                cout << "Selamat! Akun Anda telah berhasil dibuat." << endl;
                cout << "Sekarang Anda dapat login untuk mulai berbelanja." << endl;
                cout << repeatChar(70, '-') << endl;

                char autoLogin;
                cout << "Apakah Anda ingin langsung login? (y/n): ";
                cin >> autoLogin;

                if (autoLogin == 'y' || autoLogin == 'Y')
                {
                    cout << "\nMengalihkan ke halaman login..." << endl;
                    cout << "Tunggu sebentar";
                    for (int i = 0; i < 3; i++)
                    {
                        cout << ".";
                        for (int j = 0; j < 100000000; j++)
                        {
                        } // Simple delay
                    }
                    cout << endl;

                    User *user = loginUser();
                    if (user != nullptr)
                    {
                        cout << "\n"
                             << repeatChar(70, '=') << endl;
                        cout << "           SELAMAT DATANG DI MARKETPLACE!" << endl;
                        cout << repeatChar(70, '=') << endl;
                        cout << "Halo " << user->username << "! Terima kasih sudah bergabung." << endl;
                        cout << "Mari mulai menjelajahi dan berbelanja produk favorit Anda!" << endl;
                        cout << repeatChar(70, '=') << endl;

                        cout << "\nMemuat dashboard user";
                        for (int i = 0; i < 3; i++)
                        {
                            cout << ".";
                            for (int j = 0; j < 100000000; j++)
                            {
                            } // Simple delay
                        }
                        cout << endl;

                        userMenu(user);
                    }
                }
                else
                {
                    cout << "\nAnda dapat login kapan saja melalui menu utama." << endl;
                    cout << "Kembali ke mode guest..." << endl;
                }
            }
            else
            {
                cout << "\n"
                     << repeatChar(70, '=') << endl;
                cout << "             REGISTRASI GAGAL!" << endl;
                cout << repeatChar(70, '=') << endl;
                cout << "Terjadi kesalahan saat mendaftar." << endl;
                cout << "Mungkin username sudah digunakan atau ada masalah lain." << endl;
                cout << "Silakan coba lagi dengan data yang berbeda." << endl;
                cout << repeatChar(70, '=') << endl;

                cout << "\nKembali ke mode guest dalam";
                for (int i = 3; i > 0; i--)
                {
                    cout << " " << i;
                    for (int j = 0; j < 200000000; j++)
                    {
                    } // Simple delay
                }
                cout << "..." << endl;
            }
            break;
        }
        case 0:
            cout << "\nKembali ke menu utama..." << endl;
            break;
        default:
            cout << "\nPilihan tidak valid! Silakan pilih angka 0-8." << endl;
        }
    } while (choice != 0);
}

void mainMenu()
{
    int choice;
    do
    {
        cout << "\n"
             << repeatChar(70, '=') << endl;
        cout << "                        MARKETPLACE GADGET" << endl;
        cout << "                    Toko Elektronik Terlengkap" << endl;
        cout << "                     Sejak 2024 - Terpercaya" << endl;
        cout << repeatChar(70, '=') << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|                        MENU UTAMA                                  |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "|  1. Mode Guest (Jelajahi Tanpa Login)                              |" << endl;
        cout << "|  2. Login User (Pelanggan)                                         |" << endl;
        cout << "|  3. Login Seller (Penjual)                                         |" << endl;
        cout << "|                                                                    |" << endl;
        cout << "|  4. Daftar Sebagai User Baru                                       |" << endl;
        cout << "|  5. Daftar Sebagai Seller Baru                                     |" << endl;
        cout << "|                                                                    |" << endl;
        cout << "|  0. Keluar dari Aplikasi                                           |" << endl;
        cout << "+" << repeatChar(68, '-') << "+" << endl;
        cout << "Tips: Mulai dengan Mode Guest untuk melihat produk!" << endl;
        cout << repeatChar(70, '-') << endl;
        cout << "Masukkan pilihan Anda (0-5): ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            guestMenu();
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
        {
            Seller *seller = loginSeller();
            if (seller != nullptr)
            {
                sellerMenu(seller);
            }
            break;
        }
        case 4:
            registerUser();
            break;
        case 5:
            registerSeller();
            break;
        case 0:
            cout << "\n"
                 << repeatChar(70, '=') << endl;
            cout << "                    TERIMA KASIH!" << endl;
            cout << "            Telah menggunakan Marketplace Gadget" << endl;
            cout << "                   Sampai jumpa lagi!" << endl;
            cout << repeatChar(70, '=') << endl;
            break;
        default:
            cout << "\nPilihan tidak valid! Silakan pilih angka 0-5." << endl;
            cout << "Pastikan Anda memasukkan angka yang benar." << endl;
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
    stringCopy(sellerHashTable[hashIndex].lokasi_toko, "Jakarta"); // Tambahkan ini
    sellerHashTable[hashIndex].isActive = true;

    hashIndex = hashFunction("seller2");
    sellerHashTable[hashIndex].id = sellerIdCounter++;
    stringCopy(sellerHashTable[hashIndex].username, "seller2");
    stringCopy(sellerHashTable[hashIndex].password, "pass123");
    stringCopy(sellerHashTable[hashIndex].nama_toko, "Electronic Store Surabaya");
    stringCopy(sellerHashTable[hashIndex].lokasi_toko, "Surabaya"); // Tambahkan ini
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

    // Sample User 1
    hashIndex = hashFunction("user1");
    int originalIndex = hashIndex;
    while (userHashTable[hashIndex].isActive)
    {
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
            break;
    }
    userHashTable[hashIndex].id = userIdCounter++;
    stringCopy(userHashTable[hashIndex].username, "user1");
    stringCopy(userHashTable[hashIndex].password, "pass123");
    stringCopy(userHashTable[hashIndex].alamat, "Jl. Merdeka No. 123, Jakarta");
    userHashTable[hashIndex].isActive = true;

    // Sample User 2 - Devadatta
    hashIndex = hashFunction("Devadatta");
    originalIndex = hashIndex;
    while (userHashTable[hashIndex].isActive)
    {
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
            break;
    }
    userHashTable[hashIndex].id = userIdCounter++;
    stringCopy(userHashTable[hashIndex].username, "Devadatta");
    stringCopy(userHashTable[hashIndex].password, "dev123");
    stringCopy(userHashTable[hashIndex].alamat, "JL. Sidakarya No. 90, Denpasar");
    userHashTable[hashIndex].isActive = true;

    // Sample User 3
    hashIndex = hashFunction("user3");
    originalIndex = hashIndex;
    while (userHashTable[hashIndex].isActive)
    {
        hashIndex = (hashIndex + 1) % HASH_SIZE;
        if (hashIndex == originalIndex)
            break;
    }
    userHashTable[hashIndex].id = userIdCounter++;
    stringCopy(userHashTable[hashIndex].username, "user3");
    stringCopy(userHashTable[hashIndex].password, "pass123");
    stringCopy(userHashTable[hashIndex].alamat, "Jl. Gajah Mada No. 45, Bandung");
    userHashTable[hashIndex].isActive = true;

    // Sample Reviews (sesuai yang sebelumnya)
    reviewRoot = insertReview(reviewRoot, 1, 1, "Hermanto", 5, "iPhone terbaik yang pernah saya beli! Kamera luar biasa!");
    reviewRoot = insertReview(reviewRoot, 1, 2, "Devadatta", 4, "Performa bagus tapi harganya mahal banget");
    reviewRoot = insertReview(reviewRoot, 1, 3, "Marselino", 5, "Build quality premium, worth the price!");
    reviewRoot = insertReview(reviewRoot, 1, 1, "Malik", 4, "Battery life sangat memuaskan");

    reviewRoot = insertReview(reviewRoot, 2, 1, "David Gadgetin", 5, "Samsung Galaxy terbaik! S-Pen sangat berguna");
    reviewRoot = insertReview(reviewRoot, 2, 2, "Mas Owen", 5, "Kamera zoom 100x benar-benar incredible!");
    reviewRoot = insertReview(reviewRoot, 2, 3, "Om Deddy", 4, "Display sangat jernih, gaming lancar jaya");
    reviewRoot = insertReview(reviewRoot, 2, 1, "Ole Romeny", 4, "OneUI interface friendly dan smooth");

    reviewRoot = insertReview(reviewRoot, 3, 2, "Pak Bambang", 5, "Perfect untuk digital drawing dan design!");
    reviewRoot = insertReview(reviewRoot, 3, 1, "Bu Rina", 4, "M1 chip sangat powerful untuk tablet");
    reviewRoot = insertReview(reviewRoot, 3, 3, "Duta SO7", 5, "Screen quality amazing, cocok untuk streaming");
    reviewRoot = insertReview(reviewRoot, 3, 2, "Ari Lasso", 4, "Apple Pencil support sangat responsive");

    reviewRoot = insertReview(reviewRoot, 4, 1, "Ahmad Dhani", 5, "Laptop terbaik untuk programming dan design!");
    reviewRoot = insertReview(reviewRoot, 4, 2, "Pak Kahar", 5, "M2 chip performance luar biasa, silent operation");
    reviewRoot = insertReview(reviewRoot, 4, 3, "Hovit MCI", 5, "Battery life all day, build quality premium");
    reviewRoot = insertReview(reviewRoot, 4, 1, "Mas Arhan", 4, "macOS ecosystem integration seamless");
    reviewRoot = insertReview(reviewRoot, 4, 2, "Purwanto", 5, "Perfect weight untuk mobile computing");

    reviewRoot = insertReview(reviewRoot, 5, 3, "Herlambang", 4, "Health tracking features sangat akurat");
    reviewRoot = insertReview(reviewRoot, 5, 1, "Luna Maya", 5, "Fitness companion yang sempurna!");
    reviewRoot = insertReview(reviewRoot, 5, 2, "Anya Geraldine", 4, "Always-on display sangat membantu");
    reviewRoot = insertReview(reviewRoot, 5, 3, "Ariel Tatum", 5, "Water resistant untuk swimming mantap");
    reviewRoot = insertReview(reviewRoot, 5, 1, "Dr. Tirta", 4, "Integration dengan iPhone sangat smooth");

    cout << "Sample data berhasil dimuat!" << endl;
    cout << "Sample login:" << endl;
    cout << "User - username: user1, password: pass123" << endl;
    cout << "User - username: Devadatta, password: dev123" << endl;
    cout << "User - username: user3, password: pass123" << endl;
    cout << "Seller - username: seller1, password: pass123" << endl;
    cout << "Seller - username: seller2, password: pass123" << endl;
}

// Tambahkan fungsi debug ini setelah initializeSampleData()
void debugHashTable()
{
    cout << "\n=== DEBUG HASH TABLE ===" << endl;
    cout << "Active users in hash table:" << endl;

    for (int i = 0; i < HASH_SIZE; i++)
    {
        if (userHashTable[i].isActive)
        {
            cout << "Index " << i << ": " << userHashTable[i].username
                << " (ID: " << userHashTable[i].id << ")" << endl;
        }
    }

    // Test hash function untuk username tertentu
    cout << "\nHash values:" << endl;
    cout << "user1: " << hashFunction("user1") << endl;
    cout << "Devadatta: " << hashFunction("Devadatta") << endl;
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
    debugHashTable(); // Debugging function to check hash table

    cout << "\nSistem siap digunakan!" << endl;

    // Jalankan menu utama
    mainMenu();

    // Cleanup memory
    cleanupProducts();
    cleanupReviews(reviewRoot);

    cout << "Program selesai. Terima kasih!" << endl;

    return 0;
}