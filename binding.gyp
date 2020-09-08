{
    "targets": [
        {
            "target_name": "carrier_addon",
            "sources": [
                "init.cc",
                "carrier.cc",
                "carrierUtils.cc",
                "carrierHandler.cc",
                "<(module_root_dir)/include/ela_carrier.h"
            ],
            "library_dirs": [
                "<(module_root_dir)/lib"
            ],
            "libraries": [
                "-lelacarrier"
            ],
            "copies": [
                {
                    "destination": "<(module_root_dir)/build/Release/",
                    "files": [
                        "<(module_root_dir)/lib/elacarrier.dll",
                        "<(module_root_dir)/lib/crystal.dll",
                        "<(module_root_dir)/lib/pthreadVC2.dll"
                    ]
                }
            ]
        }
    ]
}