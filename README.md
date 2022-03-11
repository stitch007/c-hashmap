# hashmap

基于C11实现的hashmap, 使用链表解决冲突。

## API

### 初始化和销毁

```c
HashMap map = createHashMap();

deleteHashMap(&map);
```

### 插入、取值和删除

```c
// 插入, key为string, value支持任意类型
map->set(map, "hello", "world");
int number = 65535;
map->set(map, "number", &number);

// 取值
char *val1 = (char *)map->get(map, "hello");
int val2 = *(int *)map->get(map, "number");
bool hasVal = map->has(map, "hello"); // true

// 删除
map->delete(map, "hello");

// 删除所有
map->clear(map);
```

### 迭代器

```c
// 创建
HashMapIterator iterator = createHashMapIterator(map);

// 根据hashcode的顺序进行迭代
while (iterator->hasNext(iterator)) {
    iterator = iterator->next(iterator);
    char *key = (char *)iterator->node->key;
    char *value = (char *)iterator->node->value;
}

// 销毁
deleteHashMapIterator(&iterator);
```
