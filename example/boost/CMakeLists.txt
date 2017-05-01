IF (MSGPACK_BOOST)
    LIST (APPEND exec_PROGRAMS
        msgpack_variant_capitalize.cpp
        msgpack_variant_mapbased.cpp
    )
ENDIF ()

FOREACH (source_file ${exec_PROGRAMS})
    GET_FILENAME_COMPONENT (source_file_we ${source_file} NAME_WE)
    ADD_EXECUTABLE (
        ${source_file_we}
        ${source_file}
    )
    TARGET_LINK_LIBRARIES (${source_file_we}
        MsgPack::msgpack-cpp
    )
ENDFOREACH ()
