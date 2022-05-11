/*
* This work is licensed under the terms of the MIT license.  
* For a copy, see <https://opensource.org/licenses/MIT>.
*
* Developed by NISLAB - Network and Information Security Laboratory
* at George Emil Palade University of Medicine, Pharmacy, Science and
* Technology of Târgu Mureş <https://nislab.umfst.ro/>
*
* Contributors: Bela Genge
*/

#include "Config.h"
#include <string.h>

const char* getParameter(const char* cfgFile, const char* parameter) { 
    const char* str;
    const char* ret;

    config_t cfg;
    config_init(&cfg);

    if (!config_read_file(&cfg, cfgFile)) {
        return nullptr;
    }

    if (!config_lookup_string(&cfg, parameter, &str)) { 
        return nullptr;
    }

    ret = strdup(str);
    config_destroy(&cfg);
    return ret;
}
