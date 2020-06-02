struct obj1
{
    char a[0x2000];
};

struct fix_object
{
    int is_use;
    struct page_struct *obj;
};

int fix_object_count;
int fix_obj_alloc();
int fix_obj_free(int free_index);
struct fix_object fix_object_array[100];
