#include "hashmap.h"

static int hashCode(HashMap hashMap, char *key);

static inline bool equal(char *key1, char *key2);

static void set(HashMap hashMap, char *key, void *value);

static void *get(HashMap hashMap, char *key);

static bool has(HashMap hashMap, char *key);

static bool delete(HashMap hashMap, char *key);

static void clear(HashMap hashMap);

static void resizeHashMap(HashMap hashMap, unsigned capacity);

static HashMapIterator next(HashMapIterator iterator);

static bool hasNext(HashMapIterator iterator);

static int hashCode(HashMap hashMap, char *key) {
    char *k = key;
    unsigned long h = 0;
    while (*k) {
        h = (h << 4) + *k++;
        unsigned long g = h & 0xF0000000L;
        if (g) h ^= g >> 24;
        h &= ~g;
    }
    return (int) (h % hashMap->capacity);
}

static inline bool equal(char *key1, char *key2) {
    return strcmp(key1, key2) == 0;
}

static void set(HashMap hashMap, char *key, void *value) {
    if (hashMap->size >= hashMap->capacity) {
        resizeHashMap(hashMap, hashMap->capacity * 2);
    }
    HashMapNode node = &hashMap->nodes[hashMap->hashCode(hashMap, key)];
    if (!node->key) {
        node->key = key;
        node->value = value;
        hashMap->size++;
    } else {
        // 存在key, 则把数组接到冲突链表
        HashMapNode nextNode = node;
        while (nextNode) {
            // key已存在冲突链表, 更新value即可
            if (equal(nextNode->key, key)) {
                nextNode->value = value;
                return;
            }
            nextNode = nextNode->next;
        }
        // key不存在冲突链表, 新建节点, 插入到node的下一个节点而不是最后一个节点
        HashMapNode newNode = (HashMapNode) malloc(sizeof(struct hashMapNode));
        if (!newNode) return;
        newNode->key = key;
        newNode->value = value;
        newNode->next = node->next;
        node->next = newNode;
        hashMap->size++;
    }
}

static void *get(HashMap hashMap, char *key) {
    HashMapNode node = &hashMap->nodes[hashMap->hashCode(hashMap, key)];
    if (!node->key) return NULL;
    if (equal(node->key, key)) return node->value;
    // 在冲突链表找
    if (node->next) {
        node = node->next;
        while (node) {
            if (equal(node->key, key)) return node->value;
            node = node->next;
        }
    }
    return NULL;
}

static bool has(HashMap hashMap, char *key) {
    return hashMap->get(hashMap, key) != NULL;
}

static bool delete(HashMap hashMap, char *key) {
    HashMapNode node = &hashMap->nodes[hashMap->hashCode(hashMap, key)];
    if (!node) return false;
    bool res = false;
    if (equal(node->key, key)) {
        // 把node->next的值拷贝到node然后释放node->next
        if (node->next) {
            HashMapNode t = node->next;
            node->key = t->key;
            node->value = t->value;
            node->next = t->next;
            free(t);
        } else {
            node->key = NULL;
            node->value = NULL;
        }
        hashMap->size--;
        res = true;
    } else {
        // 在冲突链表上找
        HashMapNode preNode = node;
        node = node->next;
        while (node) {
            if (equal(node->key, key)) {
                preNode->next = node->next;
                free(node);
                hashMap->size--;
                res = true;
                break;
            }
            preNode = node;
            node = node->next;
        }
    }
    if (hashMap->size < hashMap->capacity / 2) {
        resizeHashMap(hashMap, hashMap->capacity / 2);
    }
    return res;
}

static void clear(HashMap hashMap) {
    for (int i = 0; i < hashMap->capacity; i++) {
        // hashMap->nodes[i]是跟着hashMap->nodes分配的不能直接free
        HashMapNode node = hashMap->nodes[i].next;
        while (node) {
            HashMapNode nextNode = node->next;
            memset(node, 0, sizeof(struct hashMapNode));
            free(node);
            node = nextNode;
        }
        memset(&hashMap->nodes[i], 0, sizeof(struct hashMapNode));
    }
    hashMap->size = 0;
    hashMap->capacity = 8;
    hashMap->nodes = realloc(hashMap->nodes, hashMap->capacity * sizeof(struct hashMapNode));
}

static void resizeHashMap(HashMap hashMap, unsigned capacity) {
    if (capacity < 8) return;
    HashMapNode tempNodes = (HashMapNode) malloc(hashMap->size * sizeof(struct hashMapNode));
    if (!tempNodes) return;
    HashMapIterator iterator = createHashMapIterator(hashMap);
    // 暂存数据
    for (int i = 0; iterator->hasNext(iterator); i++) {
        iterator = next(iterator);
        tempNodes[i].key = iterator->node->key;
        tempNodes[i].value = iterator->node->value;
        tempNodes[i].next = NULL;
    }
    deleteHashMapIterator(&iterator);
    // 清空hashMap
    for (int i = 0; i < hashMap->capacity; i++) {
        HashMapNode node = &hashMap->nodes[i];
        node->key = NULL;
        node->value = NULL;
        while (node->next) {
            HashMapNode nextNode = node->next->next;
            free(node->next);
            node->next = nextNode;
        }
    }
    unsigned size = hashMap->size;
    hashMap->size = 0;
    hashMap->capacity = capacity;
    // 重新分配内存
    HashMapNode resizeNodes = (HashMapNode) realloc(hashMap->nodes, hashMap->capacity * sizeof(struct hashMapNode));
    if (!resizeNodes) return;
    memset(resizeNodes, 0, hashMap->capacity * sizeof(struct hashMapNode));
    hashMap->nodes = resizeNodes;
    // 从暂存数据nodes中写入hashMap
    for (int i = 0; i < size; i++) {
        hashMap->set(hashMap, tempNodes[i].key, tempNodes[i].value);
    }
    free(tempNodes);
}

static HashMapIterator next(HashMapIterator iterator) {
    if (iterator->hasNext(iterator)) {
        // 迭代冲突链表上的node
        if (iterator->node && iterator->node->next) {
            iterator->count++;
            iterator->node = iterator->node->next;
            return iterator;
        }
        while (++iterator->hashCode < iterator->hashMap->capacity) {
            HashMapNode node = &iterator->hashMap->nodes[iterator->hashCode];
            if (node->key) {
                iterator->count++;
                iterator->node = node;
                break;
            }
        }
    }
    return iterator;
}

static bool hasNext(HashMapIterator iterator) {
    return iterator->count < iterator->hashMap->size;
}

HashMap createHashMap() {
    HashMap hashMap = (HashMap) malloc(sizeof(struct hashMap));
    if (!hashMap) return NULL;
    hashMap->size = 0;
    hashMap->capacity = 8;
    hashMap->hashCode = hashCode;
    hashMap->set = set;
    hashMap->get = get;
    hashMap->has = has;
    hashMap->delete = delete;
    hashMap->clear = clear;
    hashMap->nodes = (HashMapNode) malloc(hashMap->capacity * sizeof(struct hashMapNode));
    if (!hashMap->nodes) return NULL;
    memset(hashMap->nodes, 0, hashMap->capacity * sizeof(struct hashMapNode));
    return hashMap;
}

void deleteHashMap(HashMap *hashMap) {
    (*hashMap)->clear(*hashMap);
    free(*hashMap);
    *hashMap = NULL;
}

HashMapIterator createHashMapIterator(HashMap hashMap) {
    HashMapIterator iterator = (HashMapIterator) malloc(sizeof(struct hashMapIterator));
    if (!iterator) return NULL;
    memset(iterator, 0, sizeof(struct hashMapIterator));
    iterator->hashMap = hashMap;
    iterator->hashCode = -1;
    iterator->next = next;
    iterator->hasNext = hasNext;
    return iterator;
}

void deleteHashMapIterator(HashMapIterator *iterator) {
    free(*iterator);
    *iterator = NULL;
}
