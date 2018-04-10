{
  "targets": [
    {
      "target_name": "elca",
      "sources": [
        "src/elca.cc",
        "src/callback.cc",
        "src/datatype.cc",
        "src/friend.cc",
        "src/nodeinfo.cc",
        "src/utility.cc",
      ],
      "include_dirs": ["<!(echo $CARRIER_NATIVE_SDK)/include"],
      "conditions": [
        [ 'OS=="mac"', {
          "libraries": [
            "-L<!(echo $CARRIER_NATIVE_SDK)/lib",
            # "-L<(module_root_dir)/<!(echo $CARRIER_NATIVE_SDK)/lib",
            "libelasession.a",
            "libelacarrier.a",
            "libelacommon.a",
            "libtoxcore.a",
            "libsodium.a",
            "libflatcc.a",
            "libflatccrt.a",
            "libpjmedia.a",
            "libpjnath.a",
            "libpjlib-util.a",
            "libpj.a",
            "libconfuse.a",
            "-lm"
          ],
        }],
        [ 'OS=="linux"', {
          "libraries": [
            "-L<!(echo $CARRIER_NATIVE_SDK)/lib",
            # "-L<(module_root_dir)/<!(echo $CARRIER_NATIVE_SDK)/lib",
            "-l:libelasession.a",
            "-l:libelacarrier.a",
            "-l:libelacommon.a",
            "-l:libtoxcore.a",
            "-l:libsodium.a",
            "-l:libflatcc.a",
            "-l:libflatccrt.a",
            "-l:libpjmedia.a",
            "-l:libpjnath.a",
            "-l:libpjlib-util.a",
            "-l:libpj.a",
            "-l:libconfuse.a",
	          "-lm"
          ],
        }],
        [ 'OS=="win"', {
        }]
      ],
    }
  ]
}