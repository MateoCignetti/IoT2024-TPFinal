idf_component_register(SRCS "main.c" "ds18x20.c" "onewire.c" "webserver.c" "wifi.c"
                    INCLUDE_DIRS "../include"
                    EMBED_TXTFILES "view.html")
