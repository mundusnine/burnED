#define NOBUILD_IMPLEMENTATION
#include <nobuild.h>

#include <stdint.h>

#define HEAP_SIZE 1024 * 1024 * 1024
static uint8_t *heap = NULL;
static size_t heap_top = 0;

static void *allocate(size_t size) {
	size_t old_top = heap_top;
	heap_top += size;
	assert(heap_top <= HEAP_SIZE);
	return &heap[old_top];
}

#ifdef _WIN32
const char* libs[] = {
    "gdi32",
    "winmm"
};
#elif __APPLE__
const char* libs[] = {
    "Cocoa",
    "AudioToolbox"
};
#else
const char* libc = "x86_64-linux";
const char* libs[] = {
    "X11",
    "asound"
};
#endif

Cstr_Array libraries = {
    .elems = libs,
    .count = sizeof(libs)/sizeof(char*)
};

Cstr_Array includeDirs = {0};
Cstr_Array defines = {0};

Cstr_Array make_compile_line(void){
    // "-MJ","compile_commands.json" waiting for https://github.com/ziglang/zig/issues/9323 to be fixed
    #ifdef _WIN32
    Cstr_Array comp = cstr_array_make("Tools/FoundryTools_windows_x64/zig.exe","cc");
    #else
    // char *home_path = getenv("HOME");
    // char zig_cache[260] = {0} ;
    // snprintf(zig_cache,260,"%s/.cache/zig",home_path);
    // RM(zig_cache);
    // add back when gen fragment is fixed:Cstr_Array comp = CSTR_ARRAY_MAKE("Tools/FoundryTools_linux_x64/zig","cc","-gen-cdb-fragment-path","cdb");
    Cstr_Array comp = CSTR_ARRAY_MAKE("Tools/FoundryTools_linux_x64/zig","cc");
    // Cstr_Array comp = CSTR_ARRAY_MAKE("clang","-lm","-gen-cdb-fragment-path","cdb");
    comp = cstr_array_append(comp,"-L/usr/lib64");
    #endif

    // defines = cstr_array_append(defines,"__STDC_LIMIT_MACROS_DEFINED_BY_CLANG");
    for(int i = 0; i < defines.count;++i){
        size_t size = strlen(defines.elems[i]) + 3;
        char* temp = allocate(size);
        snprintf(temp,size,"-D%s",defines.elems[i]);
        comp = cstr_array_append(comp,temp);
    }

    includeDirs = cstr_array_append(includeDirs,"Libraries/fenster");
    includeDirs = cstr_array_append(includeDirs,"Libraries/microui/src");
    includeDirs = cstr_array_append(includeDirs,"Libraries/stb");

    for(int i = 0; i < includeDirs.count;++i){
        size_t size = strlen(includeDirs.elems[i]) + 3;
        char* temp = allocate(size);
        snprintf(temp,size,"-I%s",includeDirs.elems[i]);
        comp = cstr_array_append(comp,temp);
    }
    for(int i = 0; i < libraries.count;++i){
        #ifdef __APPLE__
        const char* link_name = "-framework";
        #else
        const char* link_name = "-l";
        #endif
        size_t size = strlen(libraries.elems[i]) + strlen(link_name)+1;
        char* temp = allocate(size);
        snprintf(temp,size,"%s%s",link_name,libraries.elems[i]);
        comp = cstr_array_append(comp,temp);
    }

    return comp;
}

void build_tools(void)
{
    // MKDIRS("build", "tools");
    // CMD("clang", COMMON_CFLAGS, "-o", "./build/tools/png2c", "./tools/png2c.c", "-lm");
    // CMD("clang", COMMON_CFLAGS, "-o", "./build/tools/obj2c", "./tools/obj2c.c", "-lm");
}

void build_assets(void)
{
    // MKDIRS("build", "assets");
    // CMD("./build/tools/png2c", "-n", "tsodinPog", "-o", "./build/assets/tsodinPog.c", "./assets/tsodinPog.png");
    // CMD("./build/tools/png2c", "-n", "tsodinCup", "-o", "./build/assets/tsodinCup.c", "./assets/tsodinCup.png");
    // CMD("./build/tools/png2c", "-n", "oldstone", "-o", "./build/assets/oldstone.c", "./assets/oldstone.png");
    // CMD("./build/tools/png2c", "-n", "lavastone", "-o", "./build/assets/lavastone.c", "./assets/lavastone.png");
    // CMD("./build/tools/obj2c", "-o", "./build/assets/tsodinCupLowPoly.c", "./assets/tsodinCupLowPoly.obj");
    // CMD("./build/tools/obj2c", "-s", "0.40", "-o", "./build/assets/utahTeapot.c", "./assets/utahTeapot.obj");
}

void build_tests(void)
{
    // CMD("clang", COMMON_CFLAGS, "-fsanitize=memory", "-o", "./build/test", "test.c", "-lm");
}

void usage(const char *program)
{
    INFO("Usage: %s [<subcommand>]", program);
    INFO("Subcommands:");
    INFO("    clangd");
    INFO("        clangd'eznuts i.e. Output compile_commands.json.");
    INFO("    tools");
    INFO("        Build all the tools. Things like png2c, obj2c, etc.");
    INFO("    assets");
    INFO("        Build the assets in the assets/ folder.");
    INFO("        Basically convert their data to C code so we can bake them in the editor.");
    INFO("    test[s] [<args>]");
    INFO("        Build and run test.c");
    INFO("        If <args> are provided the test utility is run with them.");
    INFO("    help");
    INFO("         Print this message");
}
void foreach_file_in_dir(const char *dirpath,void (*func)(const char*,const char*,void*),void* udata) {
    do {                                                
        struct dirent *dp = NULL;                       
        DIR *dir = opendir(dirpath);                    
        if (dir == NULL) {                              
            PANIC("could not open directory %s: %s",    
                  dirpath, nobuild__strerror(errno));   
        }                                               
        errno = 0;                                      
        while ((dp = readdir(dir))) {                   
            const char *file = dp->d_name;
            func(dirpath,file,udata);            
          
        }                                                                                      
                                                        
        if (errno > 0) {                                
            PANIC("could not read directory %s: %s",    
                  dirpath, nobuild__strerror(errno));   
        }                                               
                                                        
        closedir(dir);                                  
    } while(0);
}

static void addSources(const char* dirpath,const char* file, void* udata){
        Cmd* cmd = (Cmd*)udata;
        if (cstr_ends_with(file,".c")) {
            size_t size = strlen(dirpath)+strlen(file)+2;
            char* filename = allocate(size);
            snprintf(filename,size,"%s%s%s",dirpath,PATH_SEP,file);
            cmd->line = cstr_array_append(cmd->line,filename);
        }
}
static void liloAndStich(const char* dirpath,const char* file, void* udata){
    if(strcmp(file,".") == 0 || strcmp(file,"..") == 0 ) return;
    cJSON* root = (cJSON*) udata;
    char filename[260] = {0};
    snprintf(filename,260,"%s%s%s",dirpath,PATH_SEP,file);
    FILE* f = fopen(filename,"rb+");
    fseek(f,0,SEEK_END);
    size_t size = ftell(f);
    fseek(f,0,SEEK_SET);
    char* contents = allocate(size);
    fread(contents,sizeof(char),size,f);
    fclose(f);
    cJSON* item = cJSON_Parse(contents);
    cJSON_AddItemToArray(root,item);
}
void build_editor(int is_musl){
    MKDIRS("Deployment");

    Cmd cmd = {0};
    cmd.line = make_compile_line();

    cmd.line = cstr_array_append(cmd.line,"--debug");
    cmd.line = cstr_array_append(cmd.line,"-o");
    cmd.line = cstr_array_append(cmd.line,"./Deployment/burnED");

    foreach_file_in_dir("./src",addSources,&cmd);
    foreach_file_in_dir("Libraries/microui/src",addSources,&cmd);
    
    // FOREACH_FILE_IN_DIR(file, , {

    // });

    cmd.line = cstr_array_append(cmd.line,"-fno-sanitize=undefined");

    if(is_musl){
        cmd.line = cstr_array_append(cmd.line,"-target");
        cmd.line = cstr_array_append(cmd.line,libc);
    }

    INFO("CMD: %s", cmd_show(cmd));
    cmd_run_sync(cmd);
    FILE* has_cdb = fopen("./cdb","r");
    if(has_cdb != NULL){
        fclose(has_cdb);
        cJSON* json = cJSON_Parse("[]");
        foreach_file_in_dir("./cdb",liloAndStich,json);
        char* out = cJSON_Print(json);
        FILE* f = fopen("./compile_commands.json","wb+");
        size_t f_size = strlen(out);
        fwrite(out,sizeof(char),f_size,f);
        fclose(f);
        RM("./cdb");
    }
} 

int main(int argc, char** argv){

    heap = (uint8_t *)malloc(HEAP_SIZE);
    memset(heap,0,HEAP_SIZE);
	assert(heap != NULL);

    int link_musl = 0;
    const char *program = shift_args(&argc, &argv);
    if (argc > 0) {
        const char *subcmd = shift_args(&argc, &argv);
        if(strcmp(subcmd, "musl") == 0){
            link_musl = 1;
        }
        else if (strcmp(subcmd, "tools") == 0) {
            build_tools();
        }
        else if (strcmp(subcmd, "assets") == 0) {
            build_assets();
        }
    }
    build_editor(link_musl);

    return 0;
}