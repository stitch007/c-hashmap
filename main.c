#include <stdio.h>
#include "hashmap.h"

int main() {
    HashMap map = createHashMap();
    map->set(map, "key", "No cross, no crown.");
    map->set(map, "fuck", "Stars can’t shine without darkness.");
    map->set(map, "handsome", "To laugh at yourself is to love yourself.");
    map->set(map, "w1", "Don't compare your life with others. There's no comparison between the sun and the moon. They shine when it's their time.");
    map->set(map, "&", "I am not afraid of tomorrow for I have seen yesterday and love today.");
    int number = 10086;
    map->set(map, "number", &number);
    map->set(map, "password", "',_dp9,DKimr\\H%Z6\\IJB=4naj?^#K)lkc^@,KN'rt&H'fiGHQrd>fp3Qjm\"J(2l");
    map->set(map, "=&,qQQ$f.k^c7r]ap[/)[Y2[5LR726t%", "random");
    map->set(map, "123456", "hello world!");
    printf("size:%d cap:%d\n", map->size, map->capacity);
    HashMapIterator iterator = createHashMapIterator(map);
    while (iterator->hasNext(iterator)) {
        iterator = iterator->next(iterator);
        if (strcmp(iterator->node->key, "number") == 0) {
            printf("{ key: %s, value: %d, hashcode: %d }\n",
                   (char *) iterator->node->key,
                   *(int *) iterator->node->value,
                   iterator->hashCode);
        } else {
            printf("{ key: %s, value: %s, hashcode: %d }\n",
                   (char *) iterator->node->key,
                   (char *) iterator->node->value,
                   iterator->hashCode);
        }
    }
    deleteHashMapIterator(&iterator);
    map->clear(map);
    map->set(map, "key", "value");
    printf("size:%d cap:%d\n", map->size, map->capacity);
    deleteHashMap(&map);
    return 0;
}
