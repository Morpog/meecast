#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "main.h"

// Optimizations might wipe out our functions without this
#define KEEPALIVE __attribute__((used))

/* Common */
ConfigEfl *config;
int config_is_ready = 0;


extern "C" {
    extern void prepare_config_js();

    void EMSCRIPTEN_KEEPALIVE prepareconfig() {
        config_is_ready = 1;
    }
}

ConfigEfl *
create_and_fill_config(){
    ConfigEfl *config;

    std::cerr<<"Create Config class: " << std::endl;
    try{
        config = ConfigEfl::Instance(Core::AbstractConfig::getConfigPath()+
                                       "config.xml",
                                       Core::AbstractConfig::prefix+
                                       Core::AbstractConfig::schemaPath+
                                       "config.xsd");
    }
    catch(const std::string &str){
        std::cerr<<"Error in Config class: "<< str <<std::endl;
        config =  ConfigEfl::Instance();
    }
    catch(const char *str){
        std::cerr<<"Error in Config class: "<< str <<std::endl;
        config =  ConfigEfl::Instance();
    }
    config->saveConfig();
    fprintf(stderr,"Enf of creating Config class");
    return config;
}

void mainLoop(void)
{
//        printf("%d\n", config_is_ready);
        if (config_is_ready){
            config_is_ready = 0;
            config = create_and_fill_config();
        }
}

int 
main(int argc, char *argv[])
{
    prepare_config_js();
    fprintf(stderr, "11111111\n");
    emscripten_set_main_loop(mainLoop, 0, 1);
    fprintf(stderr, "22222222\n");
    //my_js();
	return 0;
}
