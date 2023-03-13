#include "../h/MemoryBuddy.hpp"
#include "../h/slab.hpp"
#include "../h/printing.hpp"

void buddyPrint(){
    printString("\n BUDDY: \n");
    for (int i=0; i<=MemoryBuddy::instance().expMax-MemoryBuddy::expMin;i++) {
        printString("Velicina 2^");
        printInt(MemoryBuddy::expMin+i);
        printString(":");
        printInt((size_t) MemoryBuddy::instance().freeLists[i]);
        printString("\n");
    }
}

void buddyTest1(){
    buddyPrint();
    int* pr1 = (int*)MemoryBuddy::instance().alloc(12);
    *pr1 = 5;
    printInt((size_t)*pr1);
    buddyPrint();
    MemoryBuddy::instance().free(pr1, 12);
    buddyPrint();
}

void buddyTest2(){
    buddyPrint();
    int* pr1 = (int*)MemoryBuddy::instance().alloc(12);
    int* pr2 = (int*)MemoryBuddy::instance().alloc(13);
    int* pr3 = (int*)MemoryBuddy::instance().alloc(13);
    *pr1 = 5;
    *pr2 = 6;
    *pr3 = 6;
    printInt((size_t)*pr1);
    buddyPrint();
    MemoryBuddy::instance().free(pr1, 12);
    buddyPrint();
}

void buddyTestMaxMem(){
    buddyPrint();
    int* pr = (int*)5, i = 0;
    while(pr) {
        i++;
        pr = (int *) MemoryBuddy::instance().alloc(12);
    }
    buddyPrint();
    while(i) {
        i--;
        MemoryBuddy::instance().free((void*)((char*)HEAP_START_ADDR+i*(1<<12)),12);
    }
    buddyPrint();
}

void slabTest1() {
    kmem_cache_t* cache = kmem_cache_create("kes1", sizeof(char), nullptr, nullptr);
    buddyPrint();
    kmem_cache_info(cache);
    char **c = new char*[29112];
    for (int i = 0; i < 29112; i++) {
        c[i] = (char *) kmem_cache_alloc(cache);
        *c[i] = 'a';
    }
    //*c = 'b';
    //printInt(*c);
    buddyPrint();
    kmem_cache_info(cache);
    for (int i = 0; i < 29112; i++) {
        //printInt(*c[i]);
        //kmem_cache_free(cache, c[i]);
    }
    kmem_cache_shrink(cache);
    kmem_cache_info(cache);
    kmem_cache_destroy(cache);
    buddyPrint();
}

void slabTest2() {
    kmem_cache_t* cache = kmem_cache_create("kes2", sizeof(int), nullptr, nullptr);
    buddyPrint();
    kmem_cache_info(cache);
    int **c = new int*[500000];
    for (int i = 0; i < 500000; i++) {
        c[i] = (int *) kmem_cache_alloc(cache);
        *c[i] = 5;
        //printInt((size_t)c);
    }
    //*c = 'b';
    //printInt(*c);
    buddyPrint();
    kmem_cache_info(cache);
    for (int i = 0; i < 500000; i++) {
        //printInt(*c[i]);
        kmem_cache_free(cache, c[i]);
    }
    kmem_cache_shrink(cache);
    kmem_cache_info(cache);
    kmem_cache_destroy(cache);
    buddyPrint();
}

void cacheTest1() {
    //printInt(sizeof(int)); 4
    buddyPrint();
    int* niz = (int*)kmalloc(sizeof(int)*8);
    niz[0] = 5;
    niz[1] = 4;
    buddyPrint();
    printInt(niz[0]);
    printString(" ");
    printInt(niz[1]);
    printString(" ");
    printInt(niz[2]);
    kfree(niz);
    buddyPrint();
}

void cacheTest2() {
    //printInt(sizeof(int)); 4
    buddyPrint();
    int** niznizova = (int**) kmalloc(sizeof(int*)*10);
    buddyPrint();
    for (int i = 0; i < 10; i++){
        niznizova[i] = (int*)kmalloc(sizeof(int)*8);
        buddyPrint();
        niznizova[i][0] = i;
        niznizova[i][1] = i+1;
        niznizova[i][2] = i+2;
        niznizova[i][3] = i+3;
        niznizova[i][4] = i+4;
        niznizova[i][5] = i+5;
        niznizova[i][6] = i+6;
        niznizova[i][7] = i+7;
    }
    buddyPrint();
    for (int i = 0; i < 8; i++) {
        printInt(niznizova[5][i]);
        printString(" ");
    }
    kfree(niznizova);
    buddyPrint();
    for (int i = 0; i < 10; i++){
        kfree(niznizova[i]);
        buddyPrint();
    }
}
extern void test1();
extern void test2();

void userMain() {

    //test1();
    test2();
}